//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_CORE_MATH_RELATIVE_ANGLE_CALCULATOR_H
#define UFLEX_CORE_MATH_RELATIVE_ANGLE_CALCULATOR_H

#include "uflex_core/sensors/inertial_sample.h"

/**
 * @file relative_angle_calculator.h
 * @brief Declares utilities for estimating relative angles between inertial samples.
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
 * @brief Provides domain-level calculations derived from inertial sensor samples.
 */
class RelativeAngleCalculator {
public:
    /**
     * @brief Estimates the relative pitch and roll between two inertial samples.
     */
    static RelativeAngle calculate(const InertialSample& first, const InertialSample& second);
};

#endif // UFLEX_CORE_MATH_RELATIVE_ANGLE_CALCULATOR_H
