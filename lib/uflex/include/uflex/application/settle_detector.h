//
// Created by Salim Ramirez Mestanza on 28/06/26.
//

#ifndef UFLEX_APPLICATION_SETTLE_DETECTOR_H
#define UFLEX_APPLICATION_SETTLE_DETECTOR_H

#include "uflex/domain/sensors/imu_sample.h"

/**
 * @file settle_detector.h
 * @brief Detects that a limb is held still, by watching raw gyroscope magnitude.
 *
 * Used to gate the session-zero calibration: instead of capturing the zero at an
 * arbitrary poll tick (which may land mid-motion and anchor a wrong reference),
 * the firmware waits until the gyro reports stillness for a sustained number of
 * cycles, aligning the capture with the patient actually holding the pose.
 *
 * @author Salim Ramirez
 * @date June 28, 2026
 * @version 0.1.0
 */

/**
 * @brief Euclidean magnitude of a sample's raw gyroscope vector (LSB units).
 */
float gyroMagnitude(const ImuSample& sample);

class SettleDetector {
public:
    /**
     * @param thresholdLsb Gyro magnitude at or below which a cycle counts as still.
     * @param requiredCycles Consecutive still cycles needed before reporting settled.
     */
    SettleDetector(float thresholdLsb, int requiredCycles);

    /**
     * @brief Feeds one cycle's gyro magnitude; returns the current settled() state.
     *
     * A magnitude at or below the threshold advances the still-cycle counter; any
     * value above it resets the counter to zero (motion breaks the streak).
     */
    bool update(float gyroMagnitudeLsb);

    /**
     * @brief Whether enough consecutive still cycles have accumulated.
     */
    bool settled() const;

    /**
     * @brief Clears the still-cycle counter (call when a new settle window starts).
     */
    void reset();

private:
    float thresholdLsb;
    int requiredCycles;
    int stillCycles = 0;
};

#endif // UFLEX_APPLICATION_SETTLE_DETECTOR_H
