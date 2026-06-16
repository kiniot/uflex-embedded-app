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
    uint8_t redPin;
    uint8_t greenPin;
    uint8_t bluePin;
};

#endif // UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_RGB_LED_H
