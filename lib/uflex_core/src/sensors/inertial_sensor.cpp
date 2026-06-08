//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

/**
 * @file inertial_sensor.cpp
 * @brief Implements the InertialSensor class.
 *
 * @author Salim Ramirez Mestanza
 * @date June 8, 2026
 * @version 0.1.0
 */

#include "uflex_core/sensors/inertial_sensor.h"

const Event InertialSensor::MOTION_DETECTED_EVENT(MOTION_DETECTED_EVENT_ID);

InertialSensor::InertialSensor(int sensorId, uint8_t i2cAddress, EventHandler* eventHandler)
    : Sensor(sensorId, eventHandler),
      i2cAddress(i2cAddress),
      lastSample{0, 0, 0, 0, 0, 0, 0} {}

uint8_t InertialSensor::getI2cAddress() const {
    return i2cAddress;
}

const InertialSample& InertialSensor::getLastSample() const {
    return lastSample;
}

void InertialSensor::updateSample(const InertialSample& sample) {
    lastSample = sample;
    on(MOTION_DETECTED_EVENT);
}
