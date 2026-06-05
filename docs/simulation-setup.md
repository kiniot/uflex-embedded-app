# Hardware Simulation Guide (Wokwi Integration)

This document provides step-by-step instructions to configure and run local hardware simulations for the uFlex embedded application. This allows you to test code logic, sensor inputs, and multiplexer routing without needing physical hardware connected to your computer.

---

## 1. Shared Simulation Configuration

Both VS Code and CLion utilize the same underlying Wokwi simulation engine parameters. Before configuring your specific IDE, you must ensure that you have the following files in the project root directory (at the same level as the `platformio.ini` file).

### 1. Define the Firmware Target Map (`wokwi.toml`)
Ensure you have the following file in the project root named `wokwi.toml` with the following structural definitions to route the simulator to your compiled PlatformIO binaries:

```toml
[wokwi]
version = 1
firmware = '.pio/build/esp32dev/firmware.bin'
elf = '.pio/build/esp32dev/firmware.elf'
```

> **Important:** Make sure to **Build** the project with PlatformIO at least once before launching the simulation. If you do not compile the code first, the internal folders and the `firmware.bin` and `firmware.elf` files will not exist on the disk, meaning the simulator will not be able to recognize the paths and will throw an initialization error.

### 2. Define the Electronic Circuit Schematic (`diagram.json`)
Ensure you have the following file in the project root named `diagram.json` to map out your virtual hardware board connections.
> **Note:** You can use the same `diagram.json` file that you generate when designing your circuit visually on the [Wokwi Official Website](https://wokwi.com). Just wire the ESP32, sensors, and actuators in the web editor, click on the **diagram.json** tab within its workspace, copy the contents, and paste them directly into your local file.

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
1. Ensure your source code compiles cleanly by clicking the PlatformIO Build checkmark icon (`✓`) on the bottom toolbar.
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
1. **Mandatory Compilation Step:** Head over to the **PlatformIO** tab first and perform a **Build** of the project to generate the updated firmware in your local storage.
2. Return to the **Wokwi Simulator** tab in the right panel.
3. Click on the top **Start Simulator** button.
4. An interactive canvas featuring your virtualized electronic circuit will immediately open as a native tab inside the CLion editor, capturing the serial port data stream in real time.

   ![Running Wokwi Simulation in CLion](./assets/simulation-setup/clion-wokwi-run.png)
