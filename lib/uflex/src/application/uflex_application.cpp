//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/application/uflex_application.h"

#include <Arduino.h>

#include "config/build_config.h"
#include "uflex/domain/actuators/active_buzzer.h"
#include "uflex/domain/actuators/rgb_led.h"
#include "uflex/domain/actuators/vibration_motor.h"
#include "uflex/domain/devices/motion_state.h"
#include "uflex/infrastructure/transport/motion_payload.h"
#include "uflex/infrastructure/transport/motion_payload_mapper.h"
#include "uflex/infrastructure/transport/motion_payload_serializer.h"

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
      hasOrientationBaseline(false) {}

void UflexApplication::begin() {
    Serial.printf("uFlex motion probe (%s target)\n", UFLEX_BUILD_TARGET_NAME);
    runtime.begin();
    pulseBuzzer(2);
    pulseVibrationMotor(2);
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
        pulseBuzzer(1);
        pulseVibrationMotor(1);
        runtime.getDevice().handle(RgbLed::ADVANCE_COLOR_COMMAND);
        runtime.applyOutputs();
        logAllSamples();
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

void UflexApplication::pulseVibrationMotor(size_t pulseCount) {
    for (size_t pulseIndex = 0; pulseIndex < pulseCount; ++pulseIndex) {
        runtime.getDevice().handle(VibrationMotor::TURN_ON_COMMAND);
        runtime.applyOutputs();
        delay(VIBRATION_MOTOR_PULSE_MS);

        runtime.getDevice().handle(VibrationMotor::TURN_OFF_COMMAND);
        runtime.applyOutputs();

        if (pulseIndex + 1 < pulseCount) {
            delay(VIBRATION_MOTOR_PULSE_GAP_MS);
        }
    }
}

void UflexApplication::logAllSamples() {
    const MotionState motionState = runtime.getDevice().getMotionState();

    logSample("imu1", motionState.upperSample, runtime.getDevice().getUpperImu().getI2cAddress());
    logSample("imu2", motionState.middleSample,
              runtime.getDevice().getMiddleImu().getI2cAddress());
    logSample("imu3", motionState.lowerSample, runtime.getDevice().getLowerImu().getI2cAddress());

    const MotionPayload motionPayload = MotionPayloadMapper::map(motionState);
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

    if (MotionPayloadSerializer::toJson(motionPayload, payloadBuffer, sizeof(payloadBuffer)) >=
        0) {
        Serial.printf("payload: %s\n", payloadBuffer);
    } else {
        Serial.println("payload: serialization failed");
    }

    publishToEdgeIfDue(motionPayload);

    Serial.println();
}

void UflexApplication::publishToEdgeIfDue(const MotionPayload& motionPayload) {
    const unsigned long now = millis();
    if (now - lastEdgePublishAt < EDGE_PUBLISH_INTERVAL_MS) {
        return;
    }
    lastEdgePublishAt = now;

    EdgeTransport& edgeTransport = runtime.getEdgeTransport();
    if (edgeTransport.isReady()) {
        edgeTransport.publish(motionPayload);
    }
}
