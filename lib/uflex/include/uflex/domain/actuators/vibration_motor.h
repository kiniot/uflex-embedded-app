//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_DOMAIN_ACTUATORS_VIBRATION_MOTOR_H
#define UFLEX_DOMAIN_ACTUATORS_VIBRATION_MOTOR_H

#include <Actuator.h>

/**
 * @file vibration_motor.h
 * @brief Declares the domain vibration motor actuator for uFlex.
 *
 * VibrationMotor models the enabled state of the haptic motor so upper layers
 * can request pulses through explicit commands while infrastructure adapters
 * apply that state in simulation and hardware.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
class VibrationMotor : public Actuator {
public:
    static constexpr int TURN_ON_COMMAND_ID = 120;
    static constexpr int TURN_OFF_COMMAND_ID = 121;

    static const Command TURN_ON_COMMAND;
    static const Command TURN_OFF_COMMAND;

    explicit VibrationMotor(CommandHandler* commandHandler = nullptr);

    void handle(Command command) override;

    bool isEnabled() const;

private:
    bool enabled;
};

#endif // UFLEX_DOMAIN_ACTUATORS_VIBRATION_MOTOR_H
