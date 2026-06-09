//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

/**
 * @file motion_payload_serializer.cpp
 * @brief Implements JSON serialization for the motion transport payload.
 *
 * The serializer produces a compact textual representation of the derived
 * motion angles so they can be logged, debugged, or forwarded through future
 * communication channels without depending on domain internals.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/motion_payload_serializer.h"

#include <stdio.h>

int MotionPayloadSerializer::toJson(const MotionPayload& payload, char* buffer,
                                         size_t bufferSize) {
    const int written = snprintf(
        buffer,
        bufferSize,
        "{\"upperMiddle\":{\"pitchDeg\":%.2f,\"rollDeg\":%.2f},"
        "\"middleLower\":{\"pitchDeg\":%.2f,\"rollDeg\":%.2f},"
        "\"upperLower\":{\"pitchDeg\":%.2f,\"rollDeg\":%.2f}}",
        payload.upperMiddleAngle.pitchDegrees,
        payload.upperMiddleAngle.rollDegrees,
        payload.middleLowerAngle.pitchDegrees,
        payload.middleLowerAngle.rollDegrees,
        payload.upperLowerAngle.pitchDegrees,
        payload.upperLowerAngle.rollDegrees
    );

    if (written < 0 || static_cast<size_t>(written) >= bufferSize) {
        return -1;
    }

    return written;
}
