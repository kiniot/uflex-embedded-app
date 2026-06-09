//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/application/runtime/sim_uflex_runtime.h"

#include <Arduino.h>

/**
 * @file sim_uflex_runtime.cpp
 * @brief Implements the simulation runtime for uFlex.
 *
 * This runtime configures the buses expected by the simulator, initializes the
 * simulated IMU array, and updates the shared domain device through the
 * simulation adapter on each application cycle.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

SimUflexRuntime::SimUflexRuntime()
    : device({0, 0x68}, {1, 0x69}, {2, 0x68}),
      secondaryBus(1),
      simulatedImuArray({device.getUpperImu(), Wire},
                        {device.getMiddleImu(), Wire},
                        {device.getLowerImu(), secondaryBus}) {}

bool SimUflexRuntime::begin() {
    Wire.begin(PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);
    secondaryBus.begin(SECONDARY_SDA_PIN, SECONDARY_SCL_PIN);

    Serial.printf("Primary bus SDA=%u SCL=%u\n", PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);
    Serial.printf("Secondary bus SDA=%u SCL=%u\n", SECONDARY_SDA_PIN, SECONDARY_SCL_PIN);
    Serial.println("Initializing simulated MPU6050 sensors...");

    return simulatedImuArray.begin();
}

bool SimUflexRuntime::update() {
    return simulatedImuArray.update();
}

UflexDevice& SimUflexRuntime::getDevice() {
    return device;
}
