//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/infrastructure/adapters/hw/mpu9250_imu_array.h"

#include <Arduino.h>

/**
 * @file mpu9250_imu_array.cpp
 * @brief Implements the preliminary MPU9250 hardware adapter for uFlex.
 *
 * Preliminary MPU9250 hardware adapter for uFlex. It reads the shared accelerometer, gyroscope,
 * and temperature register block, and additionally enables I2C bypass mode on each MPU9250 so the
 * AK8963 magnetometer integrated in the same package becomes directly addressable on the bus,
 * populating the full domain ImuSample contract.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

Mpu9250ImuArray::Mpu9250ImuArray(ImuBinding firstImu, ImuBinding secondImu, ImuBinding thirdImu)
    : imus{firstImu, secondImu, thirdImu} {}

bool Mpu9250ImuArray::begin() {
    bool allInitialized = true;

    for (ImuBinding& binding : imus) {
        if (!initializeImu(binding)) {
            allInitialized = false;
        }
    }

    return allInitialized;
}

bool Mpu9250ImuArray::update() {
    bool allUpdated = true;

    for (ImuBinding& binding : imus) {
        if (!updateImu(binding)) {
            allUpdated = false;
        }
    }

    return allUpdated;
}

bool Mpu9250ImuArray::initializeImu(ImuBinding& binding) {
    uint8_t whoAmI = 0;
    if (!detectImu(binding, whoAmI)) {
        Serial.printf("MPU9250 [0x%02X] not detected\n", binding.imu.getI2cAddress());
        return false;
    }

    Serial.printf("MPU9250 [0x%02X] WHO_AM_I=0x%02X\n", binding.imu.getI2cAddress(), whoAmI);

    if (whoAmI != EXPECTED_WHO_AM_I) {
        Serial.printf("MPU9250 [0x%02X] unexpected WHO_AM_I value\n",
                      binding.imu.getI2cAddress());
        return false;
    }

    if (!wakeImu(binding)) {
        Serial.printf("MPU9250 [0x%02X] wake-up failed\n", binding.imu.getI2cAddress());
        return false;
    }

    Serial.printf("MPU9250 [0x%02X] wake-up command sent\n", binding.imu.getI2cAddress());

    if (!enableMagnetometerBypass(binding)) {
        Serial.printf("MPU9250 [0x%02X] magnetometer bypass failed\n",
                      binding.imu.getI2cAddress());
        return false;
    }

    if (!initializeMagnetometer(binding)) {
        Serial.printf("MPU9250 [0x%02X] AK8963 magnetometer init failed\n",
                      binding.imu.getI2cAddress());
        return false;
    }

    Serial.printf("MPU9250 [0x%02X] AK8963 magnetometer ready\n", binding.imu.getI2cAddress());
    return true;
}

bool Mpu9250ImuArray::enableMagnetometerBypass(ImuBinding& binding) {
    return writeRegister(binding.bus, binding.imu.getI2cAddress(), INT_PIN_CFG_REGISTER,
                         BYPASS_EN_VALUE);
}

bool Mpu9250ImuArray::initializeMagnetometer(ImuBinding& binding) {
    uint8_t whoAmI = 0;
    if (!readRegisters(binding.bus, AK8963_I2C_ADDRESS, AK8963_WHO_AM_I_REGISTER, &whoAmI, 1)) {
        return false;
    }

    if (whoAmI != AK8963_EXPECTED_WHO_AM_I) {
        Serial.printf("AK8963 unexpected WHO_AM_I value: 0x%02X\n", whoAmI);
        return false;
    }

    return writeRegister(binding.bus, AK8963_I2C_ADDRESS, AK8963_CNTL1_REGISTER,
                         AK8963_CONTINUOUS_MEASUREMENT_16BIT_MODE2);
}

bool Mpu9250ImuArray::detectImu(ImuBinding& binding, uint8_t& whoAmI) {
    return readRegisters(binding.bus, binding.imu.getI2cAddress(), WHO_AM_I_REGISTER, &whoAmI, 1);
}

bool Mpu9250ImuArray::wakeImu(ImuBinding& binding) {
    return writeRegister(binding.bus, binding.imu.getI2cAddress(), POWER_MANAGEMENT_1_REGISTER,
                         0x00);
}

bool Mpu9250ImuArray::updateImu(ImuBinding& binding) {
    uint8_t rawData[14] = {};
    if (!readRegisters(binding.bus, binding.imu.getI2cAddress(), MEASUREMENT_START_REGISTER,
                       rawData, sizeof(rawData))) {
        Serial.printf("MPU9250 [0x%02X] read failed\n", binding.imu.getI2cAddress());
        return false;
    }

    int16_t magX = 0;
    int16_t magY = 0;
    int16_t magZ = 0;
    if (!readMagnetometer(binding, magX, magY, magZ)) {
        Serial.printf("MPU9250 [0x%02X] magnetometer read skipped (not ready)\n",
                      binding.imu.getI2cAddress());
    }

    const ImuSample sample{
        readBigEndianInt16(rawData, 0),
        readBigEndianInt16(rawData, 2),
        readBigEndianInt16(rawData, 4),
        readBigEndianInt16(rawData, 8),
        readBigEndianInt16(rawData, 10),
        readBigEndianInt16(rawData, 12),
        readBigEndianInt16(rawData, 6),
        magX,
        magY,
        magZ,
    };

    binding.imu.updateSample(sample);
    return true;
}

bool Mpu9250ImuArray::readMagnetometer(ImuBinding& binding, int16_t& magX, int16_t& magY,
                                       int16_t& magZ) {
    uint8_t status = 0;
    if (!readRegisters(binding.bus, AK8963_I2C_ADDRESS, AK8963_ST1_REGISTER, &status, 1)) {
        return false;
    }

    if ((status & AK8963_DATA_READY_BIT) == 0) {
        return false;
    }

    uint8_t rawData[7] = {};
    if (!readRegisters(binding.bus, AK8963_I2C_ADDRESS, AK8963_MEASUREMENT_START_REGISTER,
                       rawData, sizeof(rawData))) {
        return false;
    }

    // The AK8963 die is mounted rotated relative to the MPU9250's own accelerometer/gyroscope
    // die, so its raw axes do not match theirs. The datasheet-documented remap is: magnetometer
    // X/Y are swapped, and Z is inverted, relative to the accelerometer/gyroscope frame. Applying
    // it here keeps every ImuSample field expressed in the same axis frame for the domain layer.
    magX = readLittleEndianInt16(rawData, 2);
    magY = readLittleEndianInt16(rawData, 0);
    magZ = static_cast<int16_t>(-readLittleEndianInt16(rawData, 4));
    return true;
}

bool Mpu9250ImuArray::readRegisters(TwoWire& bus, uint8_t address, uint8_t startRegister,
                                    uint8_t* buffer, size_t length) {
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

bool Mpu9250ImuArray::writeRegister(TwoWire& bus, uint8_t address, uint8_t reg, uint8_t value) {
    bus.beginTransmission(address);
    bus.write(reg);
    bus.write(value);
    return bus.endTransmission() == 0;
}

int16_t Mpu9250ImuArray::readBigEndianInt16(const uint8_t* buffer, size_t offset) {
    return static_cast<int16_t>((static_cast<uint16_t>(buffer[offset]) << 8) |
                                static_cast<uint16_t>(buffer[offset + 1]));
}

int16_t Mpu9250ImuArray::readLittleEndianInt16(const uint8_t* buffer, size_t offset) {
    return static_cast<int16_t>((static_cast<uint16_t>(buffer[offset + 1]) << 8) |
                                static_cast<uint16_t>(buffer[offset]));
}
