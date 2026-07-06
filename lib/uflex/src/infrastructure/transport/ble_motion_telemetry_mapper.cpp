//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

/**
 * @file ble_motion_telemetry_mapper.cpp
 * @brief Implements conversion from domain state to BLE telemetry payload.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/ble_motion_telemetry_mapper.h"

BleMotionTelemetry BleMotionTelemetryMapper::map(const MotionState& motionState,
                                                 RgbLed::Color ledColor, bool buzzerActive,
                                                 bool vibrationActive, uint16_t sequenceNumber,
                                                 float jointFlexionDegrees, bool isCalibrated,
                                                 uint8_t activeJoint) {
    return BleMotionTelemetry{
        motionState.upperMiddleRotation,
        motionState.middleLowerRotation,
        motionState.upperLowerRotation,
        static_cast<uint8_t>(ledColor),
        buzzerActive,
        vibrationActive,
        sequenceNumber,
        jointFlexionDegrees,
        isCalibrated,
        activeJoint,
    };
}
