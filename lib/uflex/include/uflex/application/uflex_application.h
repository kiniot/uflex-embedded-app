//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_UFLEX_APPLICATION_H
#define UFLEX_APPLICATION_UFLEX_APPLICATION_H

#include <cstddef>
#include <cstdint>

#include "uflex/application/joint_targeting.h"
#include "uflex/application/runtime/uflex_runtime.h"
#include "uflex/domain/devices/motion_state.h"
#include "uflex/domain/services/joint_angle_calculator.h"
#include "uflex/infrastructure/transport/active_serie_context.h"
#include "uflex/infrastructure/transport/ble_motion_telemetry.h"
#include "uflex/infrastructure/transport/motion_payload.h"

/**
 * @file uflex_application.h
 * @brief Declares the top-level application coordinator for uFlex.
 *
 * UflexApplication owns the main loop policy used by the firmware entry point.
 * It delegates environment-specific updates to a runtime, applies the current
 * read cadence, and formats serial diagnostics from the device motion state.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class UflexApplication {
public:
    explicit UflexApplication(UflexRuntime& runtime);

    void begin();
    void loop();

private:
    // 40ms (~25Hz) gives the Mahony filter frequent, low-drift corrections and
    // keeps the BLE avatar stream smooth.
    static constexpr unsigned long READ_INTERVAL_MS = 40;
    // ~10Hz to the edge: enough to catch a 2-4s repetition's peak cleanly while
    // staying well under the BLE rate. Local safety reacts every cycle, not on this.
    static constexpr unsigned long EDGE_PUBLISH_INTERVAL_MS = 100;
    // The active-context poll is low-frequency: the serie context changes rarely.
    static constexpr unsigned long DOWN_CHANNEL_POLL_INTERVAL_MS = 3000;
    // Serial diagnostics stay at the old ~1Hz pace: printing the full sample/payload
    // dump on every 25Hz cycle would flood the 115200-baud serial link and could
    // itself become the loop's bottleneck.
    static constexpr unsigned long SERIAL_LOG_INTERVAL_MS = 1000;
    static constexpr unsigned long BUZZER_PULSE_MS = 100;
    static constexpr unsigned long BUZZER_PULSE_GAP_MS = 100;
    static constexpr unsigned long VIBRATION_MOTOR_PULSE_MS = 250;
    static constexpr unsigned long VIBRATION_MOTOR_PULSE_GAP_MS = 250;
    static constexpr size_t MOTION_PAYLOAD_BUFFER_SIZE = 256;

    UflexRuntime& runtime;
    unsigned long lastReadAt;
    unsigned long lastEdgePublishAt;
    unsigned long lastOrientationUpdateAt;
    unsigned long lastSerialLogAt;
    unsigned long lastDownChannelPollAt;
    bool hasOrientationBaseline;
    uint16_t bleSequenceNumber;
    ActiveSerieContext activeContext;
    JointAngleCalculator jointAngleCalculator;
    char lastCalibratedSerieId[32];

    static void logSample(const char* label, const ImuSample& sample, uint8_t address);
    void pulseBuzzer(size_t pulseCount);
    void pulseVibrationMotor(size_t pulseCount);
    void logAllSamplesIfDue(const MotionState& motionState, const MotionPayload& motionPayload);
    void publishToEdgeIfDue(float targetAngleDegrees, float proximalSignalDegrees);
    void publishBleTelemetry(const MotionState& motionState);
    void advanceOrientationFilters();
    void pollActiveContextIfDue();
    float computeTargetAngle(const MotionState& motionState);
    void enforceSafety(float targetAngleDegrees);
};

#endif // UFLEX_APPLICATION_UFLEX_APPLICATION_H
