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

namespace {

// Whether the active pair is upper-middle (brazo-antebrazo) vs middle-lower (antebrazo-mano).
// Pronation/supination is forearm axial rotation, so the hand rotates with the forearm and the
// movement is invisible on middle-lower -> measure the forearm against the still upper arm
// (upper-middle), regardless of joint. Otherwise the joint decides (Wrist -> middle-lower).
bool usesUpperMiddlePair(ActiveJoint joint, ActiveMovement movement) {
    if (movement == ActiveMovement::Pronation || movement == ActiveMovement::Supination) {
        return true;
    }
    return joint != ActiveJoint::Wrist;
}

} // namespace

Quaternion activeJointRotation(const MotionState& state, ActiveJoint joint, ActiveMovement movement) {
    return usesUpperMiddlePair(joint, movement) ? state.upperMiddleRotation
                                                : state.middleLowerRotation;
}

RelativeAngle activeJointAngle(const MotionState& state, ActiveJoint joint, ActiveMovement movement) {
    return usesUpperMiddlePair(joint, movement) ? state.upperMiddleAngle : state.middleLowerAngle;
}

bool exceedsSafeAngle(float targetAngleDegrees, const ActiveSerieContext& context) {
    return context.hasContext && context.hasMaxSafeAngle &&
           targetAngleDegrees >= context.maxSafeAngle;
}
