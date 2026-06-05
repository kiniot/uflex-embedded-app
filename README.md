# uFlex Embedded Application

This repository contains the firmware for the uFlex IoT-based telerehabilitation system. The system utilizes an ESP32 microcontroller, an Adafruit TCA9548A I2C multiplexer, and multiple MPU9250 inertial measurement units (IMUs) to capture and stream precise limb mobility data over Bluetooth Low Energy (BLE).

## Project Structure

The project is structured according to the standard PlatformIO layout:

* `src/`: Main source files (`main.cpp` and hardware execution logic).
* `include/`: Global header definitions and macro configurations.
* `lib/`: Project-specific private components and modular libraries.
* `docs/`: Detailed system documentation and environment setup manuals.
* `platformio.ini`: Declarative project configuration and dependency specification.

## Prerequisites

To compile, deploy, or simulate this firmware, you must have the following core components installed on your local operating system:

1.  **Python 3.11+**: Required to run the PlatformIO compilation engine.
2.  **PlatformIO Core**: The unified ecosystem used to manage toolchains and libraries.

For detailed setup and hardware emulation workflows regarding your operating system and preferred IDE (VS Code or CLion), refer to the documentation onboarding section below.

## Documentation and Onboarding Guides

Detailed instructions are available in both English and Spanish for setting up physical workstations as well as virtual electronics environments:

### Local Workstation Configurations
* [Environment Setup Guide (English)](docs/env-setup.md)
* [Guía de Configuración del Entorno (Español)](docs/env-setup.es.md)

### Virtual Hardware Simulations (Wokwi)
* [Hardware Simulation Guide (English)](docs/simulation-setup.md)
* [Guía de Simulación de Hardware (Español)](docs/simulation-setup.es.md)
