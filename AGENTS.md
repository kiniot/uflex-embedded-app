# AGENTS.md

## Big Picture
- This is an ESP32/PlatformIO firmware project for **uFlex**, a telerehabilitation device that reads **3 IMUs**, computes relative angles, and prepares motion data for future BLE/Wi-Fi transport.
- The main code lives in `lib/uflex/` and is intentionally split into:
  - `domain/`: problem model and rules (`Imu`, `UflexDevice`, `MotionState`, `RelativeAngleCalculator`)
  - `application/`: runtime orchestration and main loop policy (`UflexApplication`, `createUflexRuntime`)
  - `infrastructure/`: simulation/hardware adapters and transport mapping/serialization
- `src/main.cpp` is deliberately thin. Keep it as an entry point only; environment wiring belongs in `application/runtime/`.

## ModestIoT Rules
- The project must stay aligned with **ModestIoT** (`lib/ModestIoT/`).
- Follow the framework’s mental model naturally:
  - domain sensors align with `Sensor`
  - the central coordinator aligns with `Device`
  - output-oriented behavior may align with `Actuator`
  - event/command flow should stay explicit
- Do **not** copy the LED example literally into `uFlex`; use it as a conceptual reference only.

## Runtime / Data Flow
- Compile-time target selection happens through `platformio.ini` and `include/config/build_config.h`.
- `esp32_sim` defines `UFLEX_TARGET_SIM`; `esp32_hw` defines `UFLEX_TARGET_HW`.
- `lib/uflex/src/application/runtime/create_uflex_runtime.cpp` chooses the runtime at compile time:
  - `SimUflexRuntime` uses `SimulatedImuArray`
  - `HwUflexRuntime` uses `Mpu9250ImuArray`
- Both adapters update the same domain `Imu` objects, so domain code stays backend-agnostic.
- Serial output comes from `UflexApplication`, which maps `MotionState` -> `MotionPayload` -> JSON.

## Build / Validation
- Primary validation is compilation, not automated tests yet.
- Use:
  - `pio run -e esp32_sim` for the simulation target
  - `pio run -e esp32_hw` for the hardware target
- If you touch shared code in `domain/`, `application/`, or common infrastructure, validate at least `esp32_sim`.

## Project-Specific Conventions
- Public include style should use project paths, e.g. `#include "uflex/domain/devices/uflex_device.h"`.
- `build_config.h` is included as `#include "config/build_config.h"` from library code.
- Naming conventions in project code:
  - classes/structs: `PascalCase`
  - functions/variables: `camelCase`
  - files/folders: `snake_case`
- Preserve the IDE header already used across project files:
  - `//`
  - `// Created by <team member name> on <date>.`
  - `//`

## Files to Read Before Refactoring
- `README.md`: current project framing and architecture summary
- `CONTRIBUTING.md` / `CONTRIBUTING.es.md`: workflow and style conventions
- `lib/uflex/include/uflex/domain/devices/uflex_device.h`: central domain object
- `lib/uflex/include/uflex/application/uflex_application.h`: top-level loop coordinator
- `lib/uflex/include/uflex/infrastructure/adapters/sim/simulated_imu_array.h`: simulation adapter pattern
- `lib/uflex/include/uflex/infrastructure/adapters/hw/mpu9250_imu_array.h`: hardware adapter pattern

## Editing Guidance
- Keep domain code free from JSON, HTTP, BLE, Wi-Fi, `Wire`, or board-specific details.
- New environment-specific wiring belongs in `application/runtime/` or `infrastructure/`, not in `main.cpp`.
- If you add tests later, place them under `test/`; current repo convention still centers on build validation first.
- Respect `.editorconfig` and `.gitattributes`; the repo normalizes to UTF-8, LF, and 4-space indentation by default.
