//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TELEMETRY_SERVER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TELEMETRY_SERVER_H

#include <NimBLEServer.h>

#include "uflex/infrastructure/transport/ble_transport.h"

/**
 * @file ble_telemetry_server.h
 * @brief Declares the NimBLE-backed BLE telemetry transport for uFlex.
 *
 * BleTelemetryServer advertises a single custom GATT service with one notify
 * characteristic, and publishes each BleMotionTelemetry frame to whichever
 * mobile app is currently connected and subscribed. It is the esp32_hw-only
 * counterpart to NoOpBleTransport.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
class BleTelemetryServer : public BleTransport {
public:
    /**
     * @brief Describes the advertising identity for this peripheral.
     */
    struct Config {
        const char* deviceName;
    };

    explicit BleTelemetryServer(Config config);

    bool begin() override;
    bool isReady() const override;
    bool publish(const BleMotionTelemetry& payload) override;

private:
    static constexpr const char* SERVICE_UUID = "a1f7b2c0-3e4d-4a5b-8c6d-1f2e3a4b5c6d";
    static constexpr const char* TELEMETRY_CHARACTERISTIC_UUID =
        "a1f7b2c1-3e4d-4a5b-8c6d-1f2e3a4b5c6d";
    static constexpr uint16_t PREFERRED_MTU = 256;

    class ConnectionTracker : public NimBLEServerCallbacks {
    public:
        void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) override;
        void onDisconnect(NimBLEServer* server, NimBLEConnInfo& connInfo, int reason) override;

        bool hasConnectedCentral() const;

    private:
        uint16_t connectedCount = 0;
    };

    Config config;
    ConnectionTracker connectionTracker;
    NimBLEServer* server = nullptr;
    NimBLECharacteristic* telemetryCharacteristic = nullptr;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TELEMETRY_SERVER_H
