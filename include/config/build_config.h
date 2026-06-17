#ifndef UFLEX_CONFIG_BUILD_CONFIG_H
#define UFLEX_CONFIG_BUILD_CONFIG_H

/**
 * @file build_config.h
 * @brief Declares build-target configuration macros for uFlex.
 *
 * This header centralizes the compile-time target selection used across the project so the
 * application can adapt its integrations for simulation or real hardware while keeping a shared
 * core domain model.
 *
 * @author Salim Ramirez
 * @date June 8, 2026
 * @version 0.1.0
 */

#if defined(UFLEX_TARGET_SIM) && defined(UFLEX_TARGET_HW)
#error "UFLEX_TARGET_SIM and UFLEX_TARGET_HW cannot be enabled at the same time."
#endif

#if !defined(UFLEX_TARGET_SIM) && !defined(UFLEX_TARGET_HW)
#error "A uFlex build target must be defined."
#endif

#if defined(UFLEX_TARGET_SIM)
#define UFLEX_BUILD_TARGET_NAME "simulation"

// Wokwi provides a virtual access point with no password. Pinning the channel
// makes the simulated association connect almost instantly.
#define UFLEX_WIFI_SSID "Wokwi-GUEST"
#define UFLEX_WIFI_PASSWORD ""
#define UFLEX_WIFI_CHANNEL 6

// Inside the Wokwi simulation, services running on the host machine (such as the
// edge gateway on localhost) are reachable through this special hostname.
#define UFLEX_EDGE_HOST "host.wokwi.internal"
#endif

#if defined(UFLEX_TARGET_HW)
#define UFLEX_BUILD_TARGET_NAME "hardware"

// Replace with the real deployment WiFi credentials.
#define UFLEX_WIFI_SSID "YOUR_WIFI_SSID"
#define UFLEX_WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define UFLEX_WIFI_CHANNEL 0 // 0 = auto-select channel

// Replace with the edge gateway address on the local network.
#define UFLEX_EDGE_HOST "192.168.1.100"
#endif

// Shared edge gateway contract (see uflex-edge-gateway README).
#define UFLEX_EDGE_PORT 5000
#define UFLEX_EDGE_PATH "/api/v1/movement-monitoring/data-records"
#define UFLEX_DEVICE_ID "uflex-kit-001"
#define UFLEX_DEVICE_API_KEY "test-api-key-123"

#endif // UFLEX_CONFIG_BUILD_CONFIG_H
