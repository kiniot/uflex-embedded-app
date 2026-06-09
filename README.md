<h1 align="center">uFlex Embedded Application</h1>

<div align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-Embedded-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++ Embedded" />
  <img src="https://img.shields.io/badge/ESP32-Arduino-E7352C?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32 Arduino" />
  <img src="https://img.shields.io/badge/PlatformIO-Build-F5822A?style=for-the-badge&logo=platformio&logoColor=white" alt="PlatformIO" />
  <img src="https://img.shields.io/badge/Wokwi-Simulation-2D8CFF?style=for-the-badge" alt="Wokwi Simulation" />
  <br />
  <img src="https://img.shields.io/badge/Architecture-Domain%20%2F%20Application%20%2F%20Infrastructure-blue?style=flat-square" alt="Domain Application Infrastructure" />
  <img src="https://img.shields.io/badge/Model-ModestIoT-4B5563?style=flat-square" alt="ModestIoT" />
  <img src="https://img.shields.io/badge/Motion-3%20IMUs-0F766E?style=flat-square" alt="3 IMUs" />
  <img src="https://img.shields.io/badge/Targets-esp32__sim%20%7C%20esp32__hw-6B7280?style=flat-square" alt="Simulation and Hardware Targets" />
</div>

---

This repository contains the **uFlex embedded firmware**, an ESP32-based telerehabilitation project focused on capturing limb motion through multiple IMUs and producing processed motion data for future transport layers such as BLE and Wi-Fi.

The project currently supports:
- a **simulation target** for Wokwi-based development
- a **hardware target** for real-device integration
- a domain model inspired by **ModestIoT**
- an internal architecture organized around `domain/`, `application/`, and `infrastructure/`

---

## Overview

uFlex models motion sensing around three IMUs attached to different limb segments. The firmware reads inertial samples, computes relative angles between sensor pairs, and exposes a processed motion state that can later be consumed by serial diagnostics, BLE, HTTP, or other transport mechanisms.

At the moment, the project already includes:
- a domain-centered motion model
- a simulation adapter for IMU input
- a preliminary hardware adapter for MPU9250-based integration
- a transport layer for motion payload mapping and JSON serialization

---

## ModestIoT

This project follows the **ModestIoT** approach as part of its academic and architectural foundation.

In practice, that means:
- domain sensors are modeled in alignment with `Sensor`
- the central domain coordinator is modeled in alignment with `Device`
- domain outputs can also be modeled in alignment with `Actuator`
- event-driven interaction remains explicit
- the firmware entry point should stay thin

These abstractions belong naturally to the same ModestIoT model, alongside explicit event and command handling.

The framework sources are preserved in:
- [`lib/ModestIoT/`](lib/ModestIoT/)

Additional project-specific context is available in:
- [`lib/ModestIoT/README.md`](lib/ModestIoT/README.md)

---

## Architecture

The project is organized into three main layers inside `lib/uflex/`:

- `domain/`
  - core motion concepts and business rules
  - `Imu`, `UflexDevice`, `MotionState`, `RelativeAngleCalculator`
- `application/`
  - application loop coordination
  - runtime selection and environment-specific orchestration
- `infrastructure/`
  - simulation adapters
  - hardware adapters
  - transport payload mapping and serialization

This separation helps keep:
- domain logic independent from hardware and transport details
- `main.cpp` lightweight
- simulation and hardware as interchangeable runtime backends around the same shared model

---

## Repository Structure

Key directories:

- `src/`
  - minimal firmware entry point
- `include/`
  - global build-time configuration headers
- `lib/ModestIoT/`
  - framework sources used as the conceptual base of the project
- `lib/uflex/`
  - the project’s main firmware code
- `docs/`
  - setup and simulation documentation
- `test/`
  - reserved for PlatformIO tests

---

## Build Targets

The firmware currently defines two PlatformIO environments:

- `esp32_sim`
  - simulation-oriented target
  - uses the simulation runtime and simulated IMU adapter
- `esp32_hw`
  - hardware-oriented target
  - uses the hardware runtime and the preliminary MPU9250 adapter

Target selection is controlled through `platformio.ini` and compile-time macros:
- `UFLEX_TARGET_SIM`
- `UFLEX_TARGET_HW`

---

## Prerequisites

To build the project, install:

1. **Python 3.11+**
2. **PlatformIO Core**

You may also use an IDE such as:
- **VS Code**
- **CLion**

For simulation workflows, you will also need:
- **Wokwi**

---

## Build Commands

```bash
# Build simulation target
pio run -e esp32_sim

# Build hardware target
pio run -e esp32_hw
```

---

## Documentation

Setup and environment guides:

- [Environment Setup Guide (English)](docs/env-setup.md)
- [Guía de Configuración del Entorno (Español)](docs/env-setup.es.md)

Simulation guides:

- [Hardware Simulation Guide (English)](docs/simulation-setup.md)
- [Guía de Simulación de Hardware (Español)](docs/simulation-setup.es.md)

Contribution guides:

- [Contribution Guide (English)](CONTRIBUTING.md)
- [Guía de Colaboración (Español)](CONTRIBUTING.es.md)

Authors:

- [Project Authors (English)](AUTHORS.md)
- [Autores del Proyecto (Español)](AUTHORS.es.md)
