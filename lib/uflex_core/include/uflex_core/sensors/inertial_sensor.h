//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_CORE_SENSORS_INERTIAL_SENSOR_H
#define UFLEX_CORE_SENSORS_INERTIAL_SENSOR_H

#include <Sensor.h>
#include <stdint.h>

#include "uflex_core/sensors/inertial_sample.h"

/**
 * @file inertial_sensor.h
 * @brief Declares the InertialSensor class.
 *
 * InertialSensor adapts the ModestIoT Sensor abstraction to the uFlex domain. It models an
 * inertial measurement unit that stores the latest sample, exposes its I2C address, and emits
 * a motion-related event when its state is updated.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class InertialSensor : public Sensor {
public:
    static constexpr int MOTION_DETECTED_EVENT_ID = 0;
    static const Event MOTION_DETECTED_EVENT;

    /**
     * @brief Constructs an inertial sensor model.
     * @param sensorId Logical identifier passed to the ModestIoT Sensor base class.
     *        It is used to preserve framework compatibility and does not necessarily represent
     *        a direct GPIO data pin of the physical IMU.
     * @param i2cAddress I2C address of the physical or simulated sensor.
     * @param eventHandler Optional handler that receives propagated sensor events.
     */
    InertialSensor(int sensorId, uint8_t i2cAddress, EventHandler* eventHandler = nullptr);

    /**
     * @brief Gets the configured I2C address of the sensor.
     */
    uint8_t getI2cAddress() const;

    /**
     * @brief Returns the most recently stored inertial sample.
     */
    const InertialSample& getLastSample() const;

    /**
     * @brief Updates the stored sample and propagates a motion event.
     * @param sample The new inertial reading to store.
     */
    void updateSample(const InertialSample& sample);

private:
    uint8_t i2cAddress;
    InertialSample lastSample;
};

#endif // UFLEX_CORE_SENSORS_INERTIAL_SENSOR_H
