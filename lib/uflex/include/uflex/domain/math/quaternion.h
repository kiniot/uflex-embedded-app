//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_DOMAIN_MATH_QUATERNION_H
#define UFLEX_DOMAIN_MATH_QUATERNION_H

/**
 * @file quaternion.h
 * @brief Declares the unit quaternion type shared across uFlex orientation math.
 *
 * Quaternion represents a 3D rotation without the gimbal-lock and interpolation
 * issues of Euler angles. It is the common currency between sensor fusion
 * (OrientationFilter), relative joint rotation, and BLE telemetry, so all three
 * stay free of redundant conversions.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
struct Quaternion {
    float w;
    float x;
    float y;
    float z;

    static Quaternion identity();
};

/**
 * @brief Returns the Hamilton product `lhs * rhs` of two quaternions.
 */
Quaternion multiply(const Quaternion& lhs, const Quaternion& rhs);

/**
 * @brief Returns the conjugate of a unit quaternion, equal to its inverse.
 */
Quaternion conjugate(const Quaternion& quaternion);

/**
 * @brief Returns `quaternion` rescaled to unit length, or the identity quaternion
 * if its magnitude is too small to normalize safely.
 */
Quaternion normalize(const Quaternion& quaternion);

/**
 * @brief Returns the rotation magnitude of a quaternion in degrees, range
 * [0, 180]. Uses |w| so the double-cover (q and -q are the same rotation) maps
 * to a single angle; normalize the quaternion first for a non-unit input.
 */
float rotationAngleDegrees(const Quaternion& quaternion);

/**
 * @brief Returns the yaw (rotation about Z) of a quaternion in degrees, range
 * [-180, 180]. Used for the proximal-segment compensation signal.
 */
float yawDegrees(const Quaternion& quaternion);

#endif // UFLEX_DOMAIN_MATH_QUATERNION_H
