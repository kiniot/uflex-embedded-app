//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

/**
 * @file ble_telemetry_server.cpp
 * @brief Implements the NimBLE-backed BLE telemetry transport for uFlex.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/ble_telemetry_server.h"

#include <NimBLEDevice.h>

#include "uflex/infrastructure/transport/ble_motion_telemetry_serializer.h"

void BleTelemetryServer::ConnectionTracker::onConnect(NimBLEServer* server,
                                                       NimBLEConnInfo& connInfo) {
    (void)connInfo;
    ++connectedCount;
    // Keep advertising so a second mobile client (or a reconnecting one) can still find us.
    server->startAdvertising();
}

void BleTelemetryServer::ConnectionTracker::onDisconnect(NimBLEServer* server,
                                                         NimBLEConnInfo& connInfo, int reason) {
    (void)connInfo;
    (void)reason;
    if (connectedCount > 0) {
        --connectedCount;
    }
    server->startAdvertising();
}

bool BleTelemetryServer::ConnectionTracker::hasConnectedCentral() const {
    return connectedCount > 0;
}

BleTelemetryServer::BleTelemetryServer(Config config) : config(config) {}

bool BleTelemetryServer::begin() {
    NimBLEDevice::init(config.deviceName);
    NimBLEDevice::setMTU(PREFERRED_MTU);

    server = NimBLEDevice::createServer();
    server->setCallbacks(&connectionTracker);

    NimBLEService* telemetryService = server->createService(SERVICE_UUID);
    telemetryCharacteristic = telemetryService->createCharacteristic(
        TELEMETRY_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::NOTIFY);
    server->start();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->start();

    return true;
}

bool BleTelemetryServer::isReady() const {
    return connectionTracker.hasConnectedCentral();
}

bool BleTelemetryServer::publish(const BleMotionTelemetry& payload) {
    if (!isReady()) {
        return false;
    }

    uint8_t buffer[BleMotionTelemetrySerializer::WIRE_SIZE_BYTES];
    const int written = BleMotionTelemetrySerializer::toBytes(payload, buffer, sizeof(buffer));
    if (written < 0) {
        return false;
    }

    telemetryCharacteristic->setValue(buffer, static_cast<size_t>(written));
    return telemetryCharacteristic->notify();
}
