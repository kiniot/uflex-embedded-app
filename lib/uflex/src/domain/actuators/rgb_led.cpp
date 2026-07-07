//
// Created by Daniel Crispin on 16/06/26.
//

/**
 * @file rgb_led.cpp
 * @brief Implements the domain RGB LED actuator for uFlex.
 *
 * The current implementation keeps a simple color cycle in the domain so the
 * application can exercise the actuator consistently across simulation and
 * hardware while the rest of the output stack stays environment-specific.
 *
 * @author Daniel Crispin
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/domain/actuators/rgb_led.h"

const Command RgbLed::ADVANCE_COLOR_COMMAND(ADVANCE_COLOR_COMMAND_ID);
const Command RgbLed::TURN_OFF_COMMAND(TURN_OFF_COMMAND_ID);

RgbLed::RgbLed(CommandHandler* commandHandler)
    : Actuator(-1, commandHandler),
      color(Color::off),
      brightness(0) {}

void RgbLed::handle(Command command) {
    if (command == ADVANCE_COLOR_COMMAND) {
        advanceColor();
        return;
    }

    if (command == TURN_OFF_COMMAND) {
        setColor(Color::off);
        return;
    }
}

RgbLed::Color RgbLed::getColor() const {
    return color;
}

RgbLed::State RgbLed::getState() const {
    switch (color) {
        case Color::red:
            return {true, false, false, brightness};
        case Color::green:
            return {false, true, false, brightness};
        case Color::blue:
            return {false, false, true, brightness};
        case Color::yellow:
            return {true, true, false, brightness};
        case Color::cyan:
            return {false, true, true, brightness};
        case Color::magenta:
            return {true, false, true, brightness};
        case Color::off:
        default:
            return {false, false, false, 0};
    }
}

void RgbLed::setColor(Color nextColor) {
    color = nextColor;
    brightness = nextColor == Color::off ? 0 : 255;
}

void RgbLed::setBrightness(uint8_t nextBrightness) {
    brightness = color == Color::off ? 0 : nextBrightness;
}

void RgbLed::advanceColor() {
    switch (color) {
        case Color::off:
            setColor(Color::red);
            return;
        case Color::red:
            setColor(Color::green);
            return;
        case Color::green:
            setColor(Color::blue);
            return;
        case Color::blue:
            setColor(Color::yellow);
            return;
        case Color::yellow:
            setColor(Color::cyan);
            return;
        case Color::cyan:
            setColor(Color::magenta);
            return;
        case Color::magenta:
        default:
            setColor(Color::off);
            return;
    }
}
