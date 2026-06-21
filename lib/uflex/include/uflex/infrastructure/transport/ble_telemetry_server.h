//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TELEMETRY_SERVER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TELEMETRY_SERVER_H

#include <NimBLEServer.h>

#include <string>

#include "uflex/infrastructure/transport/ble_transport.h"

/**
 * @file ble_telemetry_server.h
 * @brief Declares the NimBLE-backed BLE telemetry transport for uFlex.
 *
 * BleTelemetryServer advertises a single custom GATT service with two
 * characteristics: a NOTIFY telemetry characteristic that streams each
 * BleMotionTelemetry frame to the connected mobile app, and a READ identity
 * characteristic exposing the kit serial (== advertised name) so the app can
 * confirm it connected to the device it was assigned. The serial is also set
 * as the advertised name so the app can discover the kit by name (the only
 * reliable selector on iOS, where the MAC is hidden). See
 * docs/device-identity-contract.md. It is the esp32_hw-only counterpart to
 * NoOpBleTransport.
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

    /**
     * @brief Returns the resolved BLE MAC address as a string.
     *
     * Only valid after begin() has initialized the BLE stack. Intended for
     * provisioning/registration and boot diagnostics, not as a connection
     * identity (see docs/device-identity-contract.md).
     */
    std::string bleMacAddress() const;

private:
    static constexpr const char* SERVICE_UUID = "a1f7b2c0-3e4d-4a5b-8c6d-1f2e3a4b5c6d";
    static constexpr const char* TELEMETRY_CHARACTERISTIC_UUID =
        "a1f7b2c1-3e4d-4a5b-8c6d-1f2e3a4b5c6d";
    static constexpr const char* SERIAL_CHARACTERISTIC_UUID =
        "a1f7b2c2-3e4d-4a5b-8c6d-1f2e3a4b5c6d";
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
