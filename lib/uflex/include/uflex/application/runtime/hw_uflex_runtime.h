//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_APPLICATION_RUNTIME_HW_UFLEX_RUNTIME_H
#define UFLEX_APPLICATION_RUNTIME_HW_UFLEX_RUNTIME_H

#include <Wire.h>

#include "uflex/application/runtime/uflex_runtime.h"
#include "uflex/infrastructure/adapters/hw/mpu9250_imu_array.h"

/**
 * @file hw_uflex_runtime.h
 * @brief Declares the hardware runtime for uFlex.
 *
 * HwUflexRuntime wires the domain device to the preliminary MPU9250 hardware
 * adapter and owns the buses required by the real-device target, keeping
 * hardware-specific initialization outside the firmware entry point.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class HwUflexRuntime : public UflexRuntime {
public:
    HwUflexRuntime();

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
    Mpu9250ImuArray hardwareImuArray;
};

#endif // UFLEX_APPLICATION_RUNTIME_HW_UFLEX_RUNTIME_H
