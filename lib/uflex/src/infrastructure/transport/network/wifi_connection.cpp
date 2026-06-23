//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

/**
 * @file wifi_connection.cpp
 * @brief Implements the WiFi station connection used by network transports.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */

#include "uflex/infrastructure/transport/network/wifi_connection.h"

#include <Arduino.h>
#include <WiFi.h>

WifiConnection::WifiConnection(const char* ssid, const char* password, int32_t channel)
    : ssid(ssid), password(password), channel(channel) {}

bool WifiConnection::begin(unsigned long timeoutMs) {
    Serial.printf("WiFi: connecting to \"%s\"...\n", ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password, channel);

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

bool WifiConnection::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}
