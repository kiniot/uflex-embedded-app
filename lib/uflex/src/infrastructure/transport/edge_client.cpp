//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file edge_client.cpp
 * @brief Implements the EdgeTransport for the uFlex Edge Gateway.
 *
 * EdgeClient reuses MotionPayloadSerializer to build the motion portion of the
 * request body, then wraps it with the device identity expected by the
 * gateway's authenticated data-records endpoint.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/edge_client.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <stdio.h>

#include "uflex/infrastructure/transport/motion_payload_serializer.h"

EdgeClient::EdgeClient(const EdgeEndpoint& endpoint)
    : endpoint(endpoint),
      wifiConnection(endpoint.wifiSsid, endpoint.wifiPassword, endpoint.wifiChannel),
      restClient(wifiConnection, endpoint.host, endpoint.port) {}

bool EdgeClient::begin() {
    return wifiConnection.begin();
}

bool EdgeClient::isReady() const {
    return wifiConnection.isConnected();
}

bool EdgeClient::publish(const MotionPayload& payload) {
    if (!isReady()) {
        Serial.println("edge: skipped publish (WiFi not connected)");
        return false;
    }

    char motionJson[REQUEST_BODY_BUFFER_SIZE] = {};
    if (MotionPayloadSerializer::toJson(payload, motionJson, sizeof(motionJson)) < 0) {
        Serial.println("edge: skipped publish (payload serialization failed)");
        return false;
    }

    char body[REQUEST_BODY_BUFFER_SIZE] = {};
    const int written = snprintf(body, sizeof(body), "{\"device_id\":\"%s\",\"motion\":%s}",
                                  endpoint.deviceId, motionJson);
    if (written < 0 || static_cast<size_t>(written) >= sizeof(body)) {
        Serial.println("edge: skipped publish (request body too large)");
        return false;
    }

    const RestResponse response = restClient.post(endpoint.path, body, endpoint.apiKey);
    const bool accepted =
        response.statusCode == HTTP_CODE_OK || response.statusCode == HTTP_CODE_CREATED;

    if (accepted) {
        Serial.printf("edge: payload published (HTTP %d)\n", response.statusCode);
    } else {
        Serial.printf("edge: publish failed (HTTP %d)\n", response.statusCode);
    }

    return accepted;
}
