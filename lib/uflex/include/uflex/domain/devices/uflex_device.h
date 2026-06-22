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
#include "uflex/domain/math/quaternion.h"
#include "uflex/domain/services/orientation_filter.h"
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
    OrientationFilter upperOrientationFilter;
    OrientationFilter middleOrientationFilter;
    OrientationFilter lowerOrientationFilter;
    Quaternion upperMiddleRotation;
    Quaternion middleLowerRotation;
    Quaternion upperLowerRotation;
    Quaternion upperOrientation;

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
     * @brief Fuses each IMU's latest sample into its orientation filter and
     * recalculates the relative joint rotations.
     *
     * This is driven explicitly by the application loop (rather than reactively
     * by IMU events) because the underlying filter needs the elapsed time since
     * its previous call, which only the loop cadence knows.
     *
     * @param deltaTimeSeconds Elapsed time since the previous call, in seconds.
     */
    void updateOrientations(float deltaTimeSeconds);

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
     * @brief Returns the fused relative rotation between the upper and middle IMUs.
     */
    Quaternion getUpperMiddleRotation() const;

    /**
     * @brief Returns the fused relative rotation between the middle and lower IMUs.
     */
    Quaternion getMiddleLowerRotation() const;

    /**
     * @brief Returns the fused relative rotation between the upper and lower IMUs.
     */
    Quaternion getUpperLowerRotation() const;

    /**
     * @brief Returns the upper segment's absolute fused orientation; its yaw is
     * the proximal-segment compensation signal.
     */
    Quaternion getUpperOrientation() const;

    /**
     * @brief Returns the current aggregated motion state.
     */
    MotionState getMotionState() const;
};

#endif // UFLEX_DOMAIN_DEVICES_UFLEX_DEVICE_H
