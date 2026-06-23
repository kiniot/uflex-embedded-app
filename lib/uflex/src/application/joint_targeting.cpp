//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

/**
 * @file joint_targeting.cpp
 * @brief Implements the pure active-joint targeting and safety helpers.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */

#include "uflex/application/joint_targeting.h"

Quaternion activeJointRotation(const MotionState& state, ActiveJoint joint) {
    switch (joint) {
        case ActiveJoint::Wrist:
            return state.middleLowerRotation;
        case ActiveJoint::Elbow:
        case ActiveJoint::None:
        default:
            return state.upperMiddleRotation;
    }
}

bool exceedsSafeAngle(float targetAngleDegrees, const ActiveSerieContext& context) {
    return context.hasContext && context.hasMaxSafeAngle &&
           targetAngleDegrees >= context.maxSafeAngle;
}
