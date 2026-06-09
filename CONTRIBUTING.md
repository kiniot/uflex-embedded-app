# Contribution Guide

Available languages:
- [English](CONTRIBUTING.md)
- [Spanish](CONTRIBUTING.es.md)

This repository contains the **uFlex embedded firmware** built on **ESP32**, **PlatformIO**, and an architecture inspired by **ModestIoT**.  
This guide defines how we collaborate so the project stays consistent in structure, style, and workflow.

---

## 1) Quick Start

### Requirements
- **Python 3.11+**
- **PlatformIO Core**
- A compatible IDE such as **VS Code** or **CLion**
- For simulation: a configured **Wokwi** environment
- For real hardware: an **ESP32** board and, when applicable, the physical sensors

### Clone the repository
```bash
git clone <REPOSITORY-URL>
cd <REPO-NAME>
```

### Configure your Git identity
```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

### Build the project
This project uses multiple PlatformIO environments:
- `esp32_sim` for simulation
- `esp32_hw` for real hardware

```bash
# Build simulation
pio run -e esp32_sim

# Build hardware
pio run -e esp32_hw
```

> If this is your first contribution, create a working branch before making changes.

---

## 2) Git Workflow

- Central repository: **GitHub**
- Integration and working branch: `develop`
- Final stable release branch: `main`
- New features and refactors should be developed in working branches created from `develop`

### Branch naming
- `feature/<topic>` for new functionality
- `fix/<topic>` for bug fixes
- `refactor/<topic>` for structural changes without intended behavior changes
- `docs/<topic>` for documentation

Examples:
- `feature/ble-motion-stream`
- `fix/sim-runtime-init`
- `refactor/application-runtime`
- `docs/contributing-guide`

### Create a working branch
```bash
git checkout develop
git pull origin develop
git checkout -b feature/<your-topic>
```

### Keep your branch updated
```bash
git fetch origin
git merge origin/develop
```

---

## 3) Commit Messages

We use **Conventional Commits** in English.

### Format
```text
<type>(<scope>): <message>
```

### Common types
- `feat` → new functionality
- `fix` → bug fix
- `refactor` → structural improvement without intended behavior change
- `docs` → documentation
- `test` → tests
- `build` → PlatformIO, dependency, or configuration changes
- `style` → formatting and style-only changes

### Suggested scopes
- `domain`
- `application`
- `infrastructure`
- `sim`
- `hw`
- `transport`
- `ble`
- `wifi`
- `edge`
- `docs`
- `build`

### Examples
```text
feat(transport): add motion payload batch serializer
fix(sim): correct secondary bus initialization
refactor(application): extract runtime factory from main
docs(domain): document imu and motion state
build(platformio): add ArduinoJson dependency
```

---

## 4) Pull Requests

- The normal base branch for PRs is `develop`.
- The `main` branch is reserved for final stable releases or closed project milestones.
- Each PR should focus on one clear logical change.
- Avoid mixing refactors, documentation, and unrelated new functionality in the same PR.

### Checklist before opening a PR
- [ ] The project builds in the affected environment.
- [ ] If you changed shared code, you validated at least `pio run -e esp32_sim`.
- [ ] No fragile include paths or unnecessary hacks were introduced.
- [ ] Documentation was updated when needed.
- [ ] Commits follow Conventional Commits.
- [ ] No secrets, Wi-Fi keys, or credentials were committed.

---

## 5) Project Architecture Principles

The current project architecture is organized around these layers:
- `domain/`
- `application/`
- `infrastructure/`

### Important rule
The project **must follow the ModestIoT approach** in domain modeling.

That means, whenever applicable:
- using event-driven interactions
- keeping the main coordination inside the domain device
- modeling domain sensors in alignment with `Sensor`
- modeling the central device in alignment with `Device`
- avoiding domain or integration logic concentrated in `main.cpp`

### What belongs in each layer
- `domain/`: problem models, domain sensors, central device, angle calculations, motion state
- `application/`: execution coordination, runtime selection, main application loop
- `infrastructure/`: simulation adapters, hardware adapters, transport, networking, and future external integrations

### What to avoid
- putting business logic in `main.cpp`
- mixing hardware details into the domain
- coupling the domain to JSON, HTTP, BLE, or transport libraries
- introducing unnecessary extra layers without a real need

---

## 6) Code Style and Conventions

This project uses **C++** on top of **Arduino/ESP32** with PlatformIO.

### General rules
- Use **UTF-8** encoding
- Use **4 spaces** for indentation
- Do not use tabs
- Keep lines reasonably short
- Avoid oversized classes
- Avoid unnecessary commented-out code
- Prefer small, cohesive changes

### Naming conventions
- **Classes / structs**: `PascalCase`
  - `UflexDevice`
  - `ImuSample`
  - `MotionPayload`
- **Functions and variables**: `camelCase`
  - `updateSample`
  - `getMotionState`
  - `middleLowerAngle`
- **Constants/macros**: `UPPER_SNAKE_CASE`
  - `UFLEX_TARGET_SIM`
  - `READ_INTERVAL_MS`
- **Project source files and folders**: `snake_case`
  - `imu_sample.h`
  - `motion_payload_serializer.cpp`

### Includes
- Prefer public project include paths:
```cpp
#include "uflex/domain/devices/uflex_device.h"
```
- Avoid fragile relative includes when a clearer public include path exists.

### Headers and documentation
- Keep include guards clear and consistent.
- Preserve the IDE-generated header where that convention is already used:
```cpp
//
// Created by <team member name> on <date>.
//
```
- Document important files with `@file`, `@brief`, author, date, and version metadata.

---

## 7) ModestIoT in This Project

The **ModestIoT** framework is part of the conceptual foundation of this project and should be respected in the design.

### In practice
- `Imu` should follow the idea of a `Sensor`
- `UflexDevice` should follow the idea of a `Device`
- when the domain requires it, output can also be modeled through an `Actuator`
- coordination should happen inside the domain device, not in the entry point

### Important
- `ModestIoT` should guide the domain model
- the framework also supports working with classes such as `Sensor`, `Actuator`, and `Device`
- its LED example should not be forced as a literal template for everything
- examples in `lib/ModestIoT/` can serve as references, but `uFlex` code must respond to the actual needs of the project

---

## 8) Simulation and Hardware

The project currently supports two main environments:
- `esp32_sim`
- `esp32_hw`

### Simulation
Uses the simulation adapter and the simulation runtime.

```bash
pio run -e esp32_sim
```

### Hardware
Uses the hardware runtime and the physical adapters.

```bash
pio run -e esp32_hw
```

### Contribution rule
If you modify code that affects both environments, make sure you do not break the separation between:
- simulation runtime
- hardware runtime
- shared domain

---

## 9) Testing and Validation

The current main validation mechanisms are:
- per-environment compilation
- manual simulation testing
- future real-hardware validation

### Minimum recommended validation
```bash
pio run -e esp32_sim
```

If the change touches hardware or shared infrastructure, also validate:
```bash
pio run -e esp32_hw
```

### What to review manually
- the correct target builds successfully
- includes do not break other modules
- `main.cpp` remains lightweight
- the `domain/application/infrastructure` separation stays clear

---

## 10) Good Contribution Practices

- One logical change per branch or PR
- Prefer incremental refactors
- Document structural decisions when they are not obvious
- Keep the domain free from transport details
- Design simulation and hardware as adapters around the same model
- Do not add external dependencies without a clear reason

---

## 11) Conflict Resolution

Update your branch frequently to reduce conflicts:
```bash
git fetch origin
git merge origin/develop
```

If conflicts occur:
- resolve them locally
- review sensitive files carefully, especially:
  - `platformio.ini`
  - `src/main.cpp`
  - `include/build_config.h`
  - `application/runtime/` components
  - `infrastructure/` adapters

Avoid `force push` unless the team explicitly agrees on it.

---

## 12) Communication

- If you make architecture changes, describe them clearly in the PR.
- If a change touches `domain/`, explain how it remains aligned with ModestIoT.
- If you introduce new infrastructure such as BLE, Wi-Fi, or edge integration, be explicit about what is still preliminary and what has already been validated.

---
