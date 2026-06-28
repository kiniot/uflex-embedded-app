//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_DOMAIN_SERVICES_JOINT_ANGLE_CALCULATOR_H
#define UFLEX_DOMAIN_SERVICES_JOINT_ANGLE_CALCULATOR_H

#include "uflex/domain/math/quaternion.h"

/**
 * @file joint_angle_calculator.h
 * @brief Reduces a relative joint rotation to one absolute flexion angle, with a
 * per-session zero offset.
 *
 * The active joint's rotation is the relative quaternion between its two segment
 * IMUs. This service captures a reference ("zero") relative rotation at session
 * start (the patient in the extension pose) and reports every later angle
 * relative to it, so 0 degrees means the calibrated reference pose and the value
 * is anatomically meaningful for both ROM and the absolute safety ceiling.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */
class JointAngleCalculator {
public:
    /**
     * @brief Captures the current relative rotation as the zero reference.
     */
    void calibrate(const Quaternion& relativeRotation);

    /**
     * @brief Returns the flexion angle (degrees) of `relativeRotation` measured
     * from the calibrated zero. Before any calibration the zero is the identity,
     * so it degrades gracefully to the raw rotation magnitude.
     */
    float absoluteFlexionDegrees(const Quaternion& relativeRotation) const;

    /**
     * @brief Whether a zero reference has been captured.
     */
    bool isCalibrated() const;

    /**
     * @brief Drops the captured zero, returning to the uncalibrated state.
     *
     * Called when the active serie context is lost or expires, so safety stays
     * disarmed (see SafetyMonitor) until a fresh zero is captured for the next
     * serie rather than enforcing against a stale reference.
     */
    void reset();

private:
    Quaternion zeroReference = Quaternion::identity();
    bool calibrated = false;
};

#endif // UFLEX_DOMAIN_SERVICES_JOINT_ANGLE_CALCULATOR_H
