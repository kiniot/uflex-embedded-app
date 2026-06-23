//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_APPLICATION_JOINT_TARGETING_H
#define UFLEX_APPLICATION_JOINT_TARGETING_H

#include "uflex/domain/devices/motion_state.h"
#include "uflex/domain/math/quaternion.h"
#include "uflex/infrastructure/transport/active_serie_context.h"

/**
 * @file joint_targeting.h
 * @brief Pure helpers mapping the active joint to its kinematics and safety.
 *
 * Kept as free functions over plain structs so they are host-unit-testable,
 * independent of the application loop, Arduino, or the transport.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */

/**
 * @brief Returns the active joint's relative rotation: Elbow -> upper-middle,
 * Wrist -> middle-lower. None defaults to upper-middle.
 */
Quaternion activeJointRotation(const MotionState& state, ActiveJoint joint);

/**
 * @brief Whether the flexion angle has crossed the active serie's safety ceiling.
 */
bool exceedsSafeAngle(float targetAngleDegrees, const ActiveSerieContext& context);

#endif // UFLEX_APPLICATION_JOINT_TARGETING_H
