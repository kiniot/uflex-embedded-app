//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_CORE_DEVICES_UFLEX_DEVICE_H
#define UFLEX_CORE_DEVICES_UFLEX_DEVICE_H

#include <Device.h>
#include <stdint.h>

#include "uflex_core/sensors/inertial_sensor.h"

/**
 * @file uflex_device.h
 * @brief Declares the UflexDevice class.
 *
 * UflexDevice is the central ModestIoT-style device for the uFlex project. It composes three
 * inertial sensors and acts as the common coordination point for sensor events and future
 * command handling.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class UflexDevice : public Device {
public:
    /**
     * @brief Configuration required to construct a single inertial sensor within the device.
     */
    struct SensorConfig {
        int sensorId;
        uint8_t i2cAddress;
    };

    /**
     * @brief Constructs the device with three configured inertial sensors.
     */
    UflexDevice(SensorConfig upperSensorConfig, SensorConfig middleSensorConfig,
                SensorConfig lowerSensorConfig);

    /**
     * @brief Handles propagated sensor events.
     */
    void on(Event event) override;

    /**
     * @brief Handles device-level commands.
     */
    void handle(Command command) override;

    /**
     * @brief Provides access to the upper inertial sensor.
     */
    InertialSensor& getUpperSensor();

    /**
     * @brief Provides access to the middle inertial sensor.
     */
    InertialSensor& getMiddleSensor();

    /**
     * @brief Provides access to the lower inertial sensor.
     */
    InertialSensor& getLowerSensor();

private:
    InertialSensor upperSensor;
    InertialSensor middleSensor;
    InertialSensor lowerSensor;
};

#endif // UFLEX_CORE_DEVICES_UFLEX_DEVICE_H
