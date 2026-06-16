//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file gpio_vibration_motor.cpp
 * @brief Implements the shared GPIO-backed vibration motor adapter for uFlex.
 *
 * This adapter configures the motor pin as a digital output and applies the
 * current domain state as a HIGH or LOW level on the selected GPIO.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/adapters/common/gpio_vibration_motor.h"

#include <Arduino.h>

GpioVibrationMotor::GpioVibrationMotor(uint8_t pin)
    : pin(pin) {}

void GpioVibrationMotor::begin() const {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void GpioVibrationMotor::apply(const VibrationMotor& vibrationMotor) const {
    digitalWrite(pin, vibrationMotor.isEnabled() ? HIGH : LOW);
}
