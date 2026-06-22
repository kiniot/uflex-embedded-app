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

// Fixed test key for the local edge gateway used during simulation; not a real secret.
#define UFLEX_DEVICE_API_KEY "test-api-key-123"
#endif

#if defined(UFLEX_TARGET_HW)
#define UFLEX_BUILD_TARGET_NAME "hardware"
#define UFLEX_WIFI_CHANNEL 0 // 0 = auto-select channel

// UFLEX_WIFI_SSID, UFLEX_EDGE_HOST, and UFLEX_DEVICE_API_KEY are injected as
// build flags from real deployment secrets (see platformio.ini env:esp32_hw
// and scripts/build_hw.sh). They are intentionally not hardcoded here so they
// never end up committed to source control. UFLEX_WIFI_PASSWORD may
// legitimately be empty for an open network, so it is not checked below.
#if !defined(UFLEX_WIFI_SSID) || !defined(UFLEX_WIFI_PASSWORD) || !defined(UFLEX_EDGE_HOST) || \
    !defined(UFLEX_DEVICE_API_KEY)
#error \
    "esp32_hw requires UFLEX_WIFI_SSID, UFLEX_WIFI_PASSWORD, UFLEX_EDGE_HOST, and " \
    "UFLEX_DEVICE_API_KEY as build flags. Copy .env.example to .env and build with " \
    "scripts/build_hw.sh."
#endif

#if defined(__cplusplus)
// An unset build flag still defines the macro as an empty string, which the
// preprocessor #error checks above cannot detect. sizeof() of a string
// literal can: an empty string literal has sizeof() == 1 (just the null
// terminator), so this catches the unset-but-empty case at compile time.
static_assert(sizeof(UFLEX_WIFI_SSID) > 1,
              "UFLEX_WIFI_SSID is empty. Fill in .env and build with scripts/build_hw.sh.");
static_assert(sizeof(UFLEX_EDGE_HOST) > 1,
              "UFLEX_EDGE_HOST is empty. Fill in .env and build with scripts/build_hw.sh.");
static_assert(
    sizeof(UFLEX_DEVICE_API_KEY) > 1,
    "UFLEX_DEVICE_API_KEY is empty. Fill in .env and build with scripts/build_hw.sh.");
#endif
#endif

// Shared edge gateway contract (see uflex-edge-gateway README).
#define UFLEX_EDGE_PORT 5000
#define UFLEX_EDGE_PATH "/api/v1/movement-monitoring/data-records"
// Down-channel: the firmware GETs its active serie context (joint + maxSafeAngle)
// here; the kit serial is appended as a query param at call time.
#define UFLEX_EDGE_DOWN_CHANNEL_PATH "/api/v1/movement-monitoring/active-context"

// kitSerial: the device's cross-service identity (== edge device_id, == backend
// serialNumber, == the value of the BLE serial characteristic). It is NOT the
// backend DeviceId (UUID). See docs/device-identity-contract.md.
#define UFLEX_SERIAL_NUMBER "uflex-kit-001"

// BLE advertised name (transport only). May legitimately differ from the serial
// (e.g. a shorter name to fit the advertisement); discovery is by service UUID,
// not by name. Kept equal to the serial for now.
#define UFLEX_BLE_ADVERTISED_NAME "uflex-kit-001"

#endif // UFLEX_CONFIG_BUILD_CONFIG_H
