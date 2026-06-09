//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

/**
 * @file imu.cpp
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

#include "uflex/domain/sensors/imu.h"

const Event Imu::MOTION_DETECTED_EVENT(MOTION_DETECTED_EVENT_ID);

Imu::Imu(int imuId, uint8_t i2cAddress, EventHandler* eventHandler)
    : Sensor(imuId, eventHandler),
      i2cAddress(i2cAddress),
      lastSample{0, 0, 0, 0, 0, 0, 0} {}

uint8_t Imu::getI2cAddress() const {
    return i2cAddress;
}

const ImuSample& Imu::getLastSample() const {
    return lastSample;
}

void Imu::updateSample(const ImuSample& sample) {
    lastSample = sample;
    on(MOTION_DETECTED_EVENT);
}
