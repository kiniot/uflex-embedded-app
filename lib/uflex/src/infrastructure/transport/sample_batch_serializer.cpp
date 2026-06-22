//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

/**
 * @file sample_batch_serializer.cpp
 * @brief Implements JSON serialization for the enriched sample batch.
 *
 * Built incrementally with bounds-checked snprintf calls so the layout is exact
 * regardless of batch size and never overruns the caller's fixed buffer.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/sample_batch_serializer.h"

#include <stdio.h>

int SampleBatchSerializer::toJson(const SampleBatchPayload& payload, char* buffer, size_t bufferSize) {
    int written = snprintf(buffer, bufferSize, "{\"serial_number\":\"%s\",\"samples\":[",
                           payload.serialNumber);
    if (written < 0 || static_cast<size_t>(written) >= bufferSize) {
        return -1;
    }
    int offset = written;

    for (size_t i = 0; i < payload.sampleCount; ++i) {
        const JointSample& sample = payload.samples[i];
        written = snprintf(buffer + offset, bufferSize - static_cast<size_t>(offset),
                           "%s{\"target_angle\":%.2f,\"proximal_signal\":%.2f}",
                           (i == 0 ? "" : ","),
                           sample.targetAngleDegrees, sample.proximalSignalDegrees);
        if (written < 0 || static_cast<size_t>(offset) + static_cast<size_t>(written) >= bufferSize) {
            return -1;
        }
        offset += written;
    }

    written = snprintf(buffer + offset, bufferSize - static_cast<size_t>(offset), "]}");
    if (written < 0 || static_cast<size_t>(offset) + static_cast<size_t>(written) >= bufferSize) {
        return -1;
    }
    return offset + written;
}
