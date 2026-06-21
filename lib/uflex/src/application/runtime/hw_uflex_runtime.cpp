//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#include "uflex/application/runtime/hw_uflex_runtime.h"

#include <Arduino.h>

#include "config/build_config.h"

/**
 * @file hw_uflex_runtime.cpp
 * @brief Implements the hardware runtime for uFlex.
 *
 * This runtime prepares the buses intended for the physical-device target and
 * delegates sensor initialization and updates to the MPU9250 hardware adapter.
 * The structure is ready for integration while final behavior still depends on
 * validation with the real board and wiring.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

HwUflexRuntime::HwUflexRuntime()
    : device({0, 0x68}, {1, 0x69}, {2, 0x68}),
      secondaryBus(1),
      hardwareImuArray({device.getUpperImu(), Wire},
                       {device.getMiddleImu(), Wire},
                       {device.getLowerImu(), secondaryBus}),
      statusBuzzer(BUZZER_PIN),
      statusLed(RGB_RED_PIN, RGB_GREEN_PIN, RGB_BLUE_PIN),
      vibrationMotor(VIBRATION_MOTOR_PIN),
      edgeClient({UFLEX_WIFI_SSID, UFLEX_WIFI_PASSWORD, UFLEX_WIFI_CHANNEL, UFLEX_EDGE_HOST,
                  UFLEX_EDGE_PORT, UFLEX_EDGE_PATH, UFLEX_DEVICE_ID, UFLEX_DEVICE_API_KEY}) {}

bool HwUflexRuntime::begin() {
    statusBuzzer.begin();
    statusLed.begin();
    vibrationMotor.begin();

    Wire.begin(PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);
    secondaryBus.begin(SECONDARY_SDA_PIN, SECONDARY_SCL_PIN);

    Serial.printf("Primary bus SDA=%u SCL=%u\n", PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);
    Serial.printf("Secondary bus SDA=%u SCL=%u\n", SECONDARY_SDA_PIN, SECONDARY_SCL_PIN);
    Serial.printf("Buzzer test pin=%u\n", BUZZER_PIN);
    Serial.printf("Vibration motor test pin=%u\n", VIBRATION_MOTOR_PIN);
    Serial.printf("RGB test pins R=%u G=%u B=%u\n", RGB_RED_PIN, RGB_GREEN_PIN, RGB_BLUE_PIN);
    Serial.println("Initializing MPU9250 IMU array...");
    Serial.println("Hardware integration is preliminary and still needs physical validation.");
    applyOutputs();

    if (!edgeClient.begin()) {
        Serial.println("Edge gateway unreachable; continuing without network publishing.");
    }

    return hardwareImuArray.begin();
}

bool HwUflexRuntime::update() {
    return hardwareImuArray.update();
}

void HwUflexRuntime::applyOutputs() {
    statusBuzzer.apply(device.getStatusBuzzer());
    statusLed.apply(device.getStatusLed());
    vibrationMotor.apply(device.getVibrationMotor());
}

UflexDevice& HwUflexRuntime::getDevice() {
    return device;
}

EdgeTransport& HwUflexRuntime::getEdgeTransport() {
    return edgeClient;
}
