//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_DOMAIN_SENSORS_IMU_H
#define UFLEX_DOMAIN_SENSORS_IMU_H

#include <Sensor.h>
#include <stdint.h>

#include "uflex/domain/sensors/imu_sample.h"

/**
 * @file imu.h
 * @brief Declares the domain IMU abstraction for uFlex.
 *
 * Imu adapts the ModestIoT Sensor base class to the motion domain. It stores
 * the latest inertial sample, exposes the configured I2C address, and emits a
 * motion-related event whenever a new sample is pushed into the domain model.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class Imu : public Sensor {
private:
    uint8_t i2cAddress;
    ImuSample lastSample;

public:
    static constexpr int MOTION_DETECTED_EVENT_ID = 0;
    static const Event MOTION_DETECTED_EVENT;

    /**
     * @brief Constructs a domain IMU with its logical identifier and address.
     */
    Imu(int imuId, uint8_t i2cAddress, EventHandler* eventHandler = nullptr);

    /**
     * @brief Returns the configured I2C address.
     */
    uint8_t getI2cAddress() const;

    /**
     * @brief Returns the latest stored IMU sample.
     */
    const ImuSample& getLastSample() const;

    /**
     * @brief Stores a new sample and emits the motion event.
     */
    void updateSample(const ImuSample& sample);
};

#endif // UFLEX_DOMAIN_SENSORS_IMU_H
