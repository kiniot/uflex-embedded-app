# uFlex Embedded Application

This repository contains the firmware for the uFlex IoT-based telerehabilitation system. The system utilizes an ESP32 microcontroller, an adafruit TCA9548A I2C multiplexer, and multiple MPU9250 inertial measurement units (IMUs) to capture and stream precise limb mobility data over Bluetooth Low Energy (BLE).

## Project Structure

The project is structured according to the standard PlatformIO layout:

* `src/`: Main source files (`main.cpp` and hardware execution logic).
* `include/`: Global header definitions and macro configurations.
* `lib/`: Project-specific private components and modular libraries.
* `docs/`: Detailed system documentation and environment setup manuals.
* `platformio.ini`: Declarative project configuration and dependency specification.

## Prerequisites

To compile and deploy this firmware, you must have the following core components installed on your local operating system:

1.  **Python 3.11+**: Required to run the PlatformIO compilation engine.
2.  **PlatformIO Core**: The unified ecosystem used to manage toolchains and libraries.

For detailed setup instructions regarding your operating system and preferred IDE (VS Code or CLion), refer to the documentation guides below.

## Documentation and Onboarding Guides

Detailed instructions are available in both English and Spanish:

* [Environment Setup Guide (English)](docs/env-setup.md)
* [Guía de Configuración del Entorno (Español)](docs/env-setup.es.md)