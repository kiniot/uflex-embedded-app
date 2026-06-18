//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file vibration_motor.cpp
 * @brief Implements the domain vibration motor actuator for uFlex.
 *
 * The domain keeps only the current on/off state for the motor. Pulse timing
 * remains an application concern while the active runtime maps the same state
 * through a shared GPIO-backed adapter.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/domain/actuators/vibration_motor.h"

const Command VibrationMotor::TURN_ON_COMMAND(TURN_ON_COMMAND_ID);
const Command VibrationMotor::TURN_OFF_COMMAND(TURN_OFF_COMMAND_ID);

VibrationMotor::VibrationMotor(CommandHandler* commandHandler)
    : Actuator(-1, commandHandler),
      enabled(false) {}

void VibrationMotor::handle(Command command) {
    if (command == TURN_ON_COMMAND) {
        enabled = true;
        return;
    }

    if (command == TURN_OFF_COMMAND) {
        enabled = false;
        return;
    }
}

bool VibrationMotor::isEnabled() const {
    return enabled;
}
