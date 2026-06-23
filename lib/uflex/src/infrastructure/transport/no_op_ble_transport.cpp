//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

/**
 * @file no_op_ble_transport.cpp
 * @brief Implements the no-op BLE transport used outside esp32_hw.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/no_op_ble_transport.h"

bool NoOpBleTransport::begin() {
    return true;
}

bool NoOpBleTransport::isReady() const {
    return false;
}

bool NoOpBleTransport::publish(const BleMotionTelemetry& payload) {
    (void)payload;
    return false;
}
