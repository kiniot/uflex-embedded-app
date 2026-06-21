//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_RUNTIME_SIM_UFLEX_RUNTIME_H
#define UFLEX_APPLICATION_RUNTIME_SIM_UFLEX_RUNTIME_H

#include <Wire.h>

#include "uflex/application/runtime/uflex_runtime.h"
#include "uflex/infrastructure/adapters/common/gpio_active_buzzer.h"
#include "uflex/infrastructure/adapters/common/gpio_rgb_led.h"
#include "uflex/infrastructure/adapters/common/gpio_vibration_motor.h"
#include "uflex/infrastructure/adapters/sim/simulated_imu_array.h"
#include "uflex/infrastructure/transport/edge_client.h"

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
    void applyOutputs() override;
    UflexDevice& getDevice() override;
    EdgeTransport& getEdgeTransport() override;

private:
    static constexpr uint8_t BUZZER_PIN = 27;
    static constexpr uint8_t VIBRATION_MOTOR_PIN = 32;
    static constexpr uint8_t RGB_RED_PIN = 18;
    static constexpr uint8_t RGB_GREEN_PIN = 19;
    static constexpr uint8_t RGB_BLUE_PIN = 23;
    static constexpr uint8_t PRIMARY_SDA_PIN = 21;
    static constexpr uint8_t PRIMARY_SCL_PIN = 22;
    static constexpr uint8_t SECONDARY_SDA_PIN = 26;
    static constexpr uint8_t SECONDARY_SCL_PIN = 25;

    UflexDevice device;
    TwoWire secondaryBus;
    SimulatedImuArray simulatedImuArray;
    GpioActiveBuzzer statusBuzzer;
    GpioRgbLed statusLed;
    GpioVibrationMotor vibrationMotor;
    EdgeClient edgeClient;
};

#endif // UFLEX_APPLICATION_RUNTIME_SIM_UFLEX_RUNTIME_H
