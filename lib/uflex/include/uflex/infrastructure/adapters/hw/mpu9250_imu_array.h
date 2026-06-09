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
 * domain Imu objects. This first version intentionally focuses on accelerometer, gyroscope,
 * and temperature data so it can match the current ImuSample contract already used by the
 * simulator and the core motion calculations.
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
     * @brief Binds one domain IMU to the I2C bus used to reach it.
     */
    struct ImuBinding {
        Imu& imu;
        TwoWire& bus;
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

    ImuBinding imus[3];

    bool initializeImu(ImuBinding& binding);
    bool detectImu(ImuBinding& binding, uint8_t& whoAmI);
    bool wakeImu(ImuBinding& binding);
    bool updateImu(ImuBinding& binding);
    bool readRegisters(TwoWire& bus, uint8_t address, uint8_t startRegister, uint8_t* buffer,
                       size_t length);
    bool writeRegister(TwoWire& bus, uint8_t address, uint8_t reg, uint8_t value);
    static int16_t readBigEndianInt16(const uint8_t* buffer, size_t offset);
};

#endif // UFLEX_INFRASTRUCTURE_ADAPTERS_HW_MPU9250_IMU_ARRAY_H
