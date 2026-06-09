//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

/**
 * @file relative_angle_calculator.cpp
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

#include "uflex/domain/services/relative_angle_calculator.h"

#include <math.h>

namespace {

constexpr float kRadiansToDegrees = 57.2957795f;

float calculatePitchDegrees(const ImuSample& sample) {
    return atan2f(static_cast<float>(sample.accelX),
                  sqrtf(static_cast<float>(sample.accelY) * static_cast<float>(sample.accelY) +
                        static_cast<float>(sample.accelZ) * static_cast<float>(sample.accelZ))) *
           kRadiansToDegrees;
}

float calculateRollDegrees(const ImuSample& sample) {
    return atan2f(static_cast<float>(sample.accelY), static_cast<float>(sample.accelZ)) *
           kRadiansToDegrees;
}

} // namespace

RelativeAngle RelativeAngleCalculator::calculate(const ImuSample& first,
                                                 const ImuSample& second) {
    const float firstPitch = calculatePitchDegrees(first);
    const float secondPitch = calculatePitchDegrees(second);
    const float firstRoll = calculateRollDegrees(first);
    const float secondRoll = calculateRollDegrees(second);

    return RelativeAngle{
        secondPitch - firstPitch,
        secondRoll - firstRoll,
    };
}
