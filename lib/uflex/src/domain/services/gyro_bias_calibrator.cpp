//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

/**
 * @file gyro_bias_calibrator.cpp
 * @brief Implements the stationary gyroscope bias estimator.
 *
 * Sums each axis in a 32-bit accumulator and divides by the count at the end. The
 * accumulator cannot overflow for realistic batches: even targetSamples * INT16_MAX
 * (e.g. 100 * 32767 ~= 3.3M) is far below INT32_MAX.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */

#include "uflex/domain/services/gyro_bias_calibrator.h"

GyroBiasCalibrator::GyroBiasCalibrator(uint16_t targetSamples)
    : targetSamples(targetSamples), collected(0), sumX(0), sumY(0), sumZ(0) {}

bool GyroBiasCalibrator::addSample(int16_t gyroX, int16_t gyroY, int16_t gyroZ) {
    if (collected >= targetSamples) {
        return true;
    }
    sumX += gyroX;
    sumY += gyroY;
    sumZ += gyroZ;
    ++collected;
    return collected >= targetSamples;
}

bool GyroBiasCalibrator::isComplete() const {
    return collected >= targetSamples;
}

void GyroBiasCalibrator::getBias(int16_t& biasX, int16_t& biasY, int16_t& biasZ) const {
    if (collected == 0) {
        biasX = 0;
        biasY = 0;
        biasZ = 0;
        return;
    }
    biasX = static_cast<int16_t>(sumX / collected);
    biasY = static_cast<int16_t>(sumY / collected);
    biasZ = static_cast<int16_t>(sumZ / collected);
}

void GyroBiasCalibrator::reset() {
    collected = 0;
    sumX = 0;
    sumY = 0;
    sumZ = 0;
}
