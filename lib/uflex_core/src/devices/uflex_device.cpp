//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

/**
 * @file uflex_device.cpp
 * @brief Implements the UflexDevice class.
 *
 * @author Salim Ramirez Mestanza
 * @date June 8, 2026
 * @version 0.1.0
 */

#include "uflex_core/devices/uflex_device.h"

UflexDevice::UflexDevice(SensorConfig upperSensorConfig, SensorConfig middleSensorConfig,
                         SensorConfig lowerSensorConfig)
    : upperSensor(upperSensorConfig.sensorId, upperSensorConfig.i2cAddress, this),
      middleSensor(middleSensorConfig.sensorId, middleSensorConfig.i2cAddress, this),
      lowerSensor(lowerSensorConfig.sensorId, lowerSensorConfig.i2cAddress, this) {}

void UflexDevice::on(Event event) {
    (void)event;
}

void UflexDevice::handle(Command command) {
    (void)command;
}

InertialSensor& UflexDevice::getUpperSensor() {
    return upperSensor;
}

InertialSensor& UflexDevice::getMiddleSensor() {
    return middleSensor;
}

InertialSensor& UflexDevice::getLowerSensor() {
    return lowerSensor;
}
