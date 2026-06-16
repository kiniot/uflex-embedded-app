//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_VIBRATION_MOTOR_H
#define UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_VIBRATION_MOTOR_H

#include <stdint.h>

#include "uflex/domain/actuators/vibration_motor.h"

/**
 * @file gpio_vibration_motor.h
 * @brief Declares the shared GPIO-backed vibration motor adapter for uFlex.
 *
 * GpioVibrationMotor mirrors the current domain motor state on a single GPIO
 * pin so simulation and hardware can reuse the same output mapping contract.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
class GpioVibrationMotor {
public:
    explicit GpioVibrationMotor(uint8_t pin);

    void begin() const;
    void apply(const VibrationMotor& vibrationMotor) const;

private:
    uint8_t pin;
};

#endif // UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_VIBRATION_MOTOR_H
