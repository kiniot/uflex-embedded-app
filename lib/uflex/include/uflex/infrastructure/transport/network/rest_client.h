//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_NETWORK_REST_CLIENT_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_NETWORK_REST_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#include "uflex/infrastructure/transport/network/wifi_connection.h"

/**
 * @file rest_client.h
 * @brief Declares a small, contract-agnostic REST client.
 *
 * RESTClient only knows how to perform HTTP requests against a host/port over
 * a WifiConnection. It does not know about uFlex domain types or the edge
 * gateway contract, so it can be reused for any current or future endpoint
 * (data records today, configuration or session commands later).
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
struct RestResponse {
    bool requestSucceeded;
    int statusCode;
};

class RESTClient {
public:
    RESTClient(WifiConnection& wifiConnection, const char* host, uint16_t port);

    /**
     * @brief Sends a POST request with a JSON body and an optional API key header.
     *
     * @param path Request path, e.g. "/api/v1/movement-monitoring/data-records".
     * @param jsonBody Request body, already serialized as JSON.
     * @param apiKey Optional value sent in the "X-API-Key" header; pass nullptr to omit it.
     */
    RestResponse post(const char* path, const char* jsonBody, const char* apiKey = nullptr);

    /**
     * @brief Sends a GET request and copies the response body into the buffer.
     *
     * @param path Request path.
     * @param responseBuffer Caller-owned buffer for the response body (null-terminated on return).
     * @param responseBufferSize Size of responseBuffer in bytes.
     * @param apiKey Optional value sent in the "X-API-Key" header; pass nullptr to omit it.
     */
    RestResponse get(const char* path, char* responseBuffer, size_t responseBufferSize,
                     const char* apiKey = nullptr);

private:
    WifiConnection& wifiConnection;
    const char* host;
    uint16_t port;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_NETWORK_REST_CLIENT_H
