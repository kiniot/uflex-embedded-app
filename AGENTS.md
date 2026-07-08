# AGENTS.md

## Big Picture
- This is an ESP32/PlatformIO firmware project for **uFlex**, a telerehabilitation device that reads **3 IMUs**, fuses orientation, computes relative joint motion, applies local safety feedback, and publishes movement data through BLE and an edge gateway.
- The main code lives in `lib/uflex/` and is intentionally split into:
  - `domain/`: problem model and rules (`Imu`, `UflexDevice`, `MotionState`, actuators, quaternions, orientation/angle/rotation services)
  - `application/`: runtime orchestration and main loop policy (`UflexApplication`, `createUflexRuntime`, joint targeting, settle detection, safety monitoring)
  - `infrastructure/`: simulation/hardware adapters, GPIO outputs, BLE, edge transport, Wi-Fi/REST, payload mapping/parsing/serialization
- `src/main.cpp` is deliberately thin. Keep it as an entry point only; environment wiring belongs in `application/runtime/` or `infrastructure/`.

## ModestIoT Rules
- The project must stay aligned with **ModestIoT** (`lib/ModestIoT/`).
- Follow the framework's mental model naturally:
  - domain IMUs align with `Sensor`
  - `UflexDevice` aligns with `Device`
  - buzzer, RGB LED, and vibration motor align with `Actuator`
  - event/command flow should stay explicit
- Do **not** copy the LED example literally into `uFlex`; use it as a conceptual reference only.

## Runtime / Data Flow
- Compile-time target selection happens through `platformio.ini` and `include/config/build_config.h`.
- `esp32_sim` defines `UFLEX_TARGET_SIM`; `esp32_hw` defines `UFLEX_TARGET_HW`.
- `lib/uflex/src/application/runtime/create_uflex_runtime.cpp` chooses the runtime at compile time:
  - `SimUflexRuntime` uses `SimulatedImuArray`, simulation Wi-Fi defaults, `EdgeClient`, and `NoOpBleTransport`
  - `HwUflexRuntime` uses `Mpu9250ImuArray`, GPIO actuator adapters, `EdgeClient`, and `BleTelemetryServer`
- Both IMU adapters update the same domain `Imu` objects, so domain code stays backend-agnostic.
- The loop reads IMUs, advances orientation filters, calculates relative angles/rotations, polls active serie context, handles settle-based zero calibration, evaluates safety, applies outputs, publishes BLE/edge data, and prints serial diagnostics.
- Serial diagnostics come from `UflexApplication`, which maps `MotionState` -> `MotionPayload` -> JSON. BLE and edge payloads use their own mapper/serializer contracts.
- Hardware secrets are build-time inputs. Do not hardcode Wi-Fi credentials, edge hosts, or device API keys; use `.env` plus `scripts/build_hw.sh` / `scripts/build_hw.ps1`.

## Build / Validation
- Primary validation is PlatformIO compilation plus the existing Unity suite.
- Use:
  - `pio run -e esp32_sim` for the simulation target
  - `pio test -e esp32_sim --without-uploading --without-testing` to compile tests without requiring a device/test port
  - `pio test -e esp32_sim` when a connected or simulated ESP32 test target is available
  - `./scripts/build_hw.sh` or `.\scripts\build_hw.ps1` for the hardware target with secrets loaded from `.env`
- If you touch shared code in `domain/`, `application/`, or common infrastructure, validate at least `pio run -e esp32_sim`.
- If you touch tested domain/transport behavior, also compile or run the `esp32_sim` tests.
- If you touch hardware wiring, MPU9250/TCA9548A access, BLE server behavior, Wi-Fi, edge transport, or build secrets, validate the hardware build path too.

## Project-Specific Conventions
- Public include style should use project paths, e.g. `#include "uflex/domain/devices/uflex_device.h"`.
- `build_config.h` is included as `#include "config/build_config.h"` from library code.
- Naming conventions in project code:
  - classes/structs: `PascalCase`
  - functions/variables: `camelCase`
  - constants/macros: `UPPER_SNAKE_CASE`
  - files/folders: `snake_case`
- Preserve the IDE header already used across project files:
  - `//`
  - `// Created by <team member name> on <date>.`
  - `//`

## Files to Read Before Refactoring
- `README.md`: current project framing and architecture summary
- `CONTRIBUTING.md` / `CONTRIBUTING.es.md`: workflow and style conventions
- `include/config/build_config.h`: build target, identity, edge, BLE, and secret handling
- `lib/uflex/include/uflex/domain/devices/uflex_device.h`: central domain object
- `lib/uflex/include/uflex/application/uflex_application.h`: top-level loop coordinator
- `lib/uflex/include/uflex/application/runtime/uflex_runtime.h`: runtime contract
- `lib/uflex/include/uflex/infrastructure/adapters/sim/simulated_imu_array.h`: simulation adapter pattern
- `lib/uflex/include/uflex/infrastructure/adapters/hw/mpu9250_imu_array.h`: hardware adapter pattern
- Relevant transport headers under `lib/uflex/include/uflex/infrastructure/transport/` when changing BLE, edge, JSON, or active-context behavior.

## Editing Guidance
- Keep domain code free from JSON, HTTP, BLE, Wi-Fi, `Wire`, GPIO, or board-specific details.
- New environment-specific wiring belongs in `application/runtime/` or `infrastructure/`, not in `main.cpp`.
- Keep active serie context and device identity behavior consistent with `docs/device-identity-contract.md` and `include/config/build_config.h`.
- Keep local safety behavior independent from network publishing; safety should use the cached active context and drive outputs locally.
- Place new tests under `test/`, following the current `test/test_domain/` style unless a different suite boundary is clearer.
- Respect `.editorconfig` and `.gitattributes`; the repo normalizes to UTF-8, LF, and 4-space indentation by default.
