//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_DOMAIN_DEVICES_UFLEX_DEVICE_H
#define UFLEX_DOMAIN_DEVICES_UFLEX_DEVICE_H

#include <Device.h>
#include <stdint.h>

#include "uflex/domain/actuators/active_buzzer.h"
#include "uflex/domain/actuators/rgb_led.h"
#include "uflex/domain/actuators/vibration_motor.h"
#include "uflex/domain/devices/motion_state.h"
#include "uflex/domain/services/relative_angle_calculator.h"
#include "uflex/domain/sensors/imu.h"

/**
 * @file uflex_device.h
 * @brief Declares the central domain device for uFlex.
 *
 * UflexDevice is the main ModestIoT-aligned coordinator in the domain layer.
 * It owns three IMUs, reacts to their motion events, recalculates the current
 * relative angles, and exposes the resulting motion state to upper layers.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class UflexDevice : public Device {
private:
    Imu upperImu;
    Imu middleImu;
    Imu lowerImu;
    ActiveBuzzer statusBuzzer;
    RgbLed statusLed;
    VibrationMotor vibrationMotor;
    RelativeAngle upperMiddleAngle;
    RelativeAngle middleLowerAngle;
    RelativeAngle upperLowerAngle;

public:
    /**
     * @brief Describes the information required to construct a domain IMU.
     */
    struct ImuConfig {
        int imuId;
        uint8_t i2cAddress;
    };

    /**
     * @brief Constructs the device with three configured IMUs.
     */
    UflexDevice(ImuConfig upperImuConfig, ImuConfig middleImuConfig,
                ImuConfig lowerImuConfig);

    /**
     * @brief Handles propagated domain events.
     */
    void on(Event event) override;

    /**
     * @brief Handles device-level commands.
     */
    void handle(Command command) override;

    /**
     * @brief Returns the upper IMU.
     */
    Imu& getUpperImu();

    /**
     * @brief Returns the middle IMU.
     */
    Imu& getMiddleImu();

    /**
     * @brief Returns the lower IMU.
     */
    Imu& getLowerImu();

    /**
     * @brief Returns the domain active buzzer.
     */
    ActiveBuzzer& getStatusBuzzer();

    /**
     * @brief Returns the domain RGB status LED.
     */
    RgbLed& getStatusLed();

    /**
     * @brief Returns the domain vibration motor.
     */
    VibrationMotor& getVibrationMotor();

    /**
     * @brief Returns the relative angle between the upper and middle IMUs.
     */
    RelativeAngle getUpperMiddleAngle() const;

    /**
     * @brief Returns the relative angle between the middle and lower IMUs.
     */
    RelativeAngle getMiddleLowerAngle() const;

    /**
     * @brief Returns the relative angle between the upper and lower IMUs.
     */
    RelativeAngle getUpperLowerAngle() const;

    /**
     * @brief Returns the current aggregated motion state.
     */
    MotionState getMotionState() const;
};

#endif // UFLEX_DOMAIN_DEVICES_UFLEX_DEVICE_H
