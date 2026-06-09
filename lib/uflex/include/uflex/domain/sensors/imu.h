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

    Imu(int imuId, uint8_t i2cAddress, EventHandler* eventHandler = nullptr);

    uint8_t getI2cAddress() const;

    const ImuSample& getLastSample() const;

    void updateSample(const ImuSample& sample);
};

#endif // UFLEX_DOMAIN_SENSORS_IMU_H
