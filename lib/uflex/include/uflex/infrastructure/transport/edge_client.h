//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_CLIENT_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_CLIENT_H

#include <stdint.h>

/**
 * @file edge_client.h
 * @brief Declares the WiFi/HTTP transport that forwards angles to the edge gateway.
 *
 * EdgeClient connects the ESP32 (real or Wokwi-simulated) to a WiFi network and
 * publishes a single joint-flexion angle to the uFlex Edge Gateway using its
 * authenticated data-records endpoint. It deliberately stays thin: it owns the
 * connection lifecycle and the request formatting, but not the motion model.
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

class EdgeClient {
public:
    explicit EdgeClient(const EdgeEndpoint& endpoint);

    /**
     * @brief Joins the configured WiFi network.
     *
     * @param timeoutMs Maximum time to wait for an association before giving up.
     * @return true when the station is connected, false on timeout.
     */
    bool begin(unsigned long timeoutMs = 20000);

    /**
     * @brief Reports whether the WiFi station is currently connected.
     */
    bool isConnected() const;

    /**
     * @brief Sends one flexion angle to the edge gateway as a data record.
     *
     * @param angleDegrees Joint flexion angle, in degrees.
     * @return true when the gateway accepts the record (HTTP 200/201).
     */
    bool publishAngle(float angleDegrees);

private:
    EdgeEndpoint endpoint;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_EDGE_CLIENT_H
