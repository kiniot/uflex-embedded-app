//
// Created by Salim Ramirez Mestanza on 8/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_SERIALIZER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_SERIALIZER_H

#include <stddef.h>

#include "motion_payload.h"

/**
 * @file motion_payload_serializer.h
 * @brief Declares JSON serialization for the motion transport payload.
 *
 * MotionPayloadSerializer converts the transport-level motion payload into a
 * compact JSON representation suitable for serial diagnostics and future
 * network-oriented integrations.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class MotionPayloadSerializer {
public:
    /**
     * @brief Serializes the payload into the provided buffer as JSON.
     *
     * @return The number of characters written, excluding the null terminator,
     *         or -1 when the destination buffer is too small.
     */
    static int toJson(const MotionPayload& payload, char* buffer, size_t bufferSize);
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_SERIALIZER_H
