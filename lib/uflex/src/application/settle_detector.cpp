//
// Created by Salim Ramirez Mestanza on 28/06/26.
//

/**
 * @file settle_detector.cpp
 * @brief Implements the gyroscope-based stillness detector.
 *
 * @author Salim Ramirez
 * @date June 28, 2026
 * @version 0.1.0
 */

#include "uflex/application/settle_detector.h"

#include <math.h>

float gyroMagnitude(const ImuSample& sample) {
    const float x = static_cast<float>(sample.gyroX);
    const float y = static_cast<float>(sample.gyroY);
    const float z = static_cast<float>(sample.gyroZ);
    return sqrtf(x * x + y * y + z * z);
}

SettleDetector::SettleDetector(float thresholdLsb, int requiredCycles)
    : thresholdLsb(thresholdLsb), requiredCycles(requiredCycles) {}

bool SettleDetector::update(float gyroMagnitudeLsb) {
    if (gyroMagnitudeLsb <= thresholdLsb) {
        if (stillCycles < requiredCycles) {
            ++stillCycles;
        }
    } else {
        stillCycles = 0;
    }
    return settled();
}

bool SettleDetector::settled() const {
    return stillCycles >= requiredCycles;
}

void SettleDetector::reset() {
    stillCycles = 0;
}
