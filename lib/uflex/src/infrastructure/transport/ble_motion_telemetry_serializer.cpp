//
// Created by Salim Ramirez Mestanza on 21/06/26.
//

/**
 * @file ble_motion_telemetry_serializer.cpp
 * @brief Implements the binary wire format for BLE motion telemetry.
 *
 * @author Salim Ramirez
 * @date June 21, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/ble_motion_telemetry_serializer.h"

#include <string.h>

namespace {

size_t writeFloat(uint8_t* buffer, size_t offset, float value) {
    memcpy(buffer + offset, &value, sizeof(float));
    return offset + sizeof(float);
}

size_t writeQuaternion(uint8_t* buffer, size_t offset, const Quaternion& quaternion) {
    offset = writeFloat(buffer, offset, quaternion.w);
    offset = writeFloat(buffer, offset, quaternion.x);
    offset = writeFloat(buffer, offset, quaternion.y);
    offset = writeFloat(buffer, offset, quaternion.z);
    return offset;
}

size_t writeUint8(uint8_t* buffer, size_t offset, uint8_t value) {
    buffer[offset] = value;
    return offset + 1;
}

size_t writeUint16LittleEndian(uint8_t* buffer, size_t offset, uint16_t value) {
    buffer[offset] = static_cast<uint8_t>(value & 0xFF);
    buffer[offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    return offset + 2;
}

} // namespace

int BleMotionTelemetrySerializer::toBytes(const BleMotionTelemetry& payload, uint8_t* buffer,
                                          size_t bufferSize) {
    if (bufferSize < WIRE_SIZE_BYTES) {
        return -1;
    }

    size_t offset = 0;
    offset = writeQuaternion(buffer, offset, payload.upperMiddleRotation);
    offset = writeQuaternion(buffer, offset, payload.middleLowerRotation);
    offset = writeQuaternion(buffer, offset, payload.upperLowerRotation);
    offset = writeUint8(buffer, offset, payload.ledColor);
    offset = writeUint8(buffer, offset, payload.buzzerActive ? 1 : 0);
    offset = writeUint8(buffer, offset, payload.vibrationActive ? 1 : 0);
    offset = writeUint16LittleEndian(buffer, offset, payload.sequenceNumber);

    return static_cast<int>(offset);
}
