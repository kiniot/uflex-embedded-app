//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_DOMAIN_ACTUATORS_ACTIVE_BUZZER_H
#define UFLEX_DOMAIN_ACTUATORS_ACTIVE_BUZZER_H

#include <Actuator.h>

/**
 * @file active_buzzer.h
 * @brief Declares the domain active buzzer actuator for uFlex.
 *
 * ActiveBuzzer models the current enabled state of a simple active buzzer so
 * upper layers can request pulses through explicit commands while the active
 * runtime applies that state through an infrastructure adapter.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
class ActiveBuzzer : public Actuator {
public:
    static constexpr int TURN_ON_COMMAND_ID = 110;
    static constexpr int TURN_OFF_COMMAND_ID = 111;

    static const Command TURN_ON_COMMAND;
    static const Command TURN_OFF_COMMAND;

    explicit ActiveBuzzer(CommandHandler* commandHandler = nullptr);

    void handle(Command command) override;

    bool isEnabled() const;

private:
    bool enabled;
};

#endif // UFLEX_DOMAIN_ACTUATORS_ACTIVE_BUZZER_H
