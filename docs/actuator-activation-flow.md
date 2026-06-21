# Application Flow and Actuator Activation

## Purpose

This document summarizes the main findings and architectural decisions discussed so far to answer one of the project's central questions:

**How do actuators know when to activate and under which parameters?**

The goal is to leave a clear reference for the team, especially since we are still grounding the operational architecture of the full system (`embedded`, `mobile`, `edge`, `backend`).

---

## 1. Main Problem

The `uFlex` project does not only need to read sensors and transmit data. It also needs to decide:

- when to activate local actuators
- which parameters or thresholds to use
- which node in the system decides each thing
- how `embedded`, `mobile`, `edge`, and `backend` coordinate with each other

The current firmware actuators are:

- `RgbLed`
- `ActiveBuzzer`
- `VibrationMotor`

All of them have already been modeled as domain actuators and applied to the environment through shared GPIO adapters.

---

## 2. Main Findings

### 2.1 Real-time actuators should not depend on the backend

If the device needs to vibrate, beep, or light up an LED because the patient went out of range or exceeded a threshold, that decision should not wait for a remote response.

That implies:

- not depending on the backend to react
- not depending on internet connectivity for immediate feedback
- avoiding long chains such as `embedded -> edge -> backend -> edge -> embedded` to activate actuators

### 2.2 The embedded device does need to know the active configuration

The `embedded` device does not need to be the source of truth for configuration, but it does need to know the active configuration during a session.

That includes, for example:

- minimum and maximum ROM
- warning margins
- feedback mode
- active exercise or profile

This configuration can initially live in RAM. If the device resets, another node in the system can send it again.

### 2.3 The backend should not execute immediate reaction logic

The backend should still be the global source of truth for:

- clinical parameters
- exercise profiles
- patient-specific configuration
- global system rules

But it is not the right place to trigger local actuators in real time.

### 2.4 The edge should know and persist the configuration

Since the edge will have SQLite, it is a strong candidate to act as the local coordinator:

- it keeps the current local configuration
- stores sessions
- persists samples
- resynchronizes with the backend
- resends configuration to the embedded device when needed

### 2.5 The mobile app does not necessarily need to send anything to the embedded device

It is not mandatory for the mobile app to directly control the embedded device.

For this version, the recommendation discussed is:

- the mobile app talks to the backend
- the edge coordinates locally with the embedded device
- the embedded device streams data to the mobile app over BLE

---

## 3. Recommended Flow for This Version

### 3.1 Session control flow

For this version, the recommended flow is:

1. `mobile -> backend` to start or end a session
2. `backend -> edge` to reflect the current session/configuration
3. `edge -> embedded` to send:
   - `startSession`
   - `stopSession`
   - thresholds
   - clinical parameters
   - feedback configuration

This means that we are **not using** `mobile -> edge -> embedded` as the recommended start/stop flow for this version of the document.

### 3.2 Real-time data flow

The recommended flow for immediate telemetry is:

1. `embedded -> mobile` over BLE
2. `embedded -> edge` over Wi-Fi/HTTP, ideally in batches or at a lower frequency
3. `edge -> backend` in an aggregated or periodic way

### 3.3 Actuator activation flow

The recommended flow is:

1. the `backend` defines the clinical parameters
2. the `edge` receives them, stores them in SQLite, and distributes them
3. the `embedded` device receives and keeps the active configuration in RAM
4. the `embedded` device evaluates the movement state in real time
5. the `embedded` device activates actuators locally when appropriate

In other words:

- the backend defines the policy
- the edge distributes and persists it
- the embedded device executes it in real time

---

## 4. What Each Node Needs to Know

### 4.1 Embedded

It must know:

- active session configuration
- thresholds/ROM/margins
- current movement state
- local immediate-reaction logic

It should not be the source of truth for persistent clinical rules.

It must be able to:

- measure sensors
- compute base metrics
- decide local feedback
- transmit data

### 4.2 Mobile

It must know:

- session state
- real-time telemetry
- enough information to present the user experience

For this version:

- it starts/ends sessions through the backend
- it receives real-time data from the embedded device over BLE

### 4.3 Edge

It must know:

- current configuration
- local session state
- data received from the embedded device
- backend sync state

It must be able to:

- store data in SQLite
- resend configuration to the embedded device
- store batches
- sync with the backend

### 4.4 Backend

It must know:

