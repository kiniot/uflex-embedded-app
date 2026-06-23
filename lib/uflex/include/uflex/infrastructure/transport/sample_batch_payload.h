//
// Created by Salim Ramirez Mestanza on 22/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_SAMPLE_BATCH_PAYLOAD_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_SAMPLE_BATCH_PAYLOAD_H

#include <stddef.h>

/**
 * @file sample_batch_payload.h
 * @brief Transport payload for the enriched embedded -> edge movement contract.
 *
 * Each JointSample carries the active joint's absolute flexion angle (the value
 * the edge detector consumes) and the proximal-segment signal (populated now,
 * consumed by Wave 2 compensation detection). A SampleBatchPayload groups one or
 * more samples for a kit; the firmware starts with batches of size 1 (~10Hz) and
 * can grow the batch with no edge change, since the edge already iterates the array.
 *
 * @author Salim Ramirez
 * @date June 22, 2026
 * @version 0.1.0
 */
struct JointSample {
    float targetAngleDegrees;
    float proximalSignalDegrees;
};

struct SampleBatchPayload {
    const char* serialNumber;
    const JointSample* samples;
    size_t sampleCount;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_SAMPLE_BATCH_PAYLOAD_H
