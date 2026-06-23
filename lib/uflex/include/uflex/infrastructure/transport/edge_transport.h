//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_TRANSPORT_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_TRANSPORT_H

#include "uflex/infrastructure/transport/active_serie_context.h"
#include "uflex/infrastructure/transport/motion_payload.h"
#include "uflex/infrastructure/transport/sample_batch_payload.h"

/**
 * @file edge_transport.h
 * @brief Declares the application-facing contract for publishing to the edge.
 *
 * EdgeTransport lets the application layer request publication of a
 * MotionPayload without depending on WiFi, HTTP, or the specific edge gateway
 * contract used by the concrete implementation.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
class EdgeTransport {
public:
    virtual bool begin() = 0;
    virtual bool isReady() const = 0;
    virtual bool publish(const MotionPayload& payload) = 0;
    virtual bool publishSamples(const SampleBatchPayload& payload) = 0;
    virtual bool fetchActiveContext(ActiveSerieContext& out) = 0;
    virtual ~EdgeTransport() = default;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_TRANSPORT_H
