//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_H

#include <stdint.h>

#include "uflex/domain/math/quaternion.h"

/**
 * @file ble_motion_telemetry.h
 * @brief Declares the high-frequency BLE telemetry contract for uFlex.
 *
 * BleMotionTelemetry is the payload notified to the paired mobile app over
 * BLE, separate from MotionPayload (which targets the lower-frequency,
 * batched HTTP channel toward the edge gateway). It carries full relative
 * joint rotations (not just pitch/roll) so the mobile app can drive a 3-axis
 * avatar projection, plus the locally decided actuator feedback state so the
 * app can mirror it without recomputing any clinical thresholds itself.
 *
 * sessionActive is intentionally not included yet: the embedded does not have
 * a session/configuration concept to report until the edge -> embedded
 * session contract described in docs/actuator-activation-flow.md is
 * implemented. Add it here once that lands.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
struct BleMotionTelemetry {
    Quaternion upperMiddleRotation;
    Quaternion middleLowerRotation;
    Quaternion upperLowerRotation;
    uint8_t ledColor;
    bool buzzerActive;
    bool vibrationActive;
    uint16_t sequenceNumber;
    // Calibrated, gravity-anchored flexion angle of the active joint (degrees). This is the same
    // value the safety path and edge use; the app shows it directly so its gauge matches the
    // active exercise and reads 0 at the calibrated reference pose (no yaw drift).
    float jointFlexionDegrees;
    // Whether a session zero has been captured; the app shows "—" until true.
    bool isCalibrated;
    // Active joint the flexion refers to (ActiveJoint enum: 0=None, 1=Elbow, 2=Wrist).
    uint8_t activeJoint;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_H
