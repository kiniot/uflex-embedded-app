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
    // All three IMUs share address 0x68 (AD0->GND) on one bus, isolated by their TCA9548A channel:
    // upper=bicep=ch1 (proximal, feeds proximal_signal), middle=forearm=ch0, lower=hand=ch2 (dead mag).
    : device({0, 0x68}, {1, 0x68}, {2, 0x68}),
      hardwareImuArray({device.getUpperImu(), Wire, 1, false},
                       {device.getMiddleImu(), Wire, 0, false},
                       {device.getLowerImu(), Wire, 2, false}),
      statusBuzzer(BUZZER_PIN),
      statusLed(RGB_RED_PIN, RGB_GREEN_PIN, RGB_BLUE_PIN),
      edgeClient({UFLEX_WIFI_SSID, UFLEX_WIFI_PASSWORD, UFLEX_WIFI_CHANNEL, UFLEX_EDGE_HOST,
                  UFLEX_EDGE_PORT, UFLEX_EDGE_PATH, UFLEX_EDGE_DOWN_CHANNEL_PATH,
                  UFLEX_SERIAL_NUMBER, UFLEX_DEVICE_API_KEY}),
      bleTelemetryServer({UFLEX_SERIAL_NUMBER, UFLEX_BLE_ADVERTISED_NAME}) {}

bool HwUflexRuntime::begin() {
    statusBuzzer.begin();
    statusLed.begin();

    Wire.begin(PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);

    Serial.printf("I2C bus (to TCA9548A mux) SDA=%u SCL=%u\n", PRIMARY_SDA_PIN, PRIMARY_SCL_PIN);
    Serial.printf("Buzzer test pin=%u\n", BUZZER_PIN);
    Serial.printf("RGB test pins R=%u G=%u B=%u\n", RGB_RED_PIN, RGB_GREEN_PIN, RGB_BLUE_PIN);
    Serial.println("Initializing MPU9250 IMU array...");
    Serial.println("Hardware integration is preliminary and still needs physical validation.");
    applyOutputs();

    // IMUs first: gyro-bias calibration needs the kit still right after boot, and sensing must
    // not wait on a possibly-slow or failed WiFi association (see EXECUTION-CONTRACT §13.4).
    bool imuOk = hardwareImuArray.begin();

    if (!edgeClient.begin()) {
        Serial.println("Edge gateway unreachable; continuing without network publishing.");
    }

    if (!bleTelemetryServer.begin()) {
        Serial.println("BLE telemetry server failed to start.");
    } else {
        // Serial = identity; advertised name = transport; MAC = for registration and
        // diagnostics only, not connection identity (see device-identity-contract).
        Serial.printf("BLE serial=%s advertisedName=%s MAC=%s\n", UFLEX_SERIAL_NUMBER,
                      UFLEX_BLE_ADVERTISED_NAME, bleTelemetryServer.bleMacAddress().c_str());
    }

    return imuOk;
}

bool HwUflexRuntime::update() {
    return hardwareImuArray.update();
}

void HwUflexRuntime::applyOutputs() {
    statusBuzzer.apply(device.getStatusBuzzer());
    statusLed.apply(device.getStatusLed());
}

UflexDevice& HwUflexRuntime::getDevice() {
    return device;
}

EdgeTransport& HwUflexRuntime::getEdgeTransport() {
    return edgeClient;
}

BleTransport& HwUflexRuntime::getBleTransport() {
    return bleTelemetryServer;
}
