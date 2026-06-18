//
// Created by Daniel Crispin on 16/06/26.
//

/**
 * @file gpio_rgb_led.cpp
 * @brief Implements the shared GPIO-backed RGB LED adapter for uFlex.
 *
 * This adapter configures the three LED channels as outputs and mirrors the
 * current domain actuator state onto the active runtime pins.
 *
 * @author Daniel Crispin
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/adapters/common/gpio_rgb_led.h"

#include <Arduino.h>

GpioRgbLed::GpioRgbLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin)
    : redPin(redPin),
      greenPin(greenPin),
      bluePin(bluePin) {}

void GpioRgbLed::begin() const {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
}

void GpioRgbLed::apply(const RgbLed& led) const {
    const RgbLed::State state = led.getState();

    digitalWrite(redPin, state.redOn ? HIGH : LOW);
    digitalWrite(greenPin, state.greenOn ? HIGH : LOW);
    digitalWrite(bluePin, state.blueOn ? HIGH : LOW);
}
