//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file rest_client.cpp
 * @brief Implements the contract-agnostic REST client.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/network/rest_client.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <string.h>

namespace {
constexpr unsigned long REQUEST_TIMEOUT_MS = 1500;
}

RESTClient::RESTClient(WifiConnection& wifiConnection, const char* host, uint16_t port)
    : wifiConnection(wifiConnection), host(host), port(port) {}

RestResponse RESTClient::post(const char* path, const char* jsonBody, const char* apiKey) {
    if (!wifiConnection.isConnected()) {
        return {false, 0};
    }

    char url[128] = {};
    snprintf(url, sizeof(url), "http://%s:%u%s", host, port, path);

    HTTPClient http;
    http.setTimeout(REQUEST_TIMEOUT_MS);
    if (!http.begin(url)) {
        return {false, 0};
    }

    http.addHeader("Content-Type", "application/json");
    if (apiKey != nullptr) {
        http.addHeader("X-API-Key", apiKey);
    }

    const int statusCode =
        http.POST(reinterpret_cast<uint8_t*>(const_cast<char*>(jsonBody)), strlen(jsonBody));
    http.end();

    return {statusCode > 0, statusCode};
}
