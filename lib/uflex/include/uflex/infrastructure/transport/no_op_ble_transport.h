//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_NO_OP_BLE_TRANSPORT_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_NO_OP_BLE_TRANSPORT_H

#include "uflex/infrastructure/transport/ble_transport.h"

/**
 * @file no_op_ble_transport.h
 * @brief Declares a BleTransport stand-in for targets without a real BLE stack.
 *
 * NoOpBleTransport lets SimUflexRuntime satisfy the UflexRuntime contract
 * without linking a BLE stack: Wokwi does not simulate Bluetooth, so there is
 * nothing real to drive in the esp32_sim target. It always reports as not
 * ready and never sends anything.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
class NoOpBleTransport : public BleTransport {
public:
    bool begin() override;
    bool isReady() const override;
    bool publish(const BleMotionTelemetry& payload) override;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_NO_OP_BLE_TRANSPORT_H
