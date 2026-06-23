//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_SAMPLE_BATCH_SERIALIZER_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_SAMPLE_BATCH_SERIALIZER_H

#include <stddef.h>

#include "uflex/infrastructure/transport/sample_batch_payload.h"

/**
 * @file sample_batch_serializer.h
 * @brief JSON serialization for the enriched embedded -> edge sample batch.
 *
 * Emits the edge's data-records body:
 * {"serial_number":"...","samples":[{"target_angle":..,"proximal_signal":..}]}.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */
class SampleBatchSerializer {
public:
    /**
     * @brief Serializes the batch into the buffer as the edge's data-records JSON.
     *
     * @return The number of characters written, excluding the null terminator,
     *         or -1 when the destination buffer is too small.
     */
    static int toJson(const SampleBatchPayload& payload, char* buffer, size_t bufferSize);
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_SAMPLE_BATCH_SERIALIZER_H
