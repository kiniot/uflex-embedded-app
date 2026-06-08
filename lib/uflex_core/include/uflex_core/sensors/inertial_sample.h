//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_CORE_SENSORS_INERTIAL_SAMPLE_H
#define UFLEX_CORE_SENSORS_INERTIAL_SAMPLE_H

#include <stdint.h>

/**
 * @file inertial_sample.h
 * @brief Declares the InertialSample data structure.
 *
 * This structure represents a single inertial measurement captured from a simulated or physical
 * motion sensor. It is shared across the uFlex core domain, simulation adapters, and hardware
 * adapters.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
struct InertialSample {
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
    int16_t temperature;
};

#endif // UFLEX_CORE_SENSORS_INERTIAL_SAMPLE_H