- clinical rules
- patient parameters
- global configuration
- central session history

It should act as:

- the global source of truth
- the business coordinator

---

## 5. Recommendation on Sensor Processing

### 5.1 What the embedded device does today

Today the firmware:

- reads 3 IMUs
- computes relative angles between segments
- exposes `pitchDeg` and `rollDeg`

At the moment these angles are mainly derived from the accelerometer.

### 5.2 What is recommended in the future

It is recommended that the `embedded` device be the node responsible for the base orientation processing, not the edge.

That means that, in the future, the embedded device should:

- read accelerometer + gyroscope + magnetometer
- apply basic calibration
- apply sensor fusion
- produce useful orientation outputs
- derive metrics such as:
  - Euler angles
  - quaternions
  - relative angles between segments

### 5.3 Why not leave this to the edge

Because the embedded device is the only node that:

- has the raw samples in real time
- controls actuators
- needs to react with minimal latency

The edge can enrich, store, or summarize the data, but it should not be the primary node responsible for orientation computation if we want robust local feedback.

---

## 6. Payloads and Transmission Cadence

### 6.1 Embedded -> Mobile over BLE

Goal:

- immediate feedback
- real-time visualization

Recommendation:

- lightweight payload
- high frequency
- already processed metrics

### 6.2 Embedded -> Edge over Wi-Fi/HTTP

Goal:

- local persistence
- consolidation
- later synchronization

Recommendation:

- batch delivery or longer intervals
- not necessarily every individual sample

### 6.3 Edge -> Backend

Goal:

- central history
- resilience against network failures
- overhead reduction

Recommendation:

- batch synchronization
- periodic sync
- immediate transmission only for important events if that really adds value

---

## 7. Ideal Flows

### 7.1 Ideal recommended flow for this version

1. `mobile -> backend`
   - start session
   - end session
2. `backend -> edge`
   - active configuration
   - session state
3. `edge -> embedded`
   - `startSession`
   - `stopSession`
   - thresholds
   - exercise profile
4. `embedded -> mobile`
   - real-time BLE stream
5. `embedded -> edge`
   - samples or batches
6. `edge -> backend`
   - synchronization

### 7.2 Ideal actuator activation flow

1. the embedded device receives the active configuration
2. the embedded device computes movement in real time
3. the embedded device compares it against local thresholds
4. the embedded device decides whether to activate:
   - LED
   - buzzer
   - vibration

---

## 8. Alternative Flows Considered

### 8.1 Alternative: `mobile -> embedded`

Advantages:

- less latency for direct control
- useful for MVPs or debugging

Disadvantages:

- tighter coupling between mobile app and firmware
- less centralized local coordination
- makes the operational source of truth harder to define

### 8.2 Alternative: `mobile -> edge -> embedded`

It was considered as an interesting local operational option, especially for start/stop, but **it is not the recommended flow for this version of the document**.

### 8.3 Alternative: `embedded -> edge -> mobile`

It is not recommended as the main real-time flow because it puts the edge in the lowest-latency path and complicates the immediate user experience.

---

## 9. Architectural Recommendations

### 9.1 General recommendation

Keep this conceptual separation:

- `domain/`: problem rules and actuators
- `application/`: session policy, timing, coordination
- `infrastructure/`: sensors, physical actuators, transport, input/output channels

### 9.2 Recommendation on external inputs to the embedded device

If the embedded device is going to receive configuration or commands from outside, that should be modeled as input adapters in infrastructure.

Future examples:

- BLE configuration adapter
- HTTP/Wi‑Fi configuration adapter
- session command adapter

### 9.3 Recommendation on embedded memory

For a first version:

- keeping the active configuration in RAM is enough
- if the embedded device resets, the edge can resend it

In the future:

- local persistence using ESP32 NVS/flash could be evaluated

---

## 10. Short Answer to the Central Question

### How do actuators know when to activate and under which parameters?

Actuators should activate when the `embedded` device, using the previously received active configuration, detects in real time that a relevant local condition has been met.

That implies:

- clinical parameters originate in the backend
- the edge receives, persists, and distributes them
- the embedded device keeps them active in memory during the session
- the embedded device applies those parameters locally to the measured movement
- the embedded device decides when to activate actuators

In one sentence:

**The backend defines, the edge distributes and persists, and the embedded device executes in real time.**

---

## 11. Recommended Next Steps

