# Environment Setup Guide

This document provides step-by-step instructions to configure your local development environment for the uFlex embedded application. Follow the steps corresponding to your operating system and preferred Integrated Development Environment (IDE).

---

## 1. System Prerequisites and Core Installation

PlatformIO Core acts as the compilation engine and requires Python to operate. Select your operating system below to install the core dependencies globally.

### macOS
1.  **Install Homebrew** (if not already installed) by running the official script in your Terminal:
    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```
2.  **Install PlatformIO Core** using Homebrew, which automatically manages the Python dependency:
    ```bash
    brew install platformio
    ```
3.  **Verify the installation** by checking the version:
    ```bash
    pio --version
    ```
    *Note: Modern macOS releases include native kernel extensions for the CP2102 USB-to-Serial chip. No additional driver installation is required.*

### Windows
1.  **Install Python 3.11+**: Download and install Python from the Microsoft Store (recommended for automatic path configuration) or the official Python website. If using the executable installer, ensure you check the box that says **"Add Python to PATH"** before proceeding.
2.  **Install PlatformIO Core**: Open PowerShell or Command Prompt as an Administrator and execute:
    ```powershell
    pip install -U platformio
    ```
3.  **Install CP210x USB to UART Bridge Driver**: To allow Windows to communicate with the ESP32 hardware via USB-C, you must manually install the Silicon Labs driver:
    > **Important Hardware Note:** This manual installation step is explicitly targeted for the **CP2102** bridge chip, which is the specific component used by our standard ESP32 model in this project. If you are working on a different repository or your hardware utilizes a different chip variant (such as the ESP32-S3 or ESP32-C3 series with native USB support, or an alternative bridge chip like the CH340), this driver will not work. In that case, you must check the model number printed physically on your board's hardware and consult its respective official documentation to install the correct driver.
    * Download the official archive directly from this link: [CP210x_Universal_Windows_Driver.zip](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip).
    * Locate the downloaded `.zip` file in your file explorer, right-click it, and select **Extract All...** to unpack the files into a standard folder.
    * Open the extracted folder, find the setup information file named **`silabser.inf`** (it has a gear or page icon and may just show up as `silabser` if file extensions are hidden).
    * Right-click on **`silabser.inf`** and select **Install** from the context menu (on Windows 11, you might need to click *Show more options* first to see the Install button). Confirm the administrator prompt to complete the process.
4.  **Verify the installation**: Close your terminal, open a new session, and verify the global path configuration:
    ```powershell
    pio --version
    ```

### Linux (Ubuntu/Debian-based)
1.  **Install Python and pip** via the package manager:
    ```bash
    sudo apt update
    sudo apt install python3 python3-pip python3-venv
    ```
2.  **Install PlatformIO Core** via pip:
    ```bash
    pip3 install -U platformio
    ```
3.  **Configure USB permissions (udev rules)** to allow your system to communicate with the ESP32 microcontroller without root privileges:
    ```bash
    curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/master/src/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
    sudo service udev restart
    sudo usermod -a -G dialout $USER
    sudo usermod -a -G plugdev $USER
    ```

---

## 2. Verifying Microcontroller Connectivity via Terminal

Before opening your IDE, it is recommended to verify that your operating system successfully recognizes the ESP32 when connected via a USB-C cable. Run the command matching your platform to confirm the hardware address binding.

### Verifying on macOS
Open your Terminal and filter for available Call-Up (cu) devices using the serial port prefix:
```bash
ls /dev/cu.usbserial*
```
**Expected Result:** The terminal should return a bound path matching your peripheral interface, such as:
```text
/dev/cu.usbserial-0001
```

### Verifying on Windows
Connect the ESP32 to your computer and execute one of the following diagnostic query lookups depending on your active shell manager:

* **Using PowerShell:**
  ```powershell
  Get-CimInstance -ClassName Win32_SerialPort | Select-Object DeviceID, Description
  ```
* **Using Command Prompt (CMD):**
  ```cmd
  wmic path Win32_SerialPort get DeviceID,Description
  ```
  **Expected Result:** The output console log map will display the dynamic assigned workspace index pathways, for example:
  ```text
  DeviceID    Description
  COM3        Silicon Labs CP210x USB to UART Bridge
  ```

### Verifying on Linux
Open your shell environment and inspect the raw kernel system log ring buffer mapping entries right after plugging in the USB-C device cable pipeline:
```bash
dmesg | grep ttyUSB
```
**Expected Result:** The system hardware logging interface will broadcast the attachment routine binding block maps, revealing the serial channel node index identifier:
```text
[ 1024.567890] usb 1-1: cp210x converter now attached to ttyUSB0
```

---

## 3. IDE Configuration

Once the PlatformIO Core and physical hardware connectivity are verified on your system, configure either VS Code or CLion.

### Option A: Visual Studio Code (VS Code)
1.  Open VS Code and navigate to the **Extensions** marketplace menu (shortcut: `Ctrl+Shift+X` or `Cmd+Shift+X`).
2.  Search for **PlatformIO IDE** and click **Install**.

    ![VS Code PlatformIO Extension](assets/env-setup/vscode-platformio-extension.png)

3.  Wait for the internal core synchronization to complete. Restart VS Code if prompted.
4.  The integration provides a dedicated PlatformIO icon in the sidebar and a bottom status bar containing execution controls for compilation (`✓`) and flashing/uploading (`→`).

### Option B: JetBrains CLion
Modern versions of CLion manage PlatformIO natively through its embedded development features.
1.  Open CLion and navigate to **Settings** (or *Preferences* on macOS) > **Plugins**.
2.  Under the **Installed** tab, verify if **PlatformIO Integration** is already active (enabled by default in modern releases under Embedded Development). If it is missing, switch to the **Marketplace** tab, search for **PlatformIO Integration**, install it, and restart the IDE.

    ![CLion PlatformIO Plugin](assets/env-setup/clion-platformio-plugin.png)

3.  Go to **Settings** > **Languages & Frameworks** > **PlatformIO**.
4.  Verify the **PlatformIO Executable Installation Path**:
    * **macOS (Homebrew):** `/opt/homebrew/bin` or `/usr/local/bin`
    * **Windows:** `C:\Users\<Your_User>\.platformio\penv\Scripts\platformio.exe` (or the respective Python scripts path).

---

## 4. Opening the Project and Project Synchronization

### In VS Code
1.  Select **File** > **Open Folder** and select the root directory containing the `platformio.ini` file.
2.  PlatformIO will automatically parse the workspace and download the required toolchains and external libraries specified under `lib_deps`.

### In CLion
1.  Select **File** > **Open** and choose the root directory of the project. Make sure to open the folder as a PlatformIO project context by choosing the `platformio.ini` config file if prompted.
2.  CLion will natively parse the `platformio.ini` configuration to map symbols and index code structures directly.
3.  It is highly recommended to configure the **Run Configuration** interface to easily switch between compilation and uploading operations. To do this:
    * Click on the execution target dropdown menu located in the top-right toolbar and select **Edit Configurations...**.
    * Ensure you have entry options mapped to the PlatformIO environment. You can verify a configuration targeting `PlatformIO Build` for checking code syntax, and another one targeting `PlatformIO Upload` to compile and flash the compiled binary into the ESP32 hardware via USB-C.
    ![CLion Run Configuration](assets/env-setup/clion-run-configuration.png)

---

## 5. Hardware Deployment Controls and Port Management

To successfully program and monitor your hardware, both IDEs provide specialized toolbar interfaces to handle environment routing and target connections.

### Managing Target Environments and Hardware Ports in VS Code
VS Code utilizes two primary configuration widgets located on the bottom toolbar status interface:

* **Switch PlatformIO Project Environment:** This selector dropdown displays your current execution profile mapped from the `platformio.ini` settings (e.g., `env:esp32dev`). If multiple development environments or target configurations exist within a repository workspace, clicking this button allows toggle manipulation to change the target framework compilation pipeline.
* **Set upload/monitor/test port:** Represented by a plug icon on the bottom tray bar interface. By default, it is set to **Auto**, forcing the ecosystem to automatically sweep available physical COM/USB pathways to find an attached microprocessor. If multiple serial nodes are online, click this button to open a selection dropdown window and specify the precise communication port line manually.

### Managing Target Environments and Hardware Ports in CLion
When writing embedded source workflows in JetBrains ecosystems, target routing and communication endpoints are handled via dedicated toolbar components:

* **PlatformIO Project Environment Selector:** Located as a standalone selection widget in the peripheral setup layout interface (displaying labels like `esp32dev`). If your configuration workspace targets different hardware configurations or builds in parallel, click this dropdown menu component to route compile signals to that specific profile context dynamically.
* **Upload port / Monitor port fields:** Within the configuration structures, the interface includes explicit **Upload port** checkboxes. Keeping the **Auto** checkbox checked lets PlatformIO locate the attached controller interface dynamically (e.g., binding sequentially to endpoints like `/dev/ttyUSB0` on Linux/macOS or `COM3` on Windows). If automated discovery routines conflict, uncheck the **Auto** checkbox and select your assigned physical interface line from the input field selection list box manually.

---

## 6. Re-indexing Dependencies and Resolving Syntax Errors

Whenever external libraries are added, modified, or removed inside the `platformio.ini` file, the IDE's internal code indexer might temporarily lose track of the header files, resulting in false syntax validation errors (e.g., red lines underlining `#include` directives).

To force the workspace to update its indexing configuration and enable autocomplete functionalities, follow the quick steps based on your active IDE:

### Resolving code indexing in VS Code
1. Open the Command Palette using the shortcut `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (macOS).
2. Type and select the following operation:
   ```text
   PlatformIO: Rebuild IntelliSense Index
   ```
3. Wait for the terminal routine to finish updating the C++ language server definitions.

### Resolving code indexing in CLion
You can trigger the synchronization using either the graphical interface (recommended) or the terminal:

* **Method 1 (Graphical Interface):** Locate the **PlatformIO** side panel tool window on the right side of CLion. At the top of the panel, click the **Reload PlatformIO Project** button (represented by an ant icon inside circular arrows).
* **Method 2 (Terminal Alternative):** Open the embedded terminal, ensure you are in the project root directory, and run:
  ```bash
  pio project init --ide clion
  ```
