//
// Created by Salim Ramirez Mestanza on 16/06/26.
//

#ifndef UFLEX_INFRASTRUCTURE_TRANSPORT_NETWORK_WIFI_CONNECTION_H
#define UFLEX_INFRASTRUCTURE_TRANSPORT_NETWORK_WIFI_CONNECTION_H

#include <stdint.h>

/**
 * @file wifi_connection.h
 * @brief Declares the WiFi station connection used by network transports.
 *
 * WifiConnection owns the association lifecycle to a single access point. It
 * stays deliberately small: it only knows how to join a network and report
 * its current status, so higher-level clients (such as RESTClient) can depend
 * on connectivity without knowing how it is established.
 *
 * @author Salim Ramirez
 * @date June 16, 2026
 * @version 0.1.0
 */
class WifiConnection {
public:
    WifiConnection(const char* ssid, const char* password, int32_t channel);

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

private:
    const char* ssid;
    const char* password;
    int32_t channel;
};

#endif // UFLEX_INFRASTRUCTURE_TRANSPORT_NETWORK_WIFI_CONNECTION_H
