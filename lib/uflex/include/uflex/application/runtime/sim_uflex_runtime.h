//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_RUNTIME_SIM_UFLEX_RUNTIME_H
#define UFLEX_APPLICATION_RUNTIME_SIM_UFLEX_RUNTIME_H

#include <Wire.h>

#include "uflex/application/runtime/uflex_runtime.h"
#include "uflex/infrastructure/adapters/sim/simulated_imu_array.h"

/**
 * @file sim_uflex_runtime.h
 * @brief Declares the simulation runtime for uFlex.
 *
 * SimUflexRuntime wires the domain device to the simulated IMU adapter and
 * owns the I2C resources required by the Wokwi-oriented target so the rest of
 * the application can run without simulation-specific setup code in main.cpp.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class SimUflexRuntime : public UflexRuntime {
public:
    SimUflexRuntime();

    bool begin() override;
    bool update() override;
    UflexDevice& getDevice() override;

private:
    static constexpr uint8_t PRIMARY_SDA_PIN = 21;
    static constexpr uint8_t PRIMARY_SCL_PIN = 22;
    static constexpr uint8_t SECONDARY_SDA_PIN = 26;
    static constexpr uint8_t SECONDARY_SCL_PIN = 25;

    UflexDevice device;
    TwoWire secondaryBus;
    SimulatedImuArray simulatedImuArray;
};

#endif // UFLEX_APPLICATION_RUNTIME_SIM_UFLEX_RUNTIME_H
