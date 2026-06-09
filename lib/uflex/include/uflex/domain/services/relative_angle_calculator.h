//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_DOMAIN_SERVICES_RELATIVE_ANGLE_CALCULATOR_H
#define UFLEX_DOMAIN_SERVICES_RELATIVE_ANGLE_CALCULATOR_H

#include "uflex/domain/sensors/imu_sample.h"

/**
 * @file relative_angle_calculator.h
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
struct RelativeAngle {
    float pitchDegrees;
    float rollDegrees;
};

class RelativeAngleCalculator {
public:
    static RelativeAngle calculate(const ImuSample& first, const ImuSample& second);
};

#endif // UFLEX_DOMAIN_SERVICES_RELATIVE_ANGLE_CALCULATOR_H