1. Define a formal active-configuration structure for the embedded device.
2. Define the `edge -> embedded` message contract.
3. ~~Define the minimum BLE payload toward the mobile app.~~ Done — see section 12.
4. Define the Wi‑Fi/HTTP payload toward the edge.
5. Replace the current actuator test behaviors with real clinical or functional criteria.

---

## 12. BLE Telemetry Contract (Implemented)

This section documents the `embedded -> mobile` BLE payload referenced in section 6.1, now implemented in `BleTelemetryServer` (NimBLE-based GATT server) and `BleMotionTelemetrySerializer`.

### 12.1 Why a separate contract from the HTTP payload

`MotionPayload` (the HTTP payload toward the edge, section 6.2) only carries pitch/roll deltas derived from the accelerometer. The mobile app needs full 3-axis relative rotation per joint to drive an avatar that can show motion toward/away from the camera, not just up/down — so BLE telemetry carries quaternions instead, fused from accelerometer + gyroscope + magnetometer through `OrientationFilter` (a Mahony-style explicit complementary filter) and reduced to per-joint relative rotation through `RelativeRotationCalculator`.

### 12.2 GATT profile

- Custom 128-bit Service UUID and Notify Characteristic UUID (see `BleTelemetryServer`); not a SIG-registered profile, since this is private data between our own firmware and our own app.
- One characteristic, `NOTIFY` only — the mobile app never writes to the embedded over BLE in this version (consistent with section 8.1: `mobile -> embedded` control was explicitly not adopted for this version).
- Advertised device name: `UFLEX_DEVICE_ID` (currently a compile-time constant; see open question in 12.5 about aligning it with the backend's `Device.advertisedName`).
- Preferred MTU: 256 bytes, negotiated on connect, so the full payload fits in a single notification instead of fragmenting across three ATT packets.

### 12.3 Wire format (53 bytes, little-endian)

| Offset | Field | Type |
|---|---|---|
| 0  | upperMiddleRotation.w | float32 |
| 4  | upperMiddleRotation.x | float32 |
| 8  | upperMiddleRotation.y | float32 |
| 12 | upperMiddleRotation.z | float32 |
| 16 | middleLowerRotation.w | float32 |
| 20 | middleLowerRotation.x | float32 |
| 24 | middleLowerRotation.y | float32 |
| 28 | middleLowerRotation.z | float32 |
| 32 | upperLowerRotation.w | float32 |
| 36 | upperLowerRotation.x | float32 |
| 40 | upperLowerRotation.y | float32 |
| 44 | upperLowerRotation.z | float32 |
| 48 | ledColor | uint8 |
| 49 | buzzerActive | uint8 (0/1) |
| 50 | vibrationActive | uint8 (0/1) |
| 51-52 | sequenceNumber | uint16 |

Fields are written explicitly byte-by-byte (`BleMotionTelemetrySerializer`) rather than copying the raw C++ struct's memory, so the layout is guaranteed regardless of compiler padding/alignment differences between the ESP32 firmware and the mobile app. Both platforms are little-endian, so IEEE-754 floats need no byte-swapping.

`sequenceNumber` is a monotonic counter of produced frames (incremented every read cycle regardless of whether a mobile client is connected), so the app can detect dropped notifications.

### 12.4 Cadence

The embedded read loop (and therefore the BLE publish rate) runs at ~25Hz (`UflexApplication::READ_INTERVAL_MS = 40`), matching the "high frequency" goal from section 6.1. The HTTP channel toward the edge keeps its own, much slower `EDGE_PUBLISH_INTERVAL_MS` (5s), unaffected by this change.

### 12.5 Known simplifications / open items

- **No `sessionActive` field yet.** The embedded has no session/configuration concept to report until next-step #1/#2 above are implemented. Add it to the payload once that contract exists.
- **No BLE security/pairing.** Open, unencrypted notifications for this MVP; revisit if this becomes more than an academic prototype.
- **Wokwi does not simulate Bluetooth.** `BleTelemetryServer` only backs `esp32_hw`; `esp32_sim` uses `NoOpBleTransport`, so this channel can only be exercised on real hardware.
- **Advertised name is a static compile-time constant.** The backend's `Device` aggregate already models `advertisedName`/`macAddress` fields meant to correlate a registered device with its BLE peripheral; if this project scales beyond one prototype kit, the advertised name should come from provisioned configuration instead of a hardcoded constant.

