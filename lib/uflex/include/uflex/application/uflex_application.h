//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_UFLEX_APPLICATION_H
#define UFLEX_APPLICATION_UFLEX_APPLICATION_H

#include <cstddef>
#include <cstdint>

#include "uflex/application/joint_targeting.h"
#include "uflex/application/safety_monitor.h"
#include "uflex/application/settle_detector.h"
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
    static constexpr unsigned long BOOT_LED_MS = 1500;
    static constexpr unsigned long SLOW_BLINK_MS = 600;
    static constexpr unsigned long FAST_BLINK_MS = 160;
    static constexpr size_t MOTION_PAYLOAD_BUFFER_SIZE = 256;
    // Safety stays armed only while the active context is fresh. If no down-channel
    // poll has succeeded within this window (session ended, WiFi/edge lost), the
    // context is treated as stale -> safety disarms and the zero is dropped. Generous
    // (several missed 3s polls) so a transient blip never disarms mid-session.
    static constexpr unsigned long CONTEXT_TTL_MS = 15000;
    // Once triggered, the angle must drop this far below the ceiling to clear, so
    // sensor noise/drift at the boundary cannot chatter the buzzer.
    static constexpr float SAFE_HYSTERESIS_DEG = 5.0f;
    // Session-zero is captured only when the arm is still: gyro magnitude (LSB) at or
    // below this for SETTLE_REQUIRED_CYCLES consecutive cycles. Tuned on board: a still
    // arm reads ~20-150 LSB on healthy IMUs, but one unit carries a ~800 LSB gyro bias,
    // so the threshold clears that resting bias while staying well under real motion
    // (deliberate flexion is thousands of LSB). Re-tune if IMUs change.
    static constexpr float SETTLE_GYRO_THRESHOLD_LSB = 1200.0f;
    static constexpr int SETTLE_REQUIRED_CYCLES = 10;  // ~400ms at READ_INTERVAL_MS
    // Fallback: if the arm never settles, capture the zero anyway after this long so
    // the kit is never left un-armed for a whole serie (degrades to old behavior).
    static constexpr unsigned long CALIBRATION_SETTLE_TIMEOUT_MS = 8000;

    UflexRuntime& runtime;
    unsigned long lastReadAt;
    unsigned long lastEdgePublishAt;
    unsigned long lastOrientationUpdateAt;
    unsigned long lastSerialLogAt;
    unsigned long lastDownChannelPollAt;
    unsigned long lastContextOkAt;
    unsigned long bootIndicatorUntil;
    bool hasOrientationBaseline;
    bool runtimeStarted;
    uint16_t bleSequenceNumber;
    ActiveSerieContext activeContext;
    JointAngleCalculator jointAngleCalculator;
    char lastCalibratedSerieId[32];
    char pendingCalibrationSerieId[32];
    bool calibrationPending;
    unsigned long calibrationPendingSince;
    SafetyMonitor safetyMonitor;
    SettleDetector settleDetector;
    bool lastSafetyTriggered;

    static void logSample(const char* label, const ImuSample& sample, uint8_t address);
    static float maxGyroMagnitude(const MotionState& motionState);
    void pulseBuzzer(size_t pulseCount);
    void logAllSamplesIfDue(const MotionState& motionState, const MotionPayload& motionPayload,
                            float targetAngleDegrees, float proximalSignalDegrees);
    void publishToEdgeIfDue(float targetAngleDegrees, float proximalSignalDegrees);
    void publishBleTelemetry(const MotionState& motionState, float jointFlexionDegrees,
                             bool isCalibrated, uint8_t activeJoint);
    void advanceOrientationFilters();
    void pollActiveContextIfDue(unsigned long now);
    bool contextAlive(unsigned long now) const;
    void serviceCalibration(const MotionState& motionState, bool contextIsAlive, unsigned long now);
    float computeTargetAngle(const MotionState& motionState);
    void updateSafety(float targetAngleDegrees, bool contextIsAlive);
    void applySafetyOutputs(bool on);
    void updateStatusLed(bool contextIsAlive, unsigned long now);
    void applyBlinkingLed(RgbLed::Color color, unsigned long now, unsigned long intervalMs);
};

#endif // UFLEX_APPLICATION_UFLEX_APPLICATION_H
