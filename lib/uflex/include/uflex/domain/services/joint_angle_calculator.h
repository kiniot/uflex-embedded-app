//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_DOMAIN_SERVICES_JOINT_ANGLE_CALCULATOR_H
#define UFLEX_DOMAIN_SERVICES_JOINT_ANGLE_CALCULATOR_H

#include "uflex/domain/services/relative_angle_calculator.h"

/**
 * @file joint_angle_calculator.h
 * @brief Reduces a joint's gravity-anchored relative tilt to one flexion angle,
 * with a per-session zero offset.
 *
 * The active joint angle is the accelerometer-derived relative (pitch, roll)
 * between its two segment IMUs (see RelativeAngleCalculator) — anchored to
 * gravity, so it does not drift with yaw/twist the way the full quaternion
 * rotation magnitude did. This service captures a reference ("zero") relative
 * tilt at session start (the patient in the extension pose) and reports every
 * later angle as the distance from it, so 0 degrees means the calibrated
 * reference pose and the value is a meaningful flexion for both ROM and the
 * absolute safety ceiling.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */
class JointAngleCalculator {
public:
    /**
     * @brief Captures the current relative tilt as the zero reference.
     */
    void calibrate(const RelativeAngle& jointAngle);

    /**
     * @brief Returns the flexion angle (degrees) of `jointAngle` measured from
     * the calibrated zero as the distance in (pitch, roll) space. Before any
     * calibration the zero is {0, 0}, so it degrades gracefully to the raw tilt
     * magnitude.
     */
    float absoluteFlexionDegrees(const RelativeAngle& jointAngle) const;

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
    RelativeAngle zeroReference{0.0f, 0.0f};
    bool calibrated = false;
};

#endif // UFLEX_DOMAIN_SERVICES_JOINT_ANGLE_CALCULATOR_H
