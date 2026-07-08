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
    ledcSetup(RED_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
    ledcSetup(GREEN_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
    ledcSetup(BLUE_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);

    ledcAttachPin(redPin, RED_CHANNEL);
    ledcAttachPin(greenPin, GREEN_CHANNEL);
    ledcAttachPin(bluePin, BLUE_CHANNEL);

    ledcWrite(RED_CHANNEL, 0);
    ledcWrite(GREEN_CHANNEL, 0);
    ledcWrite(BLUE_CHANNEL, 0);
}

void GpioRgbLed::apply(const RgbLed& led) const {
    const RgbLed::State state = led.getState();

    writeChannel(RED_CHANNEL, state.redOn, state.brightness);
    writeChannel(GREEN_CHANNEL, state.greenOn, state.brightness);
    writeChannel(BLUE_CHANNEL, state.blueOn, state.brightness);
}

void GpioRgbLed::writeChannel(uint8_t channel, bool on, uint8_t brightness) const {
    ledcWrite(channel, on ? brightness : 0);
}
