//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file edge_client.cpp
 * @brief Implements the WiFi/HTTP transport to the uFlex Edge Gateway.
 *
 * The client manages the WiFi association and posts each flexion angle as a
 * JSON data record authenticated with the device API key, matching the gateway
 * contract documented in the uflex-edge-gateway repository.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/edge_client.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

EdgeClient::EdgeClient(const EdgeEndpoint& endpoint) : endpoint(endpoint) {}

bool EdgeClient::begin(unsigned long timeoutMs) {
    Serial.printf("WiFi: connecting to \"%s\"...\n", endpoint.wifiSsid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(endpoint.wifiSsid, endpoint.wifiPassword, endpoint.wifiChannel);

    const unsigned long startedAt = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startedAt >= timeoutMs) {
            Serial.println("WiFi: connection timed out");
            return false;
        }
        delay(250);
        Serial.print('.');
    }

    Serial.printf("\nWiFi: connected, IP=%s\n", WiFi.localIP().toString().c_str());
    return true;
}

bool EdgeClient::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

bool EdgeClient::publishAngle(float angleDegrees) {
    if (!isConnected()) {
        Serial.println("edge: skipped publish (WiFi not connected)");
        return false;
    }

    char url[128] = {};
    snprintf(url, sizeof(url), "http://%s:%u%s", endpoint.host, endpoint.port, endpoint.path);

    char body[128] = {};
    snprintf(body, sizeof(body), "{\"device_id\":\"%s\",\"angle\":%.2f}", endpoint.deviceId,
             angleDegrees);

    HTTPClient http;
    if (!http.begin(url)) {
        Serial.println("edge: failed to initialize HTTP request");
        return false;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-Key", endpoint.apiKey);

    const int statusCode = http.POST(reinterpret_cast<uint8_t*>(body), strlen(body));
    const bool accepted = statusCode == HTTP_CODE_OK || statusCode == HTTP_CODE_CREATED;

    if (accepted) {
        Serial.printf("edge: angle=%.2f published (HTTP %d)\n", angleDegrees, statusCode);
    } else {
        Serial.printf("edge: publish failed (HTTP %d): %s\n", statusCode,
                      http.getString().c_str());
    }

    http.end();
    return accepted;
}
