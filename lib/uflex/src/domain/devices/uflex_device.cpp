//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

/**
 * @file uflex_device.cpp
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

#include "uflex/domain/devices/uflex_device.h"

UflexDevice::UflexDevice(ImuConfig upperImuConfig, ImuConfig middleImuConfig,
                         ImuConfig lowerImuConfig)
    : upperImu(upperImuConfig.imuId, upperImuConfig.i2cAddress, this),
      middleImu(middleImuConfig.imuId, middleImuConfig.i2cAddress, this),
      lowerImu(lowerImuConfig.imuId, lowerImuConfig.i2cAddress, this),
      upperMiddleAngle{0.0f, 0.0f},
      middleLowerAngle{0.0f, 0.0f},
      upperLowerAngle{0.0f, 0.0f} {}

void UflexDevice::on(Event event) {
    if (event == Imu::MOTION_DETECTED_EVENT) {
        upperMiddleAngle = RelativeAngleCalculator::calculate(
            upperImu.getLastSample(),
            middleImu.getLastSample()
        );
        middleLowerAngle = RelativeAngleCalculator::calculate(
            middleImu.getLastSample(),
            lowerImu.getLastSample()
        );
        upperLowerAngle = RelativeAngleCalculator::calculate(
            upperImu.getLastSample(),
            lowerImu.getLastSample()
        );
    }
}

void UflexDevice::handle(Command command) {
    (void)command;
}

Imu& UflexDevice::getUpperImu() {
    return upperImu;
}

Imu& UflexDevice::getMiddleImu() {
    return middleImu;
}

Imu& UflexDevice::getLowerImu() {
    return lowerImu;
}

RelativeAngle UflexDevice::getUpperMiddleAngle() const {
    return upperMiddleAngle;
}

RelativeAngle UflexDevice::getMiddleLowerAngle() const {
    return middleLowerAngle;
}

RelativeAngle UflexDevice::getUpperLowerAngle() const {
    return upperLowerAngle;
}

MotionState UflexDevice::getMotionState() const {
    return MotionState{
        upperImu.getLastSample(),
        middleImu.getLastSample(),
        lowerImu.getLastSample(),
        upperMiddleAngle,
        middleLowerAngle,
        upperLowerAngle,
    };
}
