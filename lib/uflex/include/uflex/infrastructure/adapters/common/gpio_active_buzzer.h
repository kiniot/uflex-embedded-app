//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_ACTIVE_BUZZER_H
#define UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_ACTIVE_BUZZER_H

#include <stdint.h>

#include "uflex/domain/actuators/active_buzzer.h"

/**
 * @file gpio_active_buzzer.h
 * @brief Declares the shared GPIO-backed active buzzer adapter for uFlex.
 *
 * GpioActiveBuzzer mirrors the current domain buzzer state on a single GPIO
 * pin so simulation and hardware can reuse the same output mapping contract.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
class GpioActiveBuzzer {
public:
    explicit GpioActiveBuzzer(uint8_t pin);

    void begin() const;
    void apply(const ActiveBuzzer& buzzer) const;

private:
    uint8_t pin;
};

#endif // UFLEX_INFRASTRUCTURE_ADAPTERS_COMMON_GPIO_ACTIVE_BUZZER_H
