#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_SERIALIZER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_SERIALIZER_H

#include <stddef.h>

#include "motion_payload.h"

/**
 * @file motion_payload_serializer.h
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */
class MotionPayloadSerializer {
public:
    static int toJson(const MotionPayload& payload, char* buffer, size_t bufferSize);
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_MOTION_PAYLOAD_SERIALIZER_H
