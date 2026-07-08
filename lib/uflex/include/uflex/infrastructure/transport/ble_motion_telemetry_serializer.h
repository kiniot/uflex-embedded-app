//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_SERIALIZER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_SERIALIZER_H

#include <stddef.h>

#include "ble_motion_telemetry.h"

/**
 * @file ble_motion_telemetry_serializer.h
 * @brief Declares the binary wire format for BLE motion telemetry.
 *
 * BleMotionTelemetrySerializer packs BleMotionTelemetry into a fixed-size,
 * explicit little-endian byte layout instead of relying on raw struct memory
 * layout (which could differ across compilers/alignment rules). Both ESP32
 * (Xtensa) and Android (ARM) are little-endian, so IEEE-754 floats are written
 * byte-for-byte without conversion; only the explicit field order and absence
 * of padding need to match on the mobile side.
 *
 * Wire layout (59 bytes total), all multi-byte fields little-endian:
 *   offset  0: upperMiddleRotation.w  (float32)
 *   offset  4: upperMiddleRotation.x  (float32)
 *   offset  8: upperMiddleRotation.y  (float32)
 *   offset 12: upperMiddleRotation.z  (float32)
 *   offset 16: middleLowerRotation.w  (float32)
 *   offset 20: middleLowerRotation.x  (float32)
 *   offset 24: middleLowerRotation.y  (float32)
 *   offset 28: middleLowerRotation.z  (float32)
 *   offset 32: upperLowerRotation.w   (float32)
 *   offset 36: upperLowerRotation.x   (float32)
 *   offset 40: upperLowerRotation.y   (float32)
 *   offset 44: upperLowerRotation.z   (float32)
 *   offset 48: ledColor               (uint8)
 *   offset 49: buzzerActive           (uint8, 0 or 1)
 *   offset 50: vibrationActive        (uint8, 0 or 1)
 *   offset 51: sequenceNumber         (uint16)
 *   offset 53: jointFlexionDegrees    (float32)
 *   offset 57: isCalibrated           (uint8, 0 or 1)
 *   offset 58: activeJoint            (uint8)
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */
class BleMotionTelemetrySerializer {
public:
    static constexpr size_t WIRE_SIZE_BYTES = 59;

    /**
     * @brief Encodes `payload` into `buffer` using the fixed wire layout.
     *
     * @return The number of bytes written (always WIRE_SIZE_BYTES on success),
     *         or -1 when `bufferSize` is smaller than WIRE_SIZE_BYTES.
     */
    static int toBytes(const BleMotionTelemetry& payload, uint8_t* buffer, size_t bufferSize);
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_BLE_MOTION_TELEMETRY_SERIALIZER_H
