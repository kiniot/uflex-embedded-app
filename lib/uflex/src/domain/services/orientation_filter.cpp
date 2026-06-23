//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

/**
 * @file orientation_filter.cpp
 * @brief Implements the Mahony-style AHRS sensor fusion service for uFlex.
 *
 * The filter integrates gyroscope-derived angular velocity into the running
 * orientation quaternion, then nudges that estimate toward the orientation
 * implied by the measured gravity direction (and, when available, Earth's
 * magnetic field) using a proportional cross-product error term. This is the
 * standard "explicit complementary filter" structure popularized by Mahony,
 * simplified here by omitting bias/integral estimation.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */

#include "uflex/domain/services/orientation_filter.h"

#include <math.h>

namespace {

// MPU9250 ships with a default gyroscope full-scale range of +/-250 deg/s, which this
// driver does not reconfigure, giving a fixed sensitivity of 131 LSB per deg/s.
constexpr float kGyroSensitivityLsbPerDegPerSec = 131.0f;
constexpr float kDegreesToRadians = 0.017453293f;
constexpr float kMinVectorMagnitude = 1e-6f;

struct Vector3 {
    float x;
    float y;
    float z;
};

float toRadiansPerSecond(int16_t rawGyro) {
    return (static_cast<float>(rawGyro) / kGyroSensitivityLsbPerDegPerSec) * kDegreesToRadians;
}

/**
 * @brief Normalizes `vector` in place. Returns false (leaving it untouched) when
 * its magnitude is too small to represent a meaningful direction, which signals
 * "this sensor reading is unavailable" to the caller.
 */
bool normalizeInPlace(Vector3& vector) {
    const float magnitude =
        sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    if (magnitude < kMinVectorMagnitude) {
        return false;
    }

    const float inverseMagnitude = 1.0f / magnitude;
    vector.x *= inverseMagnitude;
    vector.y *= inverseMagnitude;
    vector.z *= inverseMagnitude;
    return true;
}

} // namespace

OrientationFilter::OrientationFilter(float beta) : beta(beta), orientation(Quaternion::identity()) {}

const Quaternion& OrientationFilter::getOrientation() const {
    return orientation;
}

void OrientationFilter::reset() {
    orientation = Quaternion::identity();
}

Quaternion OrientationFilter::update(const ImuSample& sample, float deltaTimeSeconds) {
    const float q0 = orientation.w;
    const float q1 = orientation.x;
    const float q2 = orientation.y;
    const float q3 = orientation.z;

    float gyroX = toRadiansPerSecond(sample.gyroX);
    float gyroY = toRadiansPerSecond(sample.gyroY);
    float gyroZ = toRadiansPerSecond(sample.gyroZ);

    Vector3 accel{static_cast<float>(sample.accelX), static_cast<float>(sample.accelY),
                  static_cast<float>(sample.accelZ)};

    if (normalizeInPlace(accel)) {
        Vector3 mag{static_cast<float>(sample.magX), static_cast<float>(sample.magY),
                   static_cast<float>(sample.magZ)};
        const bool hasMagnetometer = normalizeInPlace(mag);

        // Estimated gravity direction in the sensor frame, derived from the current
        // orientation estimate (the body-frame representation of the inertial-frame "up").
        const float halfGravityX = q1 * q3 - q0 * q2;
        const float halfGravityY = q0 * q1 + q2 * q3;
        const float halfGravityZ = q0 * q0 - 0.5f + q3 * q3;

        float halfErrorX;
        float halfErrorY;
        float halfErrorZ;

        if (hasMagnetometer) {
            // Reference direction of Earth's magnetic field, expressed in the sensor frame.
            const float hx = 2.0f * (mag.x * (0.5f - q2 * q2 - q3 * q3) +
                                     mag.y * (q1 * q2 - q0 * q3) + mag.z * (q1 * q3 + q0 * q2));
            const float hy = 2.0f * (mag.x * (q1 * q2 + q0 * q3) +
                                     mag.y * (0.5f - q1 * q1 - q3 * q3) +
                                     mag.z * (q2 * q3 - q0 * q1));
            const float bx = sqrtf(hx * hx + hy * hy);
            const float bz = 2.0f * (mag.x * (q1 * q3 - q0 * q2) +
                                     mag.y * (q2 * q3 + q0 * q1) +
                                     mag.z * (0.5f - q1 * q1 - q2 * q2));

            const float halfFieldX = bx * (0.5f - q2 * q2 - q3 * q3) + bz * (q1 * q3 - q0 * q2);
            const float halfFieldY = bx * (q1 * q2 - q0 * q3) + bz * (q0 * q1 + q2 * q3);
            const float halfFieldZ = bx * (q0 * q2 + q1 * q3) + bz * (0.5f - q1 * q1 - q2 * q2);

            // Error is the sum of the cross products between each measured direction and
            // its corresponding estimated direction: zero when the estimate is consistent
            // with both the accelerometer and the magnetometer.
            halfErrorX = (accel.y * halfGravityZ - accel.z * halfGravityY) +
                        (mag.y * halfFieldZ - mag.z * halfFieldY);
            halfErrorY = (accel.z * halfGravityX - accel.x * halfGravityZ) +
                        (mag.z * halfFieldX - mag.x * halfFieldZ);
            halfErrorZ = (accel.x * halfGravityY - accel.y * halfGravityX) +
                        (mag.x * halfFieldY - mag.y * halfFieldX);
        } else {
            // 6-DOF fallback: only gravity anchors the estimate, so pitch/roll stay
            // accurate but yaw is free to drift (no absolute heading reference).
            halfErrorX = accel.y * halfGravityZ - accel.z * halfGravityY;
            halfErrorY = accel.z * halfGravityX - accel.x * halfGravityZ;
            halfErrorZ = accel.x * halfGravityY - accel.y * halfGravityX;
        }

        // Proportional feedback: nudge the gyroscope rate toward the accelerometer/
        // magnetometer estimate instead of integrating gyroscope drift unchecked.
        gyroX += beta * halfErrorX;
        gyroY += beta * halfErrorY;
        gyroZ += beta * halfErrorZ;
    }

    const float qDotW = 0.5f * (-q1 * gyroX - q2 * gyroY - q3 * gyroZ);
    const float qDotX = 0.5f * (q0 * gyroX + q2 * gyroZ - q3 * gyroY);
    const float qDotY = 0.5f * (q0 * gyroY - q1 * gyroZ + q3 * gyroX);
    const float qDotZ = 0.5f * (q0 * gyroZ + q1 * gyroY - q2 * gyroX);

    orientation = normalize(Quaternion{
        q0 + qDotW * deltaTimeSeconds,
        q1 + qDotX * deltaTimeSeconds,
        q2 + qDotY * deltaTimeSeconds,
        q3 + qDotZ * deltaTimeSeconds,
    });

    return orientation;
}
