# Arm Phase Assembly Plan

## Purpose

This document defines a **more detailed and operational physical assembly plan** for the arm phase of `uFlex`.

Its goal is to make sure the whole team shares the same understanding of:

- what will be assembled
- how it will be assembled
- which decisions to make when doubts appear
- which things are mandatory and which are optional
- how to validate that the assembly is correct

This document complements:

- [Arm Phase Guide](arm-phase-guide.md)
- [Hardware Overview](hardware-overview.md)

---

## 1. Goal of this phase

In this phase, we are **not** building the final wearable.

We are building a **functional arm-mounted test prototype** that must satisfy these conditions:

1. The 3 IMUs must remain firmly attached to different body segments.
2. The main module must run from battery power.
3. The wiring must withstand moderate movement without coming loose.
4. The system must be easy to power on and off.
5. The assembly must be comfortable enough for short test sessions.

---

## 2. Expected outcome

At the end of the assembly, we should have:

- one **main satellite** on the forearm
- one **IMU satellite** on the biceps
- one **IMU satellite** on the back of the hand
- all 3 sensors connected and firmly mounted
- the system powering on from battery
- the firmware reading sensors and driving actuators

---

## 3. Agreed physical architecture

## 3.1 General distribution

### Main satellite: forearm

It should contain:

- ESP32
- battery
- TP4056
- MT3608
- switch
- RGB LED
- buzzer
- vibration motor driver stage
- 1 IMU
- I²C multiplexer (TCA9548A)

### Secondary satellite 1: biceps

It should contain:

- 1 IMU
- a small base

### Secondary satellite 2: back of the hand

It should contain:

- 1 IMU
- a small base

## 3.2 General principle

The main module goes on the forearm because it:

- has more space
- tolerates weight and volume better
- simplifies power and actuator integration

The other two satellites must be as light as possible.

---

## 4. Materials to be used in this phase

## 4.1 Main electronic materials

- 3× MPU9250
- I²C multiplexer (TCA9548A)
- ESP32 WROOM-32 development board
- 3.7V LiPo battery
- TP4056 with protection
- MT3608
- coin vibration motor
- 3.3V active buzzer
- RGB LED
- 2N2222
- 1K and 220 ohm resistors
- AWG 24 wires or jumpers depending on the segment
- terminal blocks if they improve order/removability

## 4.2 Main mechanical materials

- perfboards
- fabric or elastic band
- Velcro
- optionally EVA foam or thin padding
- hot glue
- electrical tape
- heat-shrink tubing

---

## 5. Design criteria that must be respected

## 5.1 Criterion 1: sensor stability

Each IMU must be:

- firm
- oriented
- free from uncontrolled rotation
- free from noticeable displacement on the arm

## 5.2 Criterion 2: minimum weight on secondary satellites

On the biceps and hand satellites:

- only the IMU
- only the necessary wiring
- a small base

## 5.3 Criterion 3: wiring robustness

No important cable should be:

- hanging loose
- pulling directly on a solder joint
- under tension across an articulation

## 5.4 Criterion 4: enough comfort for testing

We are not aiming for maximum ergonomics yet, but we do want to avoid:

- sharp points against the skin
- hard boards directly against the arm
- excessive weight on the hand or biceps

---

## 6. Assembly decisions already agreed

## 6.1 Do not use a protoboard as the main wearable structure

Protoboards are useful on the bench, not on a moving arm.

## 6.2 Use perfboard

Perfboard will be the real physical base for this phase.

## 6.3 Do minimal strategic soldering

It is not necessary to solder everything permanently, but it is worth soldering:

- power distribution
- the vibration motor stage
- the main module structural connections
- any connection that must not come loose with movement

## 6.4 Use one band + Velcro per satellite

Velcro should not be the element that presses directly on the module.  
The module should be **mounted on top of the band**, and the Velcro should be used only to **close the band around the arm**.

---

## 7. Preparation before assembly

Before soldering or gluing anything, the team should do this preparation:

1. Confirm that the current firmware still works on the bench.
2. Confirm the current pin assignments:
   - RGB: `18`, `19`, `23`
   - buzzer: `27`
   - vibration motor: `32`
3. Confirm the sensor topology used by the real firmware.
4. Confirm the final power strategy for this phase.
5. Decide which perfboard will be:
   - the main board
   - the biceps board
   - the hand board

---

## 8. Recommended power strategy

The recommendation for this phase is:

1. LiPo battery
2. TP4056 with protection
3. adjusted MT3608
4. MT3608 output into the ESP32 `VIN` or `5V`

## 8.1 Before integrating it into the system

The MT3608 must be adjusted **before** integrating it.

### What is needed

- a small screwdriver
- ideally a multimeter

### Minimum procedure

