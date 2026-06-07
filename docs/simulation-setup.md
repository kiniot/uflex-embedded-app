# Hardware Simulation Guide (Wokwi Integration)

This document provides step-by-step instructions to configure and run local hardware simulations for the uFlex embedded application. This allows you to test code logic, sensor inputs, and multiplexer routing without needing physical hardware connected to your computer.

---

## 1. Shared Simulation Configuration

Both VS Code and CLion utilize the same underlying Wokwi simulation engine parameters. Before configuring your specific IDE, you must ensure that you have the following files in the project root directory (at the same level as the `platformio.ini` file).

### 1. Compile the Correct PlatformIO Environment
This repository uses multiple environments in `platformio.ini`. For Wokwi simulation, the expected environment is `esp32_sim`, while `esp32_hw` is reserved for real hardware.

Before starting Wokwi, explicitly compile the simulation environment from your IDE's PlatformIO panel, or use the following command:

```bash
pio run -e esp32_sim
```

### 2. Define the Firmware Target Map (`wokwi.toml`)
Ensure you have the following file in the project root named `wokwi.toml` with structural definitions to route the simulator to your PlatformIO-compiled binaries and register any custom chips required by the simulation:

```toml
[wokwi]
version = 1
firmware = '.pio/build/esp32_sim/firmware.bin'
elf = '.pio/build/esp32_sim/firmware.elf'

[[chip]]
name = 'tca9548a'
binary = 'chips/tca9548a/dist/tca9548a.chip.wasm'
```

> **Important:** Make sure to compile the `esp32_sim` environment at least once before launching the simulation. If you do not compile first, the internal folders and the `firmware.bin` and `firmware.elf` files will not exist on disk, meaning the simulator will not be able to resolve the paths and will raise an initialization error.

### 3. Define the Electronic Circuit Schematic (`diagram.json`)
Ensure you have the following file in the project root named `diagram.json` to map out your virtual hardware board connections.
> **Note:** You can use the same `diagram.json` file that you generate when designing your circuit visually on the [Wokwi Official Website](https://wokwi.com). Just wire the ESP32, sensors, and actuators in the web editor, click on the **diagram.json** tab within its workspace, copy the contents, and paste them directly into your local file.

### 4. Maintain the Project's Custom Chips
This repository may include one or more Wokwi custom chips to simulate peripherals or components that do not exist natively in the simulator catalog. The recommended structure is:

```text
chips/
  common/
    wokwi-api.h
  chip_name/
    src/
      main.c
    dist/
      chip_name.chip.json
      chip_name.chip.wasm
```

Important points:

1. `main.c` is the custom chip source code.
2. `wokwi-api.h` is required to compile the chips, but it is not part of the ESP32 firmware.
3. Each chip's `.chip.json` and `.chip.wasm` files must be kept together inside `dist/`.
4. Custom chips are used only during simulation; they are never flashed to real hardware.

### 5. Rebuild a Custom Chip When It Changes
If you modify a custom chip source file, you must rebuild it to regenerate the WebAssembly artifact.

To do so, first install [wokwi-cli](https://github.com/wokwi/wokwi-cli). Then run a command like the following:

```bash
wokwi-cli chip compile chips/tca9548a/src/main.c -o chips/tca9548a/dist/tca9548a.chip.wasm
```

---

## 2. Option A: Visual Studio Code Setup

VS Code interacts with the simulation environment through the official Wokwi extension.

### Installation and Licenses
1. Open VS Code and navigate to the **Extensions** marketplace menu (`Ctrl+Shift+X` or `Cmd+Shift+X`).
2. Search for **Wokwi Simulator** and install the extension.

   ![Wokwi Extension in VS Code](./assets/simulation-setup/vscode-wokwi-extension.png)

3. Head over to the left sidebar of VS Code and open the **Wokwi Simulator** tab (identified by the chip and debugging bug icon).
4. Follow the redirect instructions in your browser to activate your free developer token for the simulator.

   ![Start Wokwi Simulation in VS Code](./assets/simulation-setup/vscode-wokwi-start.png)

### Running the Simulator
1. Ensure your source code compiles cleanly using the `esp32_sim` environment, either from the PlatformIO Build checkmark icon (`✓`) or by running `pio run -e esp32_sim`.
2. Go to the left sidebar of VS Code and open the **Wokwi Simulator** tab.
3. Expand the **Quick Actions** section inside the simulation control panel.
4. Click on the **Start Simulation** option to initialize the virtual environment.
5. An interactive tab will open displaying the graphical layout of your wired electronic components. Live readings and logs from the virtual serial port will stream immediately to your integrated terminal at the bottom of the screen.

   ![Running Wokwi Simulation in VS Code](./assets/simulation-setup/vscode-wokwi-run.png)

---

## 3. Option B: JetBrains CLion Setup

CLion manages simulations through a dedicated control panel integrated into the IDE's sidebar toolbar.

### Installation and Workspace Binding
1. Open CLion and go to **Settings** (or *Preferences* on macOS) > **Plugins**.
2. Switch to the **Marketplace** tab, search for **Wokwi Simulator**, install the plugin module, and restart the IDE.

   ![Wokwi Plugin for CLion](./assets/simulation-setup/clion-wokwi-plugin.png)

3. Upon restarting, you will notice a new tab named **Wokwi Simulator** added to the far end of the right sidebar toolbar in CLion.

### License Activation and Path Configuration
Before launching the virtual environment, you must validate the plugin status and map your local files:

1. Click on the **Wokwi Simulator** tab in the right panel of CLion to expand its interface.
2. If this is your first time opening it, follow the instructions to activate the free developer license through your web browser.
3. In the **Settings** section, strictly configure the communication paths:
    * **wokwi.toml path:** Click on the folder icon and select the `wokwi.toml` file located in your project's root.
    * **diagram.json path:** Select the `diagram.json` file from the project root in the same manner.

   ![Start Wokwi Simulation in CLion](./assets/simulation-setup/clion-wokwi-start.png)

### Running the Simulation
1. **Mandatory Compilation Step:** Head over to the **PlatformIO** tab first and perform a **Build** of the `esp32_sim` environment to generate the updated firmware in your local storage.
2. Return to the **Wokwi Simulator** tab in the right panel.
3. Click on the top **Start Simulator** button.
4. An interactive canvas featuring your virtualized electronic circuit will immediately open as a native tab inside the CLion editor, capturing the serial port data stream in real time.

   ![Running Wokwi Simulation in CLion](./assets/simulation-setup/clion-wokwi-run.png)

> **Current project warning:** In this repository, custom chip simulation has been more reliable in VS Code than in CLion. If the CLion plugin does not detect or execute a custom chip correctly, use VS Code as the primary simulation environment until full JetBrains compatibility is verified.
