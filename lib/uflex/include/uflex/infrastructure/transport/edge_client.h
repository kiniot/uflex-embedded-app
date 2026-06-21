//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_CLIENT_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#include "uflex/infrastructure/transport/edge_transport.h"
#include "uflex/infrastructure/transport/network/rest_client.h"
#include "uflex/infrastructure/transport/network/wifi_connection.h"

/**
 * @file edge_client.h
 * @brief Declares the EdgeTransport implementation for the uFlex Edge Gateway.
 *
 * EdgeClient owns the WiFi association and translates a MotionPayload into the
 * authenticated data-record request expected by the gateway. It delegates the
 * actual HTTP exchange to RESTClient, so it only knows the gateway-specific
 * contract (path, device id, API key, JSON shape).
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
struct EdgeEndpoint {
    const char* wifiSsid;
    const char* wifiPassword;
    int32_t wifiChannel;
    const char* host;
    uint16_t port;
    const char* path;
    const char* deviceId;
    const char* apiKey;
};

class EdgeClient : public EdgeTransport {
public:
    explicit EdgeClient(const EdgeEndpoint& endpoint);

    bool begin() override;
    bool isReady() const override;
    bool publish(const MotionPayload& payload) override;

private:
    static constexpr size_t REQUEST_BODY_BUFFER_SIZE = 256;

    EdgeEndpoint endpoint;
    WifiConnection wifiConnection;
    RESTClient restClient;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_CLIENT_H
