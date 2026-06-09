# ToggableLedDevice Example (C++ Edition)

**Version**: 0.1  
**Author**: Angel Velasquez  
**Date**: March 23, 2025

## Overview

This project demonstrates the usage of the Modest IoT Nano-framework (C++ Edition) v0.1 by implementing a simple IoT device: the `ToggableLedDevice`. It toggles an LED on an ESP32 via a button press, showcasing the framework’s object-oriented, event-driven, and CQRS-inspired design. This example is not part of the core framework but serves as a practical illustration of how to apply it.

The framework itself is maintained separately in [`Modest-IoT-Nano-framework-Cpp`](https://github.com/avelasquezn/Modest-IoT-Nano-framework-Cpp).

## Prerequisites
- **Hardware**: ESP32 development board, a push button (active-low with pull-up), an LED with a 220Ω resistor.
- **Software**: Arduino IDE with ESP32 support, or Wokwi for simulation.
- **Dependency**: Modest IoT Nano-framework (C++ Edition) v0.1.

## Installation
1. **Download Framework**: Clone or download [`Modest-IoT-Nano-framework-Cpp`](https://github.com/avelasquezn/Modest-IoT-Nano-framework-Cpp).
2. **Download Example**: Clone or download this repository from [avelasquezn/ToggableLedDevice-Example-Cpp](https://github.com/avelasquezn/ToggableLedDevice-Example-Cpp).
3. **Directory Structure**: Combine files (assuming both repos are siblings):
```planetext
ToggableLedDevice-Example-Cpp/
├── ToggableLedDevice.ino
├── ToggableLedDevice.h
├── ToggableLedDevice.cpp
├── diagram.json
├── wokwi.toml
├── user-stories.md
├── (copied from Modest-IoT-Nano-framework-Cpp)
│   ├── ModestIoT.h
│   ├── EventHandler.h
│   ├── CommandHandler.h
│   ├── Sensor.h
│   ├── Sensor.cpp
│   ├── Button.h
│   ├── Button.cpp
│   ├── Actuator.h
│   ├── Actuator.cpp
│   ├── Led.h
│   ├── Led.cpp
│   ├── Device.h
│   └── Device.cpp
````
