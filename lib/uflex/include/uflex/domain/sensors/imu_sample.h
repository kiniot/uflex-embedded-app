//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_DOMAIN_SENSORS_IMU_SAMPLE_H
#define UFLEX_DOMAIN_SENSORS_IMU_SAMPLE_H

#include <stdint.h>

/**
 * @file imu_sample.h
 * @brief Declares the inertial sample structure shared across uFlex.
 *
 * ImuSample captures the accelerometer, gyroscope, and temperature values used
 * by the current domain model. The same structure is produced by simulation and
 * hardware adapters so motion calculations can stay backend-agnostic.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
struct ImuSample {
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
    int16_t temperature;
};

#endif // UFLEX_DOMAIN_SENSORS_IMU_SAMPLE_H
