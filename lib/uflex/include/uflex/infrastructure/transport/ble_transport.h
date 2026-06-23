//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TRANSPORT_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TRANSPORT_H

#include "uflex/infrastructure/transport/ble_motion_telemetry.h"

/**
 * @file ble_transport.h
 * @brief Declares the application-facing contract for BLE telemetry publishing.
 *
 * BleTransport mirrors EdgeTransport's role for the BLE channel: it lets the
 * application layer request publication of a BleMotionTelemetry frame without
 * depending on the concrete BLE stack used by the implementation. This keeps
 * SimUflexRuntime and HwUflexRuntime symmetric even though only the hardware
 * target backs it with a real BLE stack (see NoOpBleTransport, BleTelemetryServer).
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
class BleTransport {
public:
    virtual bool begin() = 0;
    virtual bool isReady() const = 0;
    virtual bool publish(const BleMotionTelemetry& payload) = 0;
    virtual ~BleTransport() = default;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_TRANSPORT_H
