//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file gpio_active_buzzer.cpp
 * @brief Implements the shared GPIO-backed active buzzer adapter for uFlex.
 *
 * This adapter configures the buzzer pin as a digital output and applies the
 * current domain state as a HIGH or LOW level on the selected GPIO.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/adapters/common/gpio_active_buzzer.h"

#include <Arduino.h>

GpioActiveBuzzer::GpioActiveBuzzer(uint8_t pin)
    : pin(pin) {}

void GpioActiveBuzzer::begin() const {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void GpioActiveBuzzer::apply(const ActiveBuzzer& buzzer) const {
    digitalWrite(pin, buzzer.isEnabled() ? HIGH : LOW);
}
