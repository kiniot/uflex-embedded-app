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
    // (raw mag passes through) and zero gyro bias (raw gyro passes through). hasMagnetometer is
    // discovered at init (true only if the AK8963 answers), so start pessimistic.
    for (ImuBinding& binding : imus) {
        binding.hasMagnetometer = false;
        binding.magAsa[0] = 1.0f;
        binding.magAsa[1] = 1.0f;
        binding.magAsa[2] = 1.0f;
        binding.gyroBias[0] = 0;
        binding.gyroBias[1] = 0;
        binding.gyroBias[2] = 0;
    }
}

bool Mpu9250ImuArray::begin() {
    // All IMUs sit behind the TCA9548A on one bus; probe the mux first (mirrors the bring-up scan).
    if (detectMux(imus[0].bus)) {
        Serial.printf("TCA9548A [0x%02X] detected\n", TCA9548A_ADDRESS);
    } else {
        Serial.printf("TCA9548A [0x%02X] absent -> IMUs unreachable\n", TCA9548A_ADDRESS);
    }

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

bool Mpu9250ImuArray::detectMux(TwoWire& bus) {
    bus.beginTransmission(TCA9548A_ADDRESS);
    return bus.endTransmission() == 0;
}

bool Mpu9250ImuArray::selectMuxChannel(TwoWire& bus, uint8_t channel) {
    // Route the shared bus to one IMU. Called before every per-IMU operation; the mux holds the
    // selection until the next write, and begin()/update() touch one IMU at a time (no interleaving).
    bus.beginTransmission(TCA9548A_ADDRESS);
    bus.write(static_cast<uint8_t>(1 << channel));
    return bus.endTransmission() == 0;
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
    // Route the shared bus to this IMU's mux channel; every access below inherits the selection.
    // The IMUs all share address 0x68, so logs are keyed by channel, not address.
    if (!selectMuxChannel(binding.bus, binding.muxChannel)) {
        Serial.printf("MPU9250 [ch%u] mux channel select failed\n", binding.muxChannel);
        return false;
    }

    uint8_t whoAmI = 0;
    if (!detectImu(binding, whoAmI)) {
        Serial.printf("MPU9250 [ch%u] not detected\n", binding.muxChannel);
        return false;
    }

    Serial.printf("MPU9250 [ch%u] WHO_AM_I=0x%02X\n", binding.muxChannel, whoAmI);

    if (whoAmI != EXPECTED_WHO_AM_I) {
        Serial.printf("MPU9250 [ch%u] unexpected WHO_AM_I value\n", binding.muxChannel);
        return false;
    }

    // Device reset (PWR_MGMT_1 H_RESET) to a known state. The MPU9250 keeps its registers across an
    // ESP32-only reset (it is not power-cycled), so leftover config from a prior boot would make the
    // bypass-based AK8963 init fail. Reset clears that.
    writeRegister(binding.bus, binding.imu.getI2cAddress(), POWER_MANAGEMENT_1_REGISTER, 0x80);
    delay(100);

    if (!wakeImu(binding)) {
        Serial.printf("MPU9250 [ch%u] wake-up failed\n", binding.muxChannel);
        return false;
    }

    // Enable I2C bypass and leave it on: with the mux isolating each channel, only one AK8963 at
    // 0x0C is ever visible, so it is directly addressable per channel (no I2C-master indirection).
    if (!enableMagnetometerBypass(binding)) {
        Serial.printf("MPU9250 [ch%u] magnetometer bypass failed\n", binding.muxChannel);
        return false;
    }

    // Tolerate a dead/absent magnetometer (one IMU's AK8963 is dead): the accel/gyro are still
    // valid, so keep the IMU and let the orientation filter fall back to 6-DOF. Logged once here,
    // never per-cycle.
    if (initializeMagnetometer(binding)) {
        binding.hasMagnetometer = true;
        Serial.printf("MPU9250 [ch%u] AK8963 magnetometer ready\n", binding.muxChannel);
    } else {
        binding.hasMagnetometer = false;
        Serial.printf("MPU9250 [ch%u] AK8963 absent -> 6-DOF (yaw will drift on this segment)\n",
                      binding.muxChannel);
    }

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
    // Select this IMU's channel; the accel/gyro read and the AK8963 read below target the same one.
    if (!selectMuxChannel(binding.bus, binding.muxChannel)) {
        return false;
    }

    uint8_t rawData[14] = {};
    if (!readRegisters(binding.bus, binding.imu.getI2cAddress(), MEASUREMENT_START_REGISTER,
                       rawData, sizeof(rawData))) {
        Serial.printf("MPU9250 [ch%u] read failed\n", binding.muxChannel);
        return false;
    }

    // Only IMUs with a working magnetometer read it; a transient failure just leaves mag at 0 for
    // this cycle (6-DOF), and the dead-mag IMU never reads it -> no per-cycle log spam.
    int16_t magX = 0;
    int16_t magY = 0;
    int16_t magZ = 0;
    if (binding.hasMagnetometer) {
        readMagnetometer(binding, magX, magY, magZ);
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
    // Bypass-per-channel: the mux isolates this channel so the AK8963 at the fixed 0x0C is directly
    // addressable. Read ST1 + 6 data bytes + ST2 (regs 0x02..0x09) in one transaction; the caller
    // has already selected this IMU's mux channel. Reading ST2 unlatches the AK8963 for next sample.
    uint8_t rawData[AK8963_READ_LENGTH] = {};
    if (!readRegisters(binding.bus, AK8963_I2C_ADDRESS, AK8963_ST1_REGISTER, rawData,
                       sizeof(rawData))) {
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
    // Hold this IMU's channel for the whole still-sampling window (no other IMU is touched here).
    if (!selectMuxChannel(binding.bus, binding.muxChannel)) {
        Serial.printf("MPU9250 [ch%u] gyro calibration: mux select failed; using zero bias\n",
                      binding.muxChannel);
        return;
    }

    Serial.printf("MPU9250 [ch%u] gyro calibration: keep the kit still...\n", binding.muxChannel);

    GyroBiasCalibrator calibrator(GYRO_BIAS_CALIBRATION_SAMPLES);
    for (uint16_t sampleIndex = 0; sampleIndex < GYRO_BIAS_CALIBRATION_SAMPLES; ++sampleIndex) {
        uint8_t rawData[14] = {};
        if (!readRegisters(binding.bus, binding.imu.getI2cAddress(), MEASUREMENT_START_REGISTER,
                           rawData, sizeof(rawData))) {
            Serial.printf("MPU9250 [ch%u] gyro calibration read failed; using zero bias\n",
                          binding.muxChannel);
            return;  // soft-fail: gyroBias stays at its identity {0,0,0}
        }
        calibrator.addSample(readBigEndianInt16(rawData, 8), readBigEndianInt16(rawData, 10),
                             readBigEndianInt16(rawData, 12));
        delay(3);
    }

    calibrator.getBias(binding.gyroBias[0], binding.gyroBias[1], binding.gyroBias[2]);
    Serial.printf("MPU9250 [ch%u] gyro bias=(%d, %d, %d)\n", binding.muxChannel,
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
