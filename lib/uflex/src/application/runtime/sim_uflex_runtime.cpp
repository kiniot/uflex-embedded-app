//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/application/runtime/sim_uflex_runtime.h"

#include <Arduino.h>

#include "config/build_config.h"

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
                        {device.getLowerImu(), secondaryBus}),
      statusBuzzer(BUZZER_PIN),
      statusLed(RGB_RED_PIN, RGB_GREEN_PIN, RGB_BLUE_PIN),
      vibrationMotor(VIBRATION_MOTOR_PIN),
      edgeClient({UFLEX_WIFI_SSID, UFLEX_WIFI_PASSWORD, UFLEX_WIFI_CHANNEL, UFLEX_EDGE_HOST,
                  UFLEX_EDGE_PORT, UFLEX_EDGE_PATH, UFLEX_SERIAL_NUMBER, UFLEX_DEVICE_API_KEY}) {}

bool SimUflexRuntime::begin() {
    statusBuzzer.begin();
    statusLed.begin();
    vibrationMotor.begin();
    Wire.begin(PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);
    secondaryBus.begin(SECONDARY_SDA_PIN, SECONDARY_SCL_PIN);

    Serial.printf("Primary bus SDA=%u SCL=%u\n", PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);
    Serial.printf("Secondary bus SDA=%u SCL=%u\n", SECONDARY_SDA_PIN, SECONDARY_SCL_PIN);
    Serial.printf("Buzzer status pin=%u\n", BUZZER_PIN);
    Serial.printf("Vibration motor status pin=%u\n", VIBRATION_MOTOR_PIN);
    Serial.printf("RGB status LED pins R=%u G=%u B=%u\n", RGB_RED_PIN, RGB_GREEN_PIN,
                  RGB_BLUE_PIN);
    Serial.println("Initializing simulated MPU6050 sensors...");

    applyOutputs();

    if (!edgeClient.begin()) {
        Serial.println("Edge gateway unreachable; continuing without network publishing.");
    }

    return simulatedImuArray.begin();
}

bool SimUflexRuntime::update() {
    return simulatedImuArray.update();
}

void SimUflexRuntime::applyOutputs() {
    statusBuzzer.apply(device.getStatusBuzzer());
    statusLed.apply(device.getStatusLed());
    vibrationMotor.apply(device.getVibrationMotor());
}

UflexDevice& SimUflexRuntime::getDevice() {
    return device;
}

EdgeTransport& SimUflexRuntime::getEdgeTransport() {
    return edgeClient;
}

BleTransport& SimUflexRuntime::getBleTransport() {
    return bleTransport;
}
