# uFlex Embedded App - Class Diagram

## Overview

The PlantUML diagram in `uflex-embedded-class-diagram.puml` represents the current production
structure of the uFlex ESP32 firmware. It follows the path from I2C IMU readings through the shared
domain model, relative-angle calculation, application orchestration, and JSON-ready motion payload.

The diagram is based on the classes, structs, free runtime factory function, configuration files,
and relationships currently present in the repository. Test-only helpers and the standalone
ModestIoT LED/button example are intentionally omitted from the architectural view.

## Architectural Scope

- **domain** contains `ImuSample`, `Imu`, `RelativeAngle`, `RelativeAngleCalculator`,
  `MotionState`, and `UflexDevice`.
- **application** contains `UflexApplication`, which controls the read cadence and serial output.
- **application.runtime** contains the `UflexRuntime` interface, its simulation and hardware
  implementations, and the `createUflexRuntime` free factory function.
- **infrastructure.adapters.sim** contains `SimulatedImuArray` and its `ImuBinding`.
- **infrastructure.adapters.hw** contains `Mpu9250ImuArray` and its `ImuBinding`.
- **infrastructure.transport** contains `MotionPayload`, `MotionPayloadMapper`, and
  `MotionPayloadSerializer`.
- **ModestIoT** contains the framework types directly relevant to uFlex inheritance and event or
  command handling: `Event`, `Command`, `EventHandler`, `CommandHandler`, `Sensor`, `Device`, and
  `Actuator`.

## Main Relationships

`Imu` inherits from the ModestIoT `Sensor` class, while `UflexDevice` inherits from `Device`.
`UflexDevice` owns three `Imu` instances and stores the three relative angles calculated from their
latest samples. Each `Imu` propagates `MOTION_DETECTED_EVENT` to the device, which calls
`RelativeAngleCalculator` and exposes the result as a `MotionState`.

Both runtime implementations own a `UflexDevice`. `SimUflexRuntime` also owns a
`SimulatedImuArray`, while `HwUflexRuntime` owns an `Mpu9250ImuArray`. Their binding structs hold
references to the same domain `Imu` instances owned by their runtime's device, so both environment
adapters update the same backend-independent domain model.

`UflexApplication` holds a reference to `UflexRuntime`. After a successful runtime update, it reads
the device's `MotionState`, uses `MotionPayloadMapper` to create a `MotionPayload`, and uses
`MotionPayloadSerializer` to produce compact JSON for serial diagnostics.

## Runtime Selection

`platformio.ini` defines `UFLEX_TARGET_SIM` for `esp32_sim` or `UFLEX_TARGET_HW` for `esp32_hw`.
`include/config/build_config.h` enforces that exactly one target macro is active and defines the
corresponding target name.

The free function `createUflexRuntime()` uses those macros at compile time. It returns a reference
to a static `SimUflexRuntime` for the simulation target or a static `HwUflexRuntime` for the
hardware target. `src/main.cpp` passes that reference to one static `UflexApplication`, keeping the
firmware entry point thin.

## Notes

- The hardware adapter is preliminary and still requires validation against the physical ESP32,
  final MPU9250 wiring, and any multiplexer arrangement.
- The current MPU9250 adapter reads accelerometer, gyroscope, and temperature data. Magnetometer
  support is not implemented.
- BLE, Wi-Fi, HTTP, and other network transports are described as future directions but no such
  production classes currently exist, so they are not shown.
- ModestIoT includes `Actuator`, but no current uFlex production class derives from it.
- No enums are currently declared in the analyzed uFlex production headers.
