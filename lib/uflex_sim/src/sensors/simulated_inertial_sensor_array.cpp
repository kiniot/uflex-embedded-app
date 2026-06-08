//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex_sim/sensors/simulated_inertial_sensor_array.h"

#include <Arduino.h>

/**
 * @file simulated_inertial_sensor_array.cpp
 * @brief Implements the SimulatedInertialSensorArray class.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

SimulatedInertialSensorArray::SimulatedInertialSensorArray(SensorBinding firstSensor,
                                                           SensorBinding secondSensor,
                                                           SensorBinding thirdSensor)
    : sensors{firstSensor, secondSensor, thirdSensor} {}

bool SimulatedInertialSensorArray::begin() {
    bool allInitialized = true;

    for (SensorBinding& binding : sensors) {
        if (!initializeSensor(binding)) {
            allInitialized = false;
        }
    }

    return allInitialized;
}

bool SimulatedInertialSensorArray::update() {
    bool allUpdated = true;

    for (SensorBinding& binding : sensors) {
        if (!updateSensor(binding)) {
            allUpdated = false;
        }
    }

    return allUpdated;
}

bool SimulatedInertialSensorArray::initializeSensor(SensorBinding& binding) {
    uint8_t whoAmI = 0;
    if (!detectSensor(binding, whoAmI)) {
        Serial.printf("Sensor [0x%02X] not detected\n", binding.sensor.getI2cAddress());
        return false;
    }

    Serial.printf("Sensor [0x%02X] WHO_AM_I=0x%02X\n", binding.sensor.getI2cAddress(), whoAmI);

    if (!wakeSensor(binding)) {
        Serial.printf("Sensor [0x%02X] wake-up failed\n", binding.sensor.getI2cAddress());
        return false;
    }

    Serial.printf("Sensor [0x%02X] wake-up command sent\n", binding.sensor.getI2cAddress());
    return true;
}

bool SimulatedInertialSensorArray::detectSensor(SensorBinding& binding, uint8_t& whoAmI) {
    return readRegisters(binding.bus, binding.sensor.getI2cAddress(), WHO_AM_I_REGISTER, &whoAmI,
                         1);
}

bool SimulatedInertialSensorArray::wakeSensor(SensorBinding& binding) {
    return writeRegister(binding.bus, binding.sensor.getI2cAddress(), POWER_MANAGEMENT_1_REGISTER,
                         0x00);
}

bool SimulatedInertialSensorArray::updateSensor(SensorBinding& binding) {
    uint8_t rawData[14] = {};
    if (!readRegisters(binding.bus, binding.sensor.getI2cAddress(), MEASUREMENT_START_REGISTER,
                       rawData, sizeof(rawData))) {
        Serial.printf("Sensor [0x%02X] read failed\n", binding.sensor.getI2cAddress());
        return false;
    }

    const InertialSample sample{
        readBigEndianInt16(rawData, 0),
        readBigEndianInt16(rawData, 2),
        readBigEndianInt16(rawData, 4),
        readBigEndianInt16(rawData, 8),
        readBigEndianInt16(rawData, 10),
        readBigEndianInt16(rawData, 12),
        readBigEndianInt16(rawData, 6),
    };

    binding.sensor.updateSample(sample);
    return true;
}

bool SimulatedInertialSensorArray::readRegisters(TwoWire& bus, uint8_t address,
                                                 uint8_t startRegister, uint8_t* buffer,
                                                 size_t length) {
    bus.beginTransmission(address);
    bus.write(startRegister);
    if (bus.endTransmission(false) != 0) {
        return false;
    }

    const size_t bytesRequested = bus.requestFrom(address, static_cast<uint8_t>(length));
    if (bytesRequested != length) {
        return false;
    }

    for (size_t index = 0; index < length; ++index) {
        buffer[index] = bus.read();
    }

    return true;
}

bool SimulatedInertialSensorArray::writeRegister(TwoWire& bus, uint8_t address, uint8_t reg,
                                                 uint8_t value) {
    bus.beginTransmission(address);
    bus.write(reg);
    bus.write(value);
    return bus.endTransmission() == 0;
}

int16_t SimulatedInertialSensorArray::readBigEndianInt16(const uint8_t* buffer, size_t offset) {
    return static_cast<int16_t>((static_cast<uint16_t>(buffer[offset]) << 8) |
                                static_cast<uint16_t>(buffer[offset + 1]));
}
