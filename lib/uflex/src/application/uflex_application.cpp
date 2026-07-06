//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/application/uflex_application.h"

#include <Arduino.h>
#include <string.h>

#include "config/build_config.h"
#include "uflex/domain/actuators/active_buzzer.h"
#include "uflex/domain/actuators/rgb_led.h"
#include "uflex/domain/actuators/vibration_motor.h"
#include "uflex/domain/devices/motion_state.h"
#include "uflex/domain/math/quaternion.h"
#include "uflex/infrastructure/transport/ble_motion_telemetry_mapper.h"
#include "uflex/infrastructure/transport/motion_payload.h"
#include "uflex/infrastructure/transport/motion_payload_mapper.h"
#include "uflex/infrastructure/transport/motion_payload_serializer.h"
#include "uflex/infrastructure/transport/sample_batch_payload.h"

/**
 * @file uflex_application.cpp
 * @brief Implements the top-level application coordinator for uFlex.
 *
 * UflexApplication keeps the firmware entry point small by owning the main
 * loop cadence, delegating environment-specific updates to a runtime, and
 * formatting serial output from the current motion state and motion payload.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

UflexApplication::UflexApplication(UflexRuntime& runtime)
    : runtime(runtime),
      lastReadAt(0),
      lastEdgePublishAt(0),
      lastOrientationUpdateAt(0),
      lastSerialLogAt(0),
      lastDownChannelPollAt(0),
      lastContextOkAt(0),
      hasOrientationBaseline(false),
      bleSequenceNumber(0),
      lastCalibratedSerieId{},
      pendingCalibrationSerieId{},
      calibrationPending(false),
      calibrationPendingSince(0),
      settleDetector(SETTLE_GYRO_THRESHOLD_LSB, SETTLE_REQUIRED_CYCLES),
      lastSafetyTriggered(false) {}

void UflexApplication::begin() {
    Serial.printf("uFlex motion probe (%s target)\n", UFLEX_BUILD_TARGET_NAME);
    runtime.begin();
    pulseBuzzer(2);
    Serial.println();
}

void UflexApplication::loop() {
    const unsigned long now = millis();

    if (now - lastReadAt < READ_INTERVAL_MS) {
        return;
    }

    lastReadAt = now;

    if (runtime.update()) {
        advanceOrientationFilters();

        const MotionState motionState = runtime.getDevice().getMotionState();
        const MotionPayload motionPayload = MotionPayloadMapper::map(motionState);

        pollActiveContextIfDue(now);

        // A context is only "alive" if a poll succeeded recently (TTL). On loss this
        // disarms safety and drops the stale zero; while a new serie waits to settle,
        // it captures the zero (serviceCalibration) before the angle is computed.
        const bool ctxAlive = contextAlive(now);
        serviceCalibration(motionState, ctxAlive, now);

        const float targetAngle = computeTargetAngle(motionState);
        const float proximalSignal = yawDegrees(runtime.getDevice().getUpperOrientation());

        // Local safety reaction runs every cycle and reaches the pins immediately,
        // independent of the network (it uses the cached active context). Armed only
        // with a fresh context + valid zero; latched with hysteresis.
        updateSafety(targetAngle, ctxAlive);
        runtime.applyOutputs();

        publishBleTelemetry(motionState, targetAngle, jointAngleCalculator.isCalibrated(),
                            static_cast<uint8_t>(activeContext.activeJoint));
        publishToEdgeIfDue(targetAngle, proximalSignal);
        logAllSamplesIfDue(motionState, motionPayload, targetAngle, proximalSignal);
    }
}

void UflexApplication::advanceOrientationFilters() {
    const unsigned long now = millis();

    if (!hasOrientationBaseline) {
        lastOrientationUpdateAt = now;
        hasOrientationBaseline = true;
        return;
    }

    const float deltaTimeSeconds = static_cast<float>(now - lastOrientationUpdateAt) / 1000.0f;
    lastOrientationUpdateAt = now;

    runtime.getDevice().updateOrientations(deltaTimeSeconds);
}

void UflexApplication::publishBleTelemetry(const MotionState& motionState, float jointFlexionDegrees,
                                           bool isCalibrated, uint8_t activeJoint) {
    UflexDevice& device = runtime.getDevice();
    const BleMotionTelemetry telemetry = BleMotionTelemetryMapper::map(
        motionState, device.getStatusLed().getColor(), device.getStatusBuzzer().isEnabled(),
        device.getVibrationMotor().isEnabled(), bleSequenceNumber, jointFlexionDegrees, isCalibrated,
        activeJoint);
    ++bleSequenceNumber;

    BleTransport& bleTransport = runtime.getBleTransport();
    if (bleTransport.isReady()) {
        bleTransport.publish(telemetry);
    }
}

void UflexApplication::logSample(const char* label, const ImuSample& sample, uint8_t address) {
    Serial.printf("%s [0x%02X] AX=%d AY=%d AZ=%d TEMP=%d GX=%d GY=%d GZ=%d\n", label, address,
                  sample.accelX, sample.accelY, sample.accelZ, sample.temperature, sample.gyroX,
                  sample.gyroY, sample.gyroZ);
}

void UflexApplication::pulseBuzzer(size_t pulseCount) {
    for (size_t pulseIndex = 0; pulseIndex < pulseCount; ++pulseIndex) {
        runtime.getDevice().handle(ActiveBuzzer::TURN_ON_COMMAND);
        runtime.applyOutputs();
        delay(BUZZER_PULSE_MS);

        runtime.getDevice().handle(ActiveBuzzer::TURN_OFF_COMMAND);
        runtime.applyOutputs();

        if (pulseIndex + 1 < pulseCount) {
            delay(BUZZER_PULSE_GAP_MS);
        }
    }
}

void UflexApplication::logAllSamplesIfDue(const MotionState& motionState,
                                          const MotionPayload& motionPayload,
                                          float targetAngleDegrees, float proximalSignalDegrees) {
    const unsigned long now = millis();
    if (now - lastSerialLogAt < SERIAL_LOG_INTERVAL_MS) {
        return;
    }
    lastSerialLogAt = now;

    logSample("imu1", motionState.upperSample, runtime.getDevice().getUpperImu().getI2cAddress());
    logSample("imu2", motionState.middleSample,
              runtime.getDevice().getMiddleImu().getI2cAddress());
    logSample("imu3", motionState.lowerSample, runtime.getDevice().getLowerImu().getI2cAddress());

    char payloadBuffer[MOTION_PAYLOAD_BUFFER_SIZE] = {};

    Serial.printf("upper-middle: pitch=%.2f roll=%.2f\n",
                  motionPayload.upperMiddleAngle.pitchDegrees,
                  motionPayload.upperMiddleAngle.rollDegrees);
    Serial.printf("middle-lower: pitch=%.2f roll=%.2f\n",
                  motionPayload.middleLowerAngle.pitchDegrees,
                  motionPayload.middleLowerAngle.rollDegrees);
    Serial.printf("upper-lower: pitch=%.2f roll=%.2f\n",
                  motionPayload.upperLowerAngle.pitchDegrees,
                  motionPayload.upperLowerAngle.rollDegrees);

    // The enriched sample actually sent to the edge: the active-joint angle and the proximal-segment
    // (bicep, upper) yaw. proximal_signal should be steady at rest (mag-anchored on ch1) and swing
    // when the upper arm rotates -> feeds the edge's compensation detector.
    Serial.printf("edge sample: target_angle=%.2f proximal_signal(bicep yaw)=%.2f\n",
                  targetAngleDegrees, proximalSignalDegrees);

    if (MotionPayloadSerializer::toJson(motionPayload, payloadBuffer, sizeof(payloadBuffer)) >=
        0) {
        Serial.printf("payload: %s\n", payloadBuffer);
    } else {
        Serial.println("payload: serialization failed");
    }

    Serial.println();
}

void UflexApplication::publishToEdgeIfDue(float targetAngleDegrees, float proximalSignalDegrees) {
    const unsigned long now = millis();
    if (now - lastEdgePublishAt < EDGE_PUBLISH_INTERVAL_MS) {
        return;
    }
    lastEdgePublishAt = now;

    EdgeTransport& edgeTransport = runtime.getEdgeTransport();
    if (!edgeTransport.isReady()) {
        return;
    }

    // First cut: a batch of one at ~10Hz. Growing the batch needs no edge change.
    const JointSample sample{targetAngleDegrees, proximalSignalDegrees};
    const SampleBatchPayload payload{UFLEX_SERIAL_NUMBER, &sample, 1};
    edgeTransport.publishSamples(payload);
}

void UflexApplication::pollActiveContextIfDue(unsigned long now) {
    if (now - lastDownChannelPollAt < DOWN_CHANNEL_POLL_INTERVAL_MS) {
        return;
    }
    lastDownChannelPollAt = now;

    EdgeTransport& edgeTransport = runtime.getEdgeTransport();
    if (!edgeTransport.isReady()) {
        return;
    }

    ActiveSerieContext fetched;
    if (!edgeTransport.fetchActiveContext(fetched)) {
        return; // keep the last-known context on a failed poll; the TTL disarms if it persists
    }
    activeContext = fetched;
    lastContextOkAt = now;

    // New serie: defer the zero capture until the arm settles (serviceCalibration),
    // and drop any prior zero so safety stays disarmed during the settle window
    // rather than enforcing the new ceiling against the previous serie's reference.
    if (activeContext.hasContext &&
        strcmp(activeContext.serieId, lastCalibratedSerieId) != 0 &&
        strcmp(activeContext.serieId, pendingCalibrationSerieId) != 0) {
        strncpy(pendingCalibrationSerieId, activeContext.serieId, sizeof(pendingCalibrationSerieId) - 1);
        pendingCalibrationSerieId[sizeof(pendingCalibrationSerieId) - 1] = '\0';
        calibrationPending = true;
        calibrationPendingSince = now;
        settleDetector.reset();
        jointAngleCalculator.reset();
        Serial.printf("calib: deferred (waiting for settle) serie=%s maxSafe=%.1f\n",
                      activeContext.serieId,
                      activeContext.hasMaxSafeAngle ? activeContext.maxSafeAngle : -1.0f);
    }
}

float UflexApplication::computeTargetAngle(const MotionState& motionState) {
    const RelativeAngle angle = activeJointAngle(motionState, activeContext.activeJoint);
    return jointAngleCalculator.absoluteFlexionDegrees(angle);
}

bool UflexApplication::contextAlive(unsigned long now) const {
    return activeContext.hasContext && (now - lastContextOkAt) <= CONTEXT_TTL_MS;
}

float UflexApplication::maxGyroMagnitude(const MotionState& motionState) {
    float maxMag = gyroMagnitude(motionState.upperSample);
    const float middle = gyroMagnitude(motionState.middleSample);
    const float lower = gyroMagnitude(motionState.lowerSample);
    if (middle > maxMag) maxMag = middle;
    if (lower > maxMag) maxMag = lower;
    return maxMag;
}

void UflexApplication::serviceCalibration(const MotionState& motionState, bool contextIsAlive,
                                          unsigned long now) {
    if (!contextIsAlive) {
        // Context lost (session ended) or expired (no poll within TTL): drop any zero
        // and pending capture so safety disarms instead of enforcing a stale ceiling.
        if (jointAngleCalculator.isCalibrated() || calibrationPending) {
            jointAngleCalculator.reset();
            calibrationPending = false;
            lastCalibratedSerieId[0] = '\0';
            pendingCalibrationSerieId[0] = '\0';
            Serial.println("ctx: lost/expired -> safety disarmed, calibration reset");
        }
        return;
    }

    if (!calibrationPending) {
        return;
    }

    const bool settled = settleDetector.update(maxGyroMagnitude(motionState));
    const bool timedOut = (now - calibrationPendingSince) >= CALIBRATION_SETTLE_TIMEOUT_MS;
    if (!settled && !timedOut) {
        return; // keep waiting for the arm to hold still
    }

    const RelativeAngle zeroPose = activeJointAngle(motionState, activeContext.activeJoint);
    jointAngleCalculator.calibrate(zeroPose);
    strncpy(lastCalibratedSerieId, pendingCalibrationSerieId, sizeof(lastCalibratedSerieId) - 1);
    lastCalibratedSerieId[sizeof(lastCalibratedSerieId) - 1] = '\0';
    calibrationPending = false;
    Serial.printf("calib: zeroed joint=%d serie=%s maxSafe=%.1f (%s)\n",
                  static_cast<int>(activeContext.activeJoint), lastCalibratedSerieId,
                  activeContext.hasMaxSafeAngle ? activeContext.maxSafeAngle : -1.0f,
                  settled ? "settled" : "timeout-fallback");
}

void UflexApplication::updateSafety(float targetAngleDegrees, bool contextIsAlive) {
    const bool actuate = safetyMonitor.evaluate(targetAngleDegrees, activeContext,
                                                jointAngleCalculator.isCalibrated(), contextIsAlive,
                                                SAFE_HYSTERESIS_DEG);
    applySafetyOutputs(actuate);

    if (actuate != lastSafetyTriggered) {
        if (actuate) {
            Serial.printf("safety: TRIGGER angle=%.1f >= maxSafe=%.1f\n", targetAngleDegrees,
                          activeContext.hasMaxSafeAngle ? activeContext.maxSafeAngle : -1.0f);
        } else {
            Serial.printf("safety: clear angle=%.1f\n", targetAngleDegrees);
        }
        lastSafetyTriggered = actuate;
    }
}

void UflexApplication::applySafetyOutputs(bool on) {
    // Local safety = buzzer only. The vibration motor was dropped (interfered with the magnetometer
    // and added motion noise); GPIO32 is now unused.
    UflexDevice& device = runtime.getDevice();
    device.handle(on ? ActiveBuzzer::TURN_ON_COMMAND : ActiveBuzzer::TURN_OFF_COMMAND);
}
