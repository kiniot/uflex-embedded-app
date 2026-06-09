//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/infrastructure/adapters/sim/simulated_imu_array.h"

#include <Arduino.h>

/**
 * @file simulated_imu_array.cpp
 * @brief Implements the simulation adapter for three IMUs.
 *
 * This adapter reads the MPU6050-compatible sensors exposed by the simulator
 * and forwards those measurements to the domain IMUs, allowing the rest of the
 * application to behave as if it were consuming real hardware input.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

SimulatedImuArray::SimulatedImuArray(ImuBinding firstImu,
    ImuBinding secondImu,
    ImuBinding thirdImu)
: imus{firstImu, secondImu, thirdImu} {}

bool SimulatedImuArray::begin() {
    bool allInitialized = true;

    for (ImuBinding& binding : imus) {
        if (!initializeImu(binding)) {
            allInitialized = false;
        }
    }

    return allInitialized;
}

bool SimulatedImuArray::update() {
    bool allUpdated = true;

    for (ImuBinding& binding : imus) {
        if (!updateImu(binding)) {
            allUpdated = false;
        }
    }

    return allUpdated;
}

bool SimulatedImuArray::initializeImu(ImuBinding& binding) {
    uint8_t whoAmI = 0;
    if (!detectImu(binding, whoAmI)) {
        Serial.printf("Imu [0x%02X] not detected\n", binding.imu.getI2cAddress());
        return false;
    }

    Serial.printf("Imu [0x%02X] WHO_AM_I=0x%02X\n", binding.imu.getI2cAddress(), whoAmI);

    if (!wakeImu(binding)) {
        Serial.printf("Imu [0x%02X] wake-up failed\n", binding.imu.getI2cAddress());
        return false;
    }

    Serial.printf("Imu [0x%02X] wake-up command sent\n", binding.imu.getI2cAddress());
    return true;
}

bool SimulatedImuArray::detectImu(ImuBinding& binding, uint8_t& whoAmI) {
    return readRegisters(binding.bus, binding.imu.getI2cAddress(), WHO_AM_I_REGISTER, &whoAmI,
                         1);
}

bool SimulatedImuArray::wakeImu(ImuBinding& binding) {
    return writeRegister(binding.bus, binding.imu.getI2cAddress(), POWER_MANAGEMENT_1_REGISTER,
                         0x00);
}

bool SimulatedImuArray::updateImu(ImuBinding& binding) {
    uint8_t rawData[14] = {};
    if (!readRegisters(binding.bus, binding.imu.getI2cAddress(), MEASUREMENT_START_REGISTER,
                       rawData, sizeof(rawData))) {
        Serial.printf("Imu [0x%02X] read failed\n", binding.imu.getI2cAddress());
        return false;
    }

    const ImuSample sample{
        readBigEndianInt16(rawData, 0),
        readBigEndianInt16(rawData, 2),
        readBigEndianInt16(rawData, 4),
        readBigEndianInt16(rawData, 8),
        readBigEndianInt16(rawData, 10),
        readBigEndianInt16(rawData, 12),
        readBigEndianInt16(rawData, 6),
    };

    binding.imu.updateSample(sample);
    return true;
}

bool SimulatedImuArray::readRegisters(TwoWire& bus, uint8_t address,
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

bool SimulatedImuArray::writeRegister(TwoWire& bus, uint8_t address, uint8_t reg,
                                                 uint8_t value) {
    bus.beginTransmission(address);
    bus.write(reg);
    bus.write(value);
    return bus.endTransmission() == 0;
}

int16_t SimulatedImuArray::readBigEndianInt16(const uint8_t* buffer, size_t offset) {
    return static_cast<int16_t>((static_cast<uint16_t>(buffer[offset]) << 8) |
                                static_cast<uint16_t>(buffer[offset + 1]));
}
