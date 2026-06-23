//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/infrastructure/adapters/hw/mpu9250_imu_array.h"

#include <Arduino.h>

#include "uflex/domain/services/gyro_bias_calibrator.h"

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
    : imus{firstImu, secondImu, thirdImu} {
    // Identity defaults so an un-calibrated IMU behaves exactly as before: unit ASA scale
    // (raw mag passes through) and zero gyro bias (raw gyro passes through).
    for (ImuBinding& binding : imus) {
        binding.magAsa[0] = 1.0f;
        binding.magAsa[1] = 1.0f;
        binding.magAsa[2] = 1.0f;
        binding.gyroBias[0] = 0;
        binding.gyroBias[1] = 0;
        binding.gyroBias[2] = 0;
    }
}

bool Mpu9250ImuArray::begin() {
    bool allInitialized = true;

    for (ImuBinding& binding : imus) {
        if (!initializeImu(binding)) {
            allInitialized = false;
            continue;
        }
        calibrateGyroBias(binding);
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

    // Read the factory sensitivity adjustment (ASA) from Fuse ROM. Per the datasheet, every
    // CNTL1 mode change must pass through power-down. ASA corrects per-axis gain so the
    // magnetometer *direction* the orientation filter relies on is not skewed.
    if (!writeRegister(binding.bus, AK8963_I2C_ADDRESS, AK8963_CNTL1_REGISTER, AK8963_POWER_DOWN_MODE)) {
        return false;
    }
    delayMicroseconds(100);
    if (!writeRegister(binding.bus, AK8963_I2C_ADDRESS, AK8963_CNTL1_REGISTER, AK8963_FUSE_ROM_ACCESS_MODE)) {
        return false;
    }
    delayMicroseconds(100);
    uint8_t asa[3] = {};
    if (!readRegisters(binding.bus, AK8963_I2C_ADDRESS, AK8963_ASA_START_REGISTER, asa, 3)) {
        return false;
    }
    for (int axis = 0; axis < 3; ++axis) {
        binding.magAsa[axis] = (static_cast<float>(asa[axis]) + 128.0f) / 256.0f;
    }
    if (!writeRegister(binding.bus, AK8963_I2C_ADDRESS, AK8963_CNTL1_REGISTER, AK8963_POWER_DOWN_MODE)) {
        return false;
    }
    delayMicroseconds(100);

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
        static_cast<int16_t>(readBigEndianInt16(rawData, 8) - binding.gyroBias[0]),
        static_cast<int16_t>(readBigEndianInt16(rawData, 10) - binding.gyroBias[1]),
        static_cast<int16_t>(readBigEndianInt16(rawData, 12) - binding.gyroBias[2]),
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

    // ST2 (the last byte) must be read to unlatch the data registers for the next sample; its
    // HOFL bit flags a magnetic overflow that invalidates this measurement -> skip it (the caller
    // keeps mag at 0, so the orientation filter falls back to 6-DOF for this tick).
    if ((rawData[AK8963_ST2_REGISTER_OFFSET] & AK8963_OVERFLOW_BIT) != 0) {
        return false;
    }

    // The AK8963 die is mounted rotated relative to the MPU9250's own accelerometer/gyroscope
    // die, so its raw axes do not match theirs. The datasheet-documented remap is: magnetometer
    // X/Y are swapped, and Z is inverted, relative to the accelerometer/gyroscope frame. The
    // per-axis factory sensitivity adjustment (ASA, read at init) is then applied so the axes
    // share a consistent gain; saturate back into int16 for the ImuSample contract.
    magX = saturateToInt16(readLittleEndianInt16(rawData, 2) * binding.magAsa[0]);
    magY = saturateToInt16(readLittleEndianInt16(rawData, 0) * binding.magAsa[1]);
    magZ = saturateToInt16(-readLittleEndianInt16(rawData, 4) * binding.magAsa[2]);
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

void Mpu9250ImuArray::calibrateGyroBias(ImuBinding& binding) {
    Serial.printf("MPU9250 [0x%02X] gyro calibration: keep the kit still...\n",
                  binding.imu.getI2cAddress());

    GyroBiasCalibrator calibrator(GYRO_BIAS_CALIBRATION_SAMPLES);
    for (uint16_t sampleIndex = 0; sampleIndex < GYRO_BIAS_CALIBRATION_SAMPLES; ++sampleIndex) {
        uint8_t rawData[14] = {};
        if (!readRegisters(binding.bus, binding.imu.getI2cAddress(), MEASUREMENT_START_REGISTER,
                           rawData, sizeof(rawData))) {
            Serial.printf("MPU9250 [0x%02X] gyro calibration read failed; using zero bias\n",
                          binding.imu.getI2cAddress());
            return;  // soft-fail: gyroBias stays at its identity {0,0,0}
        }
        calibrator.addSample(readBigEndianInt16(rawData, 8), readBigEndianInt16(rawData, 10),
                             readBigEndianInt16(rawData, 12));
        delay(3);
    }

    calibrator.getBias(binding.gyroBias[0], binding.gyroBias[1], binding.gyroBias[2]);
    Serial.printf("MPU9250 [0x%02X] gyro bias=(%d, %d, %d)\n", binding.imu.getI2cAddress(),
                  binding.gyroBias[0], binding.gyroBias[1], binding.gyroBias[2]);
}

int16_t Mpu9250ImuArray::saturateToInt16(float value) {
    if (value > 32767.0f) {
        return 32767;
    }
    if (value < -32768.0f) {
        return -32768;
    }
    return static_cast<int16_t>(value);
}
