//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_MAPPER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_MAPPER_H

#include "ble_motion_telemetry.h"
#include "uflex/domain/actuators/rgb_led.h"
#include "uflex/domain/devices/motion_state.h"

/**
 * @file ble_motion_telemetry_mapper.h
 * @brief Declares conversion from domain state to BLE telemetry payload.
 *
 * BleMotionTelemetryMapper isolates the mapping from the domain's motion and
 * actuator state to the BLE transport contract. Actuator state is taken as
 * explicit parameters (rather than read from MotionState) because actuator
 * state is not part of the motion snapshot domain concept.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
class BleMotionTelemetryMapper {
public:
    /**
     * @brief Maps the current motion and actuator state to a BLE telemetry payload.
     *
     * @param sequenceNumber Caller-assigned counter (see BleMotionTelemetry); the
     * mapper does not own sequencing, it only places the value into the payload.
     */
    static BleMotionTelemetry map(const MotionState& motionState, RgbLed::Color ledColor,
                                  bool buzzerActive, bool vibrationActive,
                                  uint16_t sequenceNumber);
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_MAPPER_H
