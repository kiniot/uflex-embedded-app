//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_ADAPTERS_SIM_SIMULATED_IMU_ARRAY_H
#define UFLEX_INFRASTRUCTURE_ADAPTERS_SIM_SIMULATED_IMU_ARRAY_H

#include <Wire.h>
#include <stdint.h>

#include "uflex/domain/sensors/imu.h"

/**
 * @file simulated_imu_array.h
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class SimulatedImuArray {
public:
    struct ImuBinding {
        Imu& imu;
        TwoWire& bus;
    };

    SimulatedImuArray(ImuBinding firstImu,
        ImuBinding secondImu,
        ImuBinding thirdImu);

    bool begin();

    bool update();

private:
    static constexpr uint8_t WHO_AM_I_REGISTER = 0x75;
    static constexpr uint8_t POWER_MANAGEMENT_1_REGISTER = 0x6B;
    static constexpr uint8_t MEASUREMENT_START_REGISTER = 0x3B;

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

#endif // UFLEX_INFRASTRUCTURE_ADAPTERS_SIM_SIMULATED_IMU_ARRAY_H
