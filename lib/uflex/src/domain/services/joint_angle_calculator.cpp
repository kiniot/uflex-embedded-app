//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

/**
 * @file joint_angle_calculator.cpp
 * @brief Implements the zero-offset joint flexion angle reduction.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */

#include "uflex/domain/services/joint_angle_calculator.h"

#include "uflex/domain/services/relative_rotation_calculator.h"

void JointAngleCalculator::calibrate(const Quaternion& relativeRotation) {
    zeroReference = normalize(relativeRotation);
    calibrated = true;
}

float JointAngleCalculator::absoluteFlexionDegrees(const Quaternion& relativeRotation) const {
    // Rotation that takes the captured zero pose to the current pose, expressed in
    // the zero's frame; its magnitude is the flexion relative to the reference.
    const Quaternion fromZero = RelativeRotationCalculator::calculate(zeroReference, relativeRotation);
    return rotationAngleDegrees(normalize(fromZero));
}

bool JointAngleCalculator::isCalibrated() const {
    return calibrated;
}

void JointAngleCalculator::reset() {
    zeroReference = Quaternion::identity();
    calibrated = false;
}
