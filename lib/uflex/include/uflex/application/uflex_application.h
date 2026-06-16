//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_UFLEX_APPLICATION_H
#define UFLEX_APPLICATION_UFLEX_APPLICATION_H

#include <cstddef>

#include "uflex/application/runtime/uflex_runtime.h"

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
    static constexpr unsigned long READ_INTERVAL_MS = 1000;
    static constexpr unsigned long BUZZER_PULSE_MS = 100;
    static constexpr unsigned long BUZZER_PULSE_GAP_MS = 100;
    static constexpr unsigned long VIBRATION_MOTOR_PULSE_MS = 250;
    static constexpr unsigned long VIBRATION_MOTOR_PULSE_GAP_MS = 250;
    static constexpr size_t MOTION_PAYLOAD_BUFFER_SIZE = 256;

    UflexRuntime& runtime;
    unsigned long lastReadAt;

    static void logSample(const char* label, const ImuSample& sample, uint8_t address);
    void pulseBuzzer(size_t pulseCount);
    void pulseVibrationMotor(size_t pulseCount);
    void logAllSamples();
};

#endif // UFLEX_APPLICATION_UFLEX_APPLICATION_H
