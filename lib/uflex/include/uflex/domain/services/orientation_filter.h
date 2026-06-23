//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_DOMAIN_SERVICES_ORIENTATION_FILTER_H
#define UFLEX_DOMAIN_SERVICES_ORIENTATION_FILTER_H

#include "uflex/domain/math/quaternion.h"
#include "uflex/domain/sensors/imu_sample.h"

/**
 * @file orientation_filter.h
 * @brief Declares the Mahony-style AHRS sensor fusion service for the uFlex domain.
 *
 * OrientationFilter turns the raw accelerometer/gyroscope/magnetometer triplet of
 * a single IMU into a stable absolute orientation quaternion. It implements a
 * proportional explicit complementary filter (Mahony-style, without bias/integral
 * estimation): the gyroscope-integrated quaternion rate is continuously corrected
 * toward the orientation implied by gravity (accelerometer) and, when available,
 * Earth's magnetic field (magnetometer). It keeps the running orientation as
 * internal state across calls, so one instance must be owned per physical IMU
 * (upper, middle, lower).
 *
 * Magnetometer data is optional: an all-zero magnetometer reading (as produced
 * by adapters that cannot source it, such as the MPU6050-backed simulator) makes
 * the filter fall back to a 6-DOF accel+gyro update. That update still tracks
 * pitch/roll correctly but lets yaw drift freely, since only the magnetometer
 * anchors yaw to an absolute heading.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
class OrientationFilter {
public:
    /**
     * @brief Constructs a filter starting at the identity orientation.
     *
     * @param beta Proportional correction gain trading off how fast the
     * accelerometer/magnetometer pull the gyroscope-integrated estimate back
     * into agreement (higher) against noise rejection and smoothness (lower).
     * The default favors stability for slow rehabilitation movements.
     */
    explicit OrientationFilter(float beta = kDefaultBeta);

    /**
     * @brief Fuses one new sample into the running orientation estimate.
     *
     * @param sample Latest raw IMU sample.
     * @param deltaTimeSeconds Elapsed time since the previous update, in seconds.
     * Must be positive; callers should skip calling update() for the very first
     * sample of a session if no prior timestamp is available.
     * @return The updated orientation quaternion (same value as getOrientation()).
     */
    Quaternion update(const ImuSample& sample, float deltaTimeSeconds);

    /**
     * @brief Returns the current orientation estimate without advancing it.
     */
    const Quaternion& getOrientation() const;

    /**
     * @brief Resets the filter back to the identity orientation.
     */
    void reset();

private:
    static constexpr float kDefaultBeta = 0.1f;

    float beta;
    Quaternion orientation;
};

#endif // UFLEX_DOMAIN_SERVICES_ORIENTATION_FILTER_H
