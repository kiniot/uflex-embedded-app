//
// Created by Daniel Crispin on 16/06/26.
//

#ifndef UFLEX_DOMAIN_ACTUATORS_RGB_LED_H
#define UFLEX_DOMAIN_ACTUATORS_RGB_LED_H

#include <stdint.h>

#include <Actuator.h>

/**
 * @file rgb_led.h
 * @brief Declares the domain RGB LED actuator for uFlex.
 *
 * RgbLed models a tri-channel indicator as a domain actuator aligned with the
 * ModestIoT command flow. It keeps the current color state in the domain so
 * each runtime can apply that state through its environment-specific adapter.
 *
 * @author Daniel Crispin
 * @date June 16, 2026
 * @version 0.1.0
 */
class RgbLed : public Actuator {
public:
    enum class Color {
        off,
        red,
        green,
        blue,
        yellow,
        cyan,
        magenta,
    };

    struct State {
        bool redOn;
        bool greenOn;
        bool blueOn;
        uint8_t brightness;
    };

    static constexpr int ADVANCE_COLOR_COMMAND_ID = 100;
    static constexpr int TURN_OFF_COMMAND_ID = 101;

    static const Command ADVANCE_COLOR_COMMAND;
    static const Command TURN_OFF_COMMAND;

    explicit RgbLed(CommandHandler* commandHandler = nullptr);

    void handle(Command command) override;

    void setColor(Color nextColor);
    void setBrightness(uint8_t nextBrightness);
    Color getColor() const;
    State getState() const;

private:
    Color color;
    uint8_t brightness;

    void advanceColor();
};

#endif // UFLEX_DOMAIN_ACTUATORS_RGB_LED_H
