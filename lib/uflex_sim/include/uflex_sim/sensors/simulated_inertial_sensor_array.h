//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_SIM_SENSORS_SIMULATED_INERTIAL_SENSOR_ARRAY_H
#define UFLEX_SIM_SENSORS_SIMULATED_INERTIAL_SENSOR_ARRAY_H

#include <Wire.h>
#include <stdint.h>

#include "uflex_core/sensors/inertial_sensor.h"

/**
 * @file simulated_inertial_sensor_array.h
 * @brief Declares the SimulatedInertialSensorArray class.
 *
 * SimulatedInertialSensorArray is a simulation-side adapter that reads three MPU6050 sensors
 * from Wokwi and updates the corresponding uFlex core InertialSensor objects.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class SimulatedInertialSensorArray {
public:
    /**
     * @brief Describes one simulated inertial sensor and the I2C bus used to access it.
     */
    struct SensorBinding {
        InertialSensor& sensor;
        TwoWire& bus;
    };

    SimulatedInertialSensorArray(SensorBinding firstSensor, SensorBinding secondSensor,
                                 SensorBinding thirdSensor);

    /**
     * @brief Detects and wakes all configured simulated sensors.
     * @return true when all sensors are detected and initialized successfully.
     */
    bool begin();

    /**
     * @brief Reads all configured simulated sensors and updates their stored samples.
     * @return true when all sensor reads succeed.
     */
    bool update();

private:
    static constexpr uint8_t WHO_AM_I_REGISTER = 0x75;
    static constexpr uint8_t POWER_MANAGEMENT_1_REGISTER = 0x6B;
    static constexpr uint8_t MEASUREMENT_START_REGISTER = 0x3B;

    SensorBinding sensors[3];

    bool initializeSensor(SensorBinding& binding);
    bool detectSensor(SensorBinding& binding, uint8_t& whoAmI);
    bool wakeSensor(SensorBinding& binding);
    bool updateSensor(SensorBinding& binding);
    bool readRegisters(TwoWire& bus, uint8_t address, uint8_t startRegister, uint8_t* buffer,
                       size_t length);
    bool writeRegister(TwoWire& bus, uint8_t address, uint8_t reg, uint8_t value);
    static int16_t readBigEndianInt16(const uint8_t* buffer, size_t offset);
};

#endif // UFLEX_SIM_SENSORS_SIMULATED_INERTIAL_SENSOR_ARRAY_H
