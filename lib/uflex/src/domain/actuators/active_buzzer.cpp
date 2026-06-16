//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file active_buzzer.cpp
 * @brief Implements the domain active buzzer actuator for uFlex.
 *
 * The domain keeps only the current on/off state for the buzzer. Pulse timing
 * remains an application concern while hardware and simulation map the same
 * state through their corresponding GPIO-backed adapter.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/domain/actuators/active_buzzer.h"

const Command ActiveBuzzer::TURN_ON_COMMAND(TURN_ON_COMMAND_ID);
const Command ActiveBuzzer::TURN_OFF_COMMAND(TURN_OFF_COMMAND_ID);

ActiveBuzzer::ActiveBuzzer(CommandHandler* commandHandler)
    : Actuator(-1, commandHandler),
      enabled(false) {}

void ActiveBuzzer::handle(Command command) {
    if (command == TURN_ON_COMMAND) {
        enabled = true;
        return;
    }

    if (command == TURN_OFF_COMMAND) {
        enabled = false;
        return;
    }
}

bool ActiveBuzzer::isEnabled() const {
    return enabled;
}
