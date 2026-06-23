//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_DOMAIN_SERVICES_GYRO_BIAS_CALIBRATOR_H
#define UFLEX_DOMAIN_SERVICES_GYRO_BIAS_CALIBRATOR_H

#include <stdint.h>

/**
 * @file gyro_bias_calibrator.h
 * @brief Estimates a gyroscope's constant zero-rate bias from stationary samples.
 *
 * A MEMS gyroscope reports a small non-zero rate even at rest; integrated over a
 * session that bias becomes drift. Averaging a batch of readings taken while the
 * device is held still yields the per-axis offset to subtract from every later
 * sample. Pure (no Arduino/Wire) so it is host-unit-testable; the hardware adapter
 * feeds it raw gyro reads at startup.
 *
 * Precondition: the device MUST be stationary while samples are collected — a
 * moving device poisons the estimate.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */
class GyroBiasCalibrator {
public:
    /**
     * @brief Constructs a calibrator that averages `targetSamples` readings.
     */
    explicit GyroBiasCalibrator(uint16_t targetSamples);

    /**
     * @brief Accumulates one stationary raw-gyro triplet (LSB). Extra samples past
     * the target are ignored. Returns true once the target has been reached.
     */
    bool addSample(int16_t gyroX, int16_t gyroY, int16_t gyroZ);

    /**
     * @brief Whether the target number of samples has been collected.
     */
    bool isComplete() const;

    /**
     * @brief Writes the mean per-axis bias (raw LSB). Yields {0,0,0} before any
     * sample is added.
     */
    void getBias(int16_t& biasX, int16_t& biasY, int16_t& biasZ) const;

    /**
     * @brief Clears all accumulation, ready to calibrate again.
     */
    void reset();

private:
    uint16_t targetSamples;
    uint16_t collected;
    int32_t sumX;
    int32_t sumY;
    int32_t sumZ;
};

#endif // UFLEX_DOMAIN_SERVICES_GYRO_BIAS_CALIBRATOR_H