1. Connect the MT3608 input.
2. Measure the output.
3. Adjust the potentiometer to the desired value.
4. Only then connect it to the rest of the system.

## 8.2 Switch

The slider switch will be used as the main system on/off switch.

---

## 9. Assembly plan by module

## 9.1 Main forearm module

### Components that must go here

- ESP32
- battery
- TP4056
- MT3608
- switch
- RGB LED
- buzzer
- transistor + resistor for the vibration motor
- vibration motor diode
- one IMU
- I²C multiplexer (TCA9548A)
- connectors or terminal blocks for remote IMUs if they improve wiring order

> **Why the multiplexer (TCA9548A).** Each MPU9250 carries an AK8963 magnetometer
> at the fixed I²C address `0x0C`, so the 3 IMUs collide on a shared bus. The
> TCA9548A isolates each IMU on its own channel, making all three magnetometers
> readable — the prerequisite for real compensation detection. All 3 IMUs route
> through the mux on a single I²C bus (the local forearm IMU plus the two remote
> satellites over the flexible wire), instead of the two-bus topology of the
> bench prototype.

### Board size

Use a full `5×7 cm` board if needed.

### Goal of this board

It must act as the physical “center” of the prototype.

### Recommended layout

Visually separate:

- power area
- ESP32 area
- actuator area
- remote IMU connection area

### What should definitely be soldered here

- main power line
- vibration motor stage
- switch
- important distribution points

### What may remain removable

- some remote satellite connections
- actuator replacements if the team wants to iterate quickly

---

## 9.2 Biceps satellite

### Components

- 1 IMU
- a small base

### Recommendation

Do not add:

- battery
- actuators
- heavy components

### Board size

If possible, use a smaller cut than `5×7 cm`.

### Priority

- low weight
- low height
- good stability

---

## 9.3 Back-of-hand satellite

### Components

- 1 IMU
- a small base

### Priority

- it must not bother the wrist
- it must not prevent the hand from closing
- it must not pull on the cable when fingers or wrist move

### Recommendation

Make it even smaller and lighter than the biceps satellite if possible.

---

## 10. How to physically build each satellite

## 10.1 Satellite layer stack

From top to bottom:

1. components
2. board/base
3. underside with solder joints
4. intermediate insulating layer
5. EVA or foam
6. elastic band or fabric
7. arm

## 10.2 Material for the intermediate layer

It can be:

- thin plastic
- firm cardboard
- coated cardboard

Its purpose is to:

- insulate
- distribute pressure
- prevent solder joints from touching or tearing the foam

## 10.3 Use of EVA or foam

EVA or foam must not go directly against a side with exposed solder joints.  
It should be placed **after** the intermediate layer.

---

## 11. How to attach each satellite to the arm

## 11.1 Recommended method

1. prepare a fabric or elastic band
2. attach the satellite onto the band
3. use Velcro to close the band around the arm

## 11.2 What NOT to do

- do not rely on electrical tape as the main mounting method
- do not use Velcro to squeeze the module from above
- do not let the satellite hang only from cables

## 11.3 How to attach the satellite to the band

Valid options:

- hot glue
- thread or ties through the perfboard holes
- stitching onto an intermediate base
- a combination of several methods

Hot glue is valid as a mechanical aid, not as the only structural guarantee for a heavy part.

---

## 12. Wiring between satellites

## 12.1 General rules

- leave slack where there are articulations
- avoid tight cables
- secure the cable at intermediate points
- do not allow motion to pull directly on an IMU

## 12.2 What to avoid

- freely hanging cables
- excessively long unguided wires
- stiff cables near the elbow or wrist

## 12.3 Which material to use

Prefer:

- flexible AWG 24 wire

Use jumpers only if the segment is short and temporary.

---

## 13. Heat-shrink tubing and electrical tape

## 13.1 Heat-shrink tubing

Use it on:

- splices
- power lines
- vibration motor wiring
- buzzer wiring
- joints that will move

## 13.2 Electrical tape

Use it only as support for:

- temporary grouping
- local protection
- provisional holding

Do not use it as the main body-mounting mechanism.

---

## 14. Is an enclosure needed?

## 14.1 Small satellites

Not necessarily.  
In many cases, it is better not to use one in order to avoid extra bulk.

## 14.2 Main module

It may benefit from a light cover if it helps:

- protect cables
- protect the battery
- visually organize the assembly

It can be made of:

- thin plastic
- temporary cardboard

Avoid metal.

---

## 15. Recommended working sequence

## Stage 1: topology validation

1. Confirm the firmware works on the bench.
2. Confirm all 3 sensors are being read.
3. Confirm the actuators work.

## Stage 2: power

1. Prepare battery + TP4056 + MT3608 + switch.
2. Adjust MT3608 output voltage.
3. Validate stable ESP32 power-up.

