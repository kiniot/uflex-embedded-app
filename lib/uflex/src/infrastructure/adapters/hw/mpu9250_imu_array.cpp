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

    // Device reset (PWR_MGMT_1 H_RESET) to a known state. The MPU9250 keeps its registers across an
    // ESP32-only reset (it is not power-cycled), so leftover I2C-master/bypass config from a prior
    // boot would make the bypass-based AK8963 init fail. Reset clears that.
    writeRegister(binding.bus, binding.imu.getI2cAddress(), POWER_MANAGEMENT_1_REGISTER, 0x80);
    delay(100);

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

    // Switch this AK8963 from bypass to the MPU9250's internal I2C master so it no longer shares
    // the fixed 0x0C address on the external bus. Safe to do per-IMU here: begin() initializes the
    // IMUs sequentially, so bypass is turned off on this one before the next same-bus IMU enables it.
    if (!enableMagnetometerMasterMode(binding)) {
        Serial.printf("MPU9250 [0x%02X] AK8963 master-mode setup failed\n",
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

bool Mpu9250ImuArray::enableMagnetometerMasterMode(ImuBinding& binding) {
    const uint8_t address = binding.imu.getI2cAddress();

    // Disable bypass FIRST: BYPASS_EN gates the internal master, and on a shared bus it is what
    // puts two AK8963 at 0x0C. After this, the AK8963 is reachable only through this MPU9250.
    if (!writeRegister(binding.bus, address, INT_PIN_CFG_REGISTER, BYPASS_DISABLE_VALUE)) {
        return false;
    }
    // Enable the MPU9250's internal I2C master and set the auxiliary-bus clock (400kHz).
    if (!writeRegister(binding.bus, address, USER_CTRL_REGISTER, I2C_MST_EN_VALUE)) {
        return false;
    }
    if (!writeRegister(binding.bus, address, I2C_MST_CTRL_REGISTER, I2C_MST_CTRL_VALUE)) {
        return false;
    }
    // SLV0: continuously read AK8963 ST1 + 6 data bytes + ST2 (regs 0x02..0x09) into EXT_SENS_DATA
    // every sample cycle. Reading ST2 each cycle is what unlatches the AK8963 for the next sample.
    if (!writeRegister(binding.bus, address, I2C_SLV0_ADDR_REGISTER, AK8963_SLV0_ADDR_VALUE)) {
        return false;
    }
    if (!writeRegister(binding.bus, address, I2C_SLV0_REG_REGISTER, AK8963_ST1_REGISTER)) {
        return false;
    }
    if (!writeRegister(binding.bus, address, I2C_SLV0_CTRL_REGISTER, I2C_SLV0_CTRL_VALUE)) {
        return false;
    }
    delay(MAGNETOMETER_MASTER_SETTLE_MS);  // let EXT_SENS_DATA populate before the first read
    return true;
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
    // The MPU9250's internal master continuously copies the AK8963's ST1 + 6 data bytes + ST2
    // into EXT_SENS_DATA; read it here under the MPU9250's own address (no 0x0C on the external
    // bus, so two MPU9250 can share a bus without their magnetometers colliding). One transaction.
    uint8_t rawData[AK8963_SLV0_READ_LENGTH] = {};
    if (!readRegisters(binding.bus, binding.imu.getI2cAddress(), EXT_SENS_DATA_00_REGISTER,
                       rawData, sizeof(rawData))) {
        return false;
    }

    // rawData[0] = ST1: bit 0 (DRDY) signals a fresh measurement.
    if ((rawData[0] & AK8963_DATA_READY_BIT) == 0) {
        return false;
    }

    // rawData[7] = ST2: its HOFL bit flags a magnetic overflow that invalidates this measurement
    // -> skip it (the caller keeps mag at 0, so the orientation filter falls back to 6-DOF here).
    if ((rawData[AK8963_ST2_REGISTER_OFFSET] & AK8963_OVERFLOW_BIT) != 0) {
        return false;
    }

    // Data bytes start at index 1 (ST1 occupies index 0): HXL=1, HYL=3, HZL=5. The AK8963 die is
    // mounted rotated vs the accel/gyro die, so the datasheet remap swaps X/Y and inverts Z; then
    // apply the per-axis factory sensitivity (ASA) and saturate back into int16 for ImuSample.
    magX = saturateToInt16(readLittleEndianInt16(rawData, 3) * binding.magAsa[0]);
    magY = saturateToInt16(readLittleEndianInt16(rawData, 1) * binding.magAsa[1]);
    magZ = saturateToInt16(-readLittleEndianInt16(rawData, 5) * binding.magAsa[2]);
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
