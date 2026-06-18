# Hardware Overview

This document describes the physical sensing topology of the uFlex embedded firmware:
how the inertial sensors are wired, how they are addressed over I2C, and how their
readings flow into the domain motion model.

It complements the build and simulation guides:

- [Environment Setup](env-setup.md)
- [Hardware Simulation](simulation-setup.md)

---

## Goal

uFlex captures limb motion for telerehabilitation by reading **three IMUs** placed on
different limb segments and computing the **relative angles** between sensor pairs. The
firmware exposes a processed `MotionState` that downstream transport layers (serial, BLE,
Wi-Fi/HTTP) can consume.

---

## Sensor topology

| Component | Role |
| --- | --- |
| ESP32 (`esp32dev`) | Main controller, runs the firmware |
| 3× MPU9250 | Inertial measurement units, one per tracked segment |
| TCA9548A | I2C multiplexer used to reach multiple IMUs that share the same address |

The three MPU9250 IMUs are modeled as a fixed array (`Mpu9250ImuArray`) where each domain
`Imu` is bound to the I2C bus used to reach it:

```text
ESP32 ── I2C ──> TCA9548A ──┬── channel 0 ──> MPU9250 (segment A)
                            ├── channel 1 ──> MPU9250 (segment B)
                            └── channel 2 ──> MPU9250 (segment C)
```

### Why a multiplexer?

The MPU9250 exposes only two selectable I2C addresses (`0x68` / `0x69` via the `AD0` pin),
which is not enough to place three identical sensors on a single bus without conflicts. The
**TCA9548A** solves this by giving each sensor its own isolated downstream channel, so the
firmware can talk to one IMU at a time through the same logical address.

---

## I2C details

- **Bus speed / monitor:** the serial monitor runs at `115200` baud (see `platformio.ini`).
- **Device identification:** each MPU9250 is validated through its `WHO_AM_I` register
  (`0x75`), expecting `0x71`.
- **Wake-up:** sensors are taken out of sleep through `PWR_MGMT_1` (`0x6B`).
- **Sampling:** accelerometer, gyroscope, and temperature are read starting at the
  measurement register (`0x3B`); 16-bit values are decoded as **big-endian**.

The current hardware adapter intentionally focuses on accelerometer, gyroscope, and
temperature so it matches the same `ImuSample` contract already used by the simulator.

---

## From samples to motion

1. `Mpu9250ImuArray::begin()` detects and initializes all configured MPU9250 devices.
2. `Mpu9250ImuArray::update()` reads each IMU and updates its domain `Imu` sample.
3. `RelativeAngleCalculator` derives the relative angles between sensor pairs.
4. The result is published as a `MotionState`, ready to be serialized and transported.

This keeps the domain model independent from wiring: the **simulation** target swaps the
hardware adapter for a `SimulatedImuArray`, while the rest of the motion pipeline stays
identical.

---

## Build targets

The same sensing model is compiled for two PlatformIO environments:

| Environment | Macro | IMU source |
| --- | --- | --- |
| `esp32_hw` | `UFLEX_TARGET_HW` | Real MPU9250 over TCA9548A |
| `esp32_sim` | `UFLEX_TARGET_SIM` | Simulated IMU array (Wokwi) |

```bash
pio run -e esp32_hw    # build for real hardware
pio run -e esp32_sim   # build for simulation
```

---

## Status & notes

- The MPU9250 adapter is a **preliminary** integration: it is structurally ready for real
  hardware but still needs validation against the physical board and the final multiplexer
  wiring.
- Magnetometer (AK8963) data is **not** read yet; the first version is limited to the
  accelerometer/gyroscope/temperature set required by the current motion calculations.
- Channel-to-segment assignment is fixed at three IMUs by design (`ImuBinding imus[3]`).