## Stage 3: main module

1. Mount ESP32 and power stage on the perfboard.
2. Mount actuators.
3. Mount the main IMU.
4. Prepare outputs toward remote satellites.

## Stage 4: secondary satellites

1. Prepare the biceps satellite.
2. Prepare the hand satellite.
3. Verify both are light and stable.

## Stage 5: mechanical integration

1. Prepare the bands.
2. Build the lower layer stack.
3. Attach each satellite to its band.
4. Route and secure the wiring.

## Stage 6: arm test

1. Place the biceps satellite.
2. Place the forearm module.
3. Place the hand satellite.
4. Power on.
5. Validate readings at rest.
6. Validate readings under gentle movement.
7. Adjust mounting if there is rotation or displacement.

---

## 16. Assembly acceptance criteria

The assembly for this phase is acceptable if:

1. The system powers on from battery.
2. All 3 IMUs are read correctly.
3. The actuators respond.
4. The satellites do not visibly shift during moderate motion.
5. No cable pulls directly on a critical solder joint.
6. A user can wear it for a short test without severe discomfort.

---

## 17. Common mistakes to avoid

- placing heavy components on the biceps or hand satellites
- using too many loose jumpers
- not planning cable routing before fixing the satellites
- mounting boards with sharp points facing the skin
- relying too much on electrical tape
- not adjusting the MT3608 before connecting it
- not clearly separating the main module from the lightweight satellites

---

## 18. Final summary for the team

The shared team decision should be:

- build one **main forearm module** on perfboard
- build **two lightweight satellites** for the biceps and hand
- use **band + Velcro** for mounting
- use **insulating layer + EVA** under the boards
- use **battery + TP4056 + MT3608** for power
- accept **minimal strategic soldering**
- prioritize mechanical stability over cosmetic finish

This phase is about achieving a prototype that is:

- stable
- understandable
- repeatable
- comfortable enough

It is not yet about achieving the final wearable form.

---

## 19. Bring-up results (2026-07-02)

The physical prototype was built and validated on real hardware with a temporary
`src/main.cpp` test (the real firmware is restored afterward — see "Pending").

### What was built
- ESP32 (on female headers) + **TCA9548A mux** + 3× MPU9250 satellites
  (**ch0 = forearm, ch1 = biceps, ch2 = hand**; each `AD0→GND`, all at `0x68`,
  isolated per channel) + RGB LED + active buzzer.
- Power (separate board): LiPo 1000 mAh → TP4056-PROT → slider switch → MT3608
  (set to **~4.92 V**) → ESP32 `VIN`. Satellites wired to the main board via
  **borneras** (AWG24 silicone, 4 wires: `3V3/GND/SDA/SCL`; `AD0→GND` done locally
  at each satellite).

### Test results — PASS
- **Mux:** responds at `0x70`.
- **IMUs:** all 3 MPU9250 respond `WHO_AM_I = 0x71` per channel (stable across the loop).
- **Actuators:** RGB (R/G/B) + buzzer fire.
- **Magnetometers (bypass-per-channel):** **2 of 3 read stable at rest and respond to
  rotation** (forearm + biceps). The **3rd IMU has a dead AK8963** (`Error 263` / I²C
  timeout; the fault **follows the IMU** — confirmed by swapping it between channels — so
  it is the module, not the wiring). It sits on the **hand (ch2)**, which is not part of
  the elbow pair (elbow = biceps + forearm, both mags good) → non-critical for elbow.
- **Old blocker resolved:** the previous "AK8963 DRDY never sets / mag = 0 / Error 263"
  was the **AK8963 I²C address collision** (`0x0C` shared). The **mux resolves it** — the
  mags read once isolated per channel. (One IMU additionally has a genuinely faulty AK8963.)

### Changes vs the original plan
- **Vibration motor DROPPED** (interferes with the magnetometer + adds motion noise to the
  readings) → **`GPIO32` is now unused** and **local safety = buzzer only**.
- Power on a **separate board** (noise isolation + comfort), not all-in-one.

### Pending (not done yet)
- **Firmware mux delta:** the real firmware still uses the **two-bus** topology and does not
  match this hardware. It needs: single I²C bus (`GPIO21/22`) + **TCA9548A channel-select**
  (`write 1<<n to 0x70`) before each IMU + **bypass-per-channel** for the AK8963. Restore/keep
  the thin real `main.cpp`.
- **`GPIO32` (motor):** remove/ignore in firmware; safety alert = buzzer.
- **Dead-mag IMU:** keep on the hand (elbow-only) or replace with a 4th MPU9250 for wrist.
- **E2E compensation** (edge `CompensationDetector` on real proximal/biceps yaw) — pending the
  firmware delta. Also: battery operation and mechanical mounting (bands/Velcro/EVA).

