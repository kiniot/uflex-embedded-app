//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_DOMAIN_SERVICES_RELATIVE_ANGLE_CALCULATOR_H
#define UFLEX_DOMAIN_SERVICES_RELATIVE_ANGLE_CALCULATOR_H

#include "uflex/domain/sensors/imu_sample.h"

/**
 * @file relative_angle_calculator.h
 * @brief Declares domain-level relative angle calculations for uFlex.
 *
 * This service concentrates the math used to derive pitch and roll deltas from
 * pairs of IMU samples so the rest of the domain can work with motion-oriented
 * values instead of repeating low-level trigonometric logic.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
struct RelativeAngle {
    float pitchDegrees;
    float rollDegrees;
};

/**
 * @brief Provides relative angle calculations derived from IMU samples.
 */
class RelativeAngleCalculator {
public:
    /**
     * @brief Calculates the relative pitch and roll between two IMU samples.
     */
    static RelativeAngle calculate(const ImuSample& first, const ImuSample& second);
};

#endif // UFLEX_DOMAIN_SERVICES_RELATIVE_ANGLE_CALCULATOR_H
