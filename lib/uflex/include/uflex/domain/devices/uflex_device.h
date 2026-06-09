//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_DOMAIN_DEVICES_UFLEX_DEVICE_H
#define UFLEX_DOMAIN_DEVICES_UFLEX_DEVICE_H

#include <Device.h>
#include <stdint.h>

#include "uflex/domain/devices/motion_state.h"
#include "uflex/domain/services/relative_angle_calculator.h"
#include "uflex/domain/sensors/imu.h"

/**
 * @file uflex_device.h
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
    RelativeAngle upperMiddleAngle;
    RelativeAngle middleLowerAngle;
    RelativeAngle upperLowerAngle;

public:
    struct ImuConfig {
        int imuId;
        uint8_t i2cAddress;
    };

    UflexDevice(ImuConfig upperImuConfig, ImuConfig middleImuConfig,
                ImuConfig lowerImuConfig);

    void on(Event event) override;

    void handle(Command command) override;

    Imu& getUpperImu();

    Imu& getMiddleImu();

    Imu& getLowerImu();

    RelativeAngle getUpperMiddleAngle() const;

    RelativeAngle getMiddleLowerAngle() const;

    RelativeAngle getUpperLowerAngle() const;

    MotionState getMotionState() const;
};

#endif // UFLEX_DOMAIN_DEVICES_UFLEX_DEVICE_H
