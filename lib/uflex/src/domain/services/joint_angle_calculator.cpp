//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

/**
 * @file joint_angle_calculator.cpp
 * @brief Implements the zero-offset, gravity-anchored joint flexion angle.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */

#include "uflex/domain/services/joint_angle_calculator.h"

#include <math.h>

void JointAngleCalculator::calibrate(const RelativeAngle& jointAngle) {
    zeroReference = jointAngle;
    calibrated = true;
}

float JointAngleCalculator::absoluteFlexionDegrees(const RelativeAngle& jointAngle) const {
    // Distance from the calibrated zero in the accelerometer-derived (pitch, roll) plane. Both
    // components are gravity-anchored, so this flexion is immune to yaw/twist drift (unlike the old
    // full quaternion magnitude), and it captures the tilt regardless of which axis the hinge is on.
    const float deltaPitch = jointAngle.pitchDegrees - zeroReference.pitchDegrees;
    const float deltaRoll = jointAngle.rollDegrees - zeroReference.rollDegrees;
    const float flexion = sqrtf(deltaPitch * deltaPitch + deltaRoll * deltaRoll);
    // Cap at 180: a joint cannot flex past it, and the L2 distance can overshoot on large combined
    // pitch+roll (non-hinge motion). Keeps peakAngle/achievedRom within the backend's [0, 180] range.
    return fminf(flexion, 180.0f);
}

bool JointAngleCalculator::isCalibrated() const {
    return calibrated;
}

void JointAngleCalculator::reset() {
    zeroReference = RelativeAngle{0.0f, 0.0f};
    calibrated = false;
}
