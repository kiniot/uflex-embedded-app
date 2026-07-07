//
// Created by Daniel Crispin on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_RGB_LED_H
#define UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_RGB_LED_H

#include <stdint.h>

#include "uflex/domain/actuators/rgb_led.h"

/**
 * @file gpio_rgb_led.h
 * @brief Declares the shared GPIO-backed RGB LED adapter for uFlex.
 *
 * GpioRgbLed applies the current domain RGB LED state to three GPIO pins. It
 * is shared by simulation and hardware runtimes so the output mapping stays in
 * infrastructure while the desired color remains in the domain.
 *
 * @author Daniel Crispin
 * @date June 16, 2026
 * @version 0.1.0
 */
class GpioRgbLed {
public:
    GpioRgbLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin);

    void begin() const;
    void apply(const RgbLed& led) const;

private:
    static constexpr uint8_t RED_CHANNEL = 0;
    static constexpr uint8_t GREEN_CHANNEL = 1;
    static constexpr uint8_t BLUE_CHANNEL = 2;
    static constexpr uint32_t PWM_FREQUENCY_HZ = 5000;
    static constexpr uint8_t PWM_RESOLUTION_BITS = 8;

    uint8_t redPin;
    uint8_t greenPin;
    uint8_t bluePin;

    void writeChannel(uint8_t channel, bool on, uint8_t brightness) const;
};

#endif // UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_RGB_LED_H
