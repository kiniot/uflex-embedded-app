//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_DOMAIN_DEVICES_MOTION_STATE_H
#define UFLEX_DOMAIN_DEVICES_MOTION_STATE_H

#include "uflex/domain/sensors/imu_sample.h"
#include "uflex/domain/services/relative_angle_calculator.h"

/**
 * @file motion_state.h
 * @brief Declares the aggregated motion snapshot for the uFlex domain.
 *
 * MotionState represents the processed state currently exposed by the central
 * domain device. It keeps the latest IMU samples together with the relative
 * angles derived from them so application and transport layers can consume a
 * stable view of the current motion frame.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
struct MotionState {
    ImuSample upperSample;
    ImuSample middleSample;
    ImuSample lowerSample;
    RelativeAngle upperMiddleAngle;
    RelativeAngle middleLowerAngle;
    RelativeAngle upperLowerAngle;
};

#endif // UFLEX_DOMAIN_DEVICES_MOTION_STATE_H
