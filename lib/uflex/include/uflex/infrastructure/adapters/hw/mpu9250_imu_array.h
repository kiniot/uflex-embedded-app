//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_ADAPTERS_HW_MPU9250_IMU_ARRAY_H
#define UFLEX_INFRASTRUCTURE_ADAPTERS_HW_MPU9250_IMU_ARRAY_H

#include <Wire.h>
#include <stdint.h>

#include "uflex/domain/sensors/imu.h"

/**
 * @file mpu9250_imu_array.h
 * @brief Declares the preliminary MPU9250 hardware adapter for uFlex.
 *
 * Hardware-side adapter that reads three MPU9250-backed IMUs and updates the corresponding
 * domain Imu objects. It reads the accelerometer, gyroscope, and temperature block from each
 * MPU9250, and additionally enables I2C bypass mode to read the AK8963 magnetometer integrated
 * in the same package, populating the full ImuSample contract used by the motion calculations.
 *
 * The implementation is designed to be structurally ready for real hardware integration, but it
 * still needs validation against the physical board and any final multiplexer wiring.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class Mpu9250ImuArray {
public:
    /**
     * @brief Binds one domain IMU to the I2C bus + TCA9548A channel used to reach it.
     */
    struct ImuBinding {
        Imu& imu;
        TwoWire& bus;
        uint8_t muxChannel;   // TCA9548A channel this IMU sits behind (selected before each access)
        bool hasMagnetometer; // false when the AK8963 is absent/dead -> this IMU runs 6-DOF (set at init)
        float magAsa[3];      // per-axis AK8963 sensitivity-adjustment scale (1.0 until read)
        int16_t gyroBias[3];  // per-axis gyro zero-rate bias in raw LSB (0 until calibrated)
    };

    /**
     * @brief Constructs the adapter for three bound IMUs.
     */
    Mpu9250ImuArray(ImuBinding firstImu, ImuBinding secondImu, ImuBinding thirdImu);

    /**
     * @brief Detects and initializes all configured MPU9250 devices.
     */
    bool begin();

    /**
     * @brief Reads all configured IMUs and updates their domain samples.
     */
    bool update();

private:
    static constexpr uint8_t WHO_AM_I_REGISTER = 0x75;
    static constexpr uint8_t POWER_MANAGEMENT_1_REGISTER = 0x6B;
    static constexpr uint8_t MEASUREMENT_START_REGISTER = 0x3B;
    static constexpr uint8_t EXPECTED_WHO_AM_I = 0x71;
    static constexpr uint8_t INT_PIN_CFG_REGISTER = 0x37;
    static constexpr uint8_t BYPASS_EN_VALUE = 0x02;

    // TCA9548A I2C multiplexer: the 3 IMUs share one bus, each isolated on its own channel. Writing
    // (1 << channel) to this address routes the bus to that channel before every IMU/AK8963 access.
    static constexpr uint8_t TCA9548A_ADDRESS = 0x70;

    static constexpr uint8_t AK8963_I2C_ADDRESS = 0x0C;
    static constexpr uint8_t AK8963_WHO_AM_I_REGISTER = 0x00;
    static constexpr uint8_t AK8963_EXPECTED_WHO_AM_I = 0x48;
    static constexpr uint8_t AK8963_CNTL1_REGISTER = 0x0A;
    static constexpr uint8_t AK8963_CONTINUOUS_MEASUREMENT_16BIT_MODE2 = 0x16;
    static constexpr uint8_t AK8963_ST1_REGISTER = 0x02;
    static constexpr uint8_t AK8963_MEASUREMENT_START_REGISTER = 0x03;
    static constexpr uint8_t AK8963_DATA_READY_BIT = 0x01;
    static constexpr uint8_t AK8963_ASA_START_REGISTER = 0x10;
    static constexpr uint8_t AK8963_FUSE_ROM_ACCESS_MODE = 0x0F;
    static constexpr uint8_t AK8963_POWER_DOWN_MODE = 0x00;
    // The AK8963 is read directly at 0x0C (bypass-per-channel) in one 8-byte transaction
    // [ST1, HXL..HZH, ST2] starting at ST1 (index 0). ST2 (index 7) must be read each cycle to
    // unlatch the AK8963 for the next sample.
    static constexpr uint8_t AK8963_READ_LENGTH = 8;
    static constexpr uint8_t AK8963_ST2_REGISTER_OFFSET = 7;
    static constexpr uint8_t AK8963_OVERFLOW_BIT = 0x08;
    static constexpr uint16_t GYRO_BIAS_CALIBRATION_SAMPLES = 100;

    ImuBinding imus[3];

    bool detectMux(TwoWire& bus);
    bool selectMuxChannel(TwoWire& bus, uint8_t channel);
    bool initializeImu(ImuBinding& binding);
    bool detectImu(ImuBinding& binding, uint8_t& whoAmI);
    bool wakeImu(ImuBinding& binding);
    bool enableMagnetometerBypass(ImuBinding& binding);
    bool initializeMagnetometer(ImuBinding& binding);
    bool updateImu(ImuBinding& binding);
    void calibrateGyroBias(ImuBinding& binding);
    bool readMagnetometer(ImuBinding& binding, int16_t& magX, int16_t& magY, int16_t& magZ);
    bool readRegisters(TwoWire& bus, uint8_t address, uint8_t startRegister, uint8_t* buffer,
                       size_t length);
    bool writeRegister(TwoWire& bus, uint8_t address, uint8_t reg, uint8_t value);
    static int16_t readBigEndianInt16(const uint8_t* buffer, size_t offset);
    static int16_t readLittleEndianInt16(const uint8_t* buffer, size_t offset);
    static int16_t saturateToInt16(float value);
};

#endif // UFLEX_INFRASTRUCTURE_ADAPTERS_HW_MPU9250_IMU_ARRAY_H
