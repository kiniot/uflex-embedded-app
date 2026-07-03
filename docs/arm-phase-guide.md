# Arm Phase Guide

## Purpose

This document summarizes what is needed to move from the bench prototype to a **phase 2 arm-mounted test** for `uFlex`.

The focus here is not firmware or simulation, but the **physical wearable prototype**:

- which current materials are already enough
- which missing items really matter
- how to distribute the modules on the arm
- how to power the system with a battery
- when soldering is worth it and when it is not
- how to secure sensors, cables, and the main module

This guide complements:

- [Hardware Overview](hardware-overview.md)
- [Application Flow and Actuator Activation](actuator-activation-flow.md)

---

## 1. Goal of this phase

The arm phase is meant to validate:

- motion reading in a real body setup
- behavior of the 3 IMUs over real body segments
- comfort and mounting stability
- local actuator response
- battery-powered operation

This phase is **not** intended to produce a final product or an industrial enclosure yet.

---

## 2. Short conclusion

Yes, with the current materials it is possible to move into an arm-mounted test phase.

However:

- a protoboard should not be used as the main wearable solution
- there is no need to manufacture a PCB yet
- **perfboard** is strongly recommended
- **some minimal strategic soldering** is strongly recommended
- the main challenge is no longer electronic, but **mechanical and ergonomic**

---

## 3. Which current materials are already useful

With what has already been purchased, it is possible to build a functional test prototype:

- 3× MPU9250
- I²C multiplexer (TCA9548A)
- ESP32 WROOM-32 development board
- 3.7V LiPo battery
- TP4056 with protection
- MT3608 step-up converter
- coin vibration motor
- 3.3V active buzzer
- RGB LED
- 2N2222 transistor
- resistors
- flexible AWG 24 wire
- perfboards
- terminal blocks
- slider switches
- soldering iron / solder / flux
- heat-shrink tubing
- hot glue
- electrical tape

---

## 4. What is actually missing

The most relevant missing items are not about digital logic, but about **physical assembly**:

- a way to secure the device to the arm
- better mechanical strain relief for wiring
- a convenient tool to adjust the MT3608

### Priority missing items

- Velcro
- elastic band or fabric strap
- optionally EVA foam or thin padding
- a small screwdriver to adjust the MT3608

### Nice to have, but not blocking

- desoldering pump
- sandpaper
- small saw for cutting perfboards
- a light enclosure for the main module

---

## 5. Recommended physical architecture

The idea of using **3 satellites** on the arm is a good one for this phase.

### Recommended distribution

#### Satellite 1: forearm

This is the best place for the main module:

- ESP32
- battery
- charging/protection
- power stage
- actuators
- 1 IMU
- I²C multiplexer (TCA9548A)

> **Why the multiplexer (TCA9548A).** The 3 MPU9250 each carry an AK8963
> magnetometer at the fixed I²C address `0x0C`, so they collide if placed on the
> same bus. The TCA9548A isolates each IMU on its own channel, so all three
> magnetometers become readable — which is what enables real compensation
> detection (a yaw signal). All 3 IMUs route through the mux on a single I²C bus;
> the two remote satellites (biceps, hand) connect to mux channels over the
> flexible wire.

Advantages:

- more space
- better support area
- better location for the heaviest module

#### Satellite 2: biceps

This should ideally contain only:

- 1 IMU
- a small base

It should be as light as possible.

#### Satellite 3: back of the hand

This should also ideally contain only:

- 1 IMU
- a small base

It should be lightweight and minimally intrusive so it does not interfere with wrist/hand movement.

---

## 6. When to use perfboard

### Yes, perfboard is recommended

For the arm phase, perfboard is better than a protoboard because it:

- handles motion better
- reduces false contacts
- lets you secure modules and wiring
- provides a more reliable physical base

### A manufactured PCB is not needed yet

There is no need to design or order a final PCB at this point.

Perfboard is enough for this validation phase.

---

## 7. Board sizes

### 5×7 cm perfboards

The `5 cm x 7 cm` perfboards are useful, but not equally so for every module.

#### For the main module

That size is fine for:

- ESP32
- power stage
- actuators
- switch
- main wiring

#### For the biceps and back-of-hand satellites

`5×7 cm` is usually large for a single IMU.

Recommendation:

- use a smaller portion
- cut a board
- or temporarily accept the larger size if you do not want to cut anything yet

---

## 8. How to cut a perfboard

If you want to reduce the board size for the smaller satellites:

- ideally use a fine saw
- if not available, you can score along a row of holes and carefully snap it

### Practical recommendation

If you do not yet have a comfortable cutting tool:

- do not get blocked by that
- temporarily use the full board
- or get a fine metal saw blade before cutting

---

## 9. How to attach each satellite to the arm

The simplest and most reasonable solution for this phase is:

- a **small rigid base**
- an **elastic band or fabric strap**
- **Velcro** to close the strap
- optional **EVA foam or padding** for comfort

### Key idea

The satellite **should not depend on Velcro passing on top of the module**.

The better approach is:

1. secure the satellite **onto the band**
2. use the Velcro only to **close the band around the arm**

### General structure

```text
[ satellite / components ]
[ rigid base ]
[ insulating layer ]
[ EVA or foam ]
[ elastic band or fabric ]
[ arm ]
```

The Velcro closes the band; it does not need to press directly on the module.

---

## 10. How to attach the base to the band

Yes, **hot glue** can be used as a mechanical aid.

### Valid uses of hot glue

- attaching the satellite base to a piece of fabric
- reinforcing cable exits
- providing strain relief
- preventing a cable from pulling directly on a solder joint

### What hot glue should not do

- replace a reliable electrical connection
- be the only support for a heavy hanging part

If possible, combine:

- hot glue
- fixing through perfboard holes
- and/or stitching/tying to the band

---

## 11. Lower layer: why it is needed

The underside of a perfboard usually contains:

- solder joints
- trimmed leads
- wire bridges

That is why **EVA should not be placed directly against that surface** if there are exposed points or joints.

### An intermediate layer is needed

It helps to:

- electrically isolate the underside
- improve comfort
- prevent sharp points from cutting the foam or touching the skin

### Useful materials for that layer

- thin plastic
- firm cardboard
- coated cardboard
- any light rigid non-metallic sheet

---

## 12. Recommended layer stack by satellite

### Small satellite (IMU on biceps or hand)

From top to bottom:

1. IMU and connections
2. small board/base
3. underside with solder joints
4. intermediate insulating layer
5. EVA or foam
6. fabric/elastic band
7. arm

### Main module (forearm)

From top to bottom:

1. ESP32, battery, actuators, IMU
2. 5×7 perfboard
3. underside with solder joints
4. stronger intermediate insulating layer
5. EVA or foam
6. wider band
7. forearm

---

## 13. Recommended power architecture

### Recommended option with the current materials

Use:

1. 3.7V LiPo battery
2. TP4056 with protection
3. adjusted MT3608
4. MT3608 output into the ESP32 board `VIN` or `5V`

### Why use the MT3608

It is not strictly mandatory, but it is the most convenient option with the current parts:

- it stabilizes system power more reliably
- it makes more sense for powering the ESP32 development board

### What is needed to use it correctly

- a small screwdriver
- ideally a multimeter

Its output should be adjusted before connecting it to the final system.

---

## 14. Power switch

The slider switches you already have **are suitable** as the main on/off switch.

There is no need to buy a different switch type for this phase.

The switch is used to:

- interrupt or allow current to the system
- make on/off control easier without unplugging wires

---

## 15. Heat-shrink tubing: what it is for

Heat-shrink tubing is not mandatory, but it is highly recommended.

It helps:

- insulate splices
- protect solder joints
- reinforce connections
- avoid shorts
- improve mechanical reliability of wiring

### Where to use it first

- vibration motor wiring
- power wiring
- buzzer wiring
- wire splices
- any joint that will move

---

## 16. Is an enclosure needed?

### For the small satellites

It is not mandatory and, in many cases, not even desirable.

It can:

- add bulk
- add weight
- make the assembly less comfortable

### For the main module

A small lightweight enclosure or cover can make sense.

It can be:

- thin plastic
- temporary cardboard

As long as it is not metallic and does not awkwardly block the ESP32 antenna area, it should not significantly affect BLE/Wi‑Fi performance.

---

## 17. What is not recommended

- using a protoboard as the final arm-mounted structure
- carrying many loose jumpers on the body
- leaving cables hanging without strain relief
- relying only on electrical tape to secure everything
- mounting heavy or bulky IMUs on the biceps or hand
- placing the soldered underside directly against skin or foam

---

## 18. What is recommended

- use perfboard for the main module
- reduce weight in small satellites
- use one band + Velcro per satellite
- use EVA or foam for comfort
- add an intermediate insulating layer under soldered boards
- secure cables at intermediate points
- leave slack around articulated areas
- do at least some minimal strategic soldering

---

## 19. Minimum recommended shopping list

To move properly into the arm phase, the most important things to get are:

- Velcro
- elastic band or fabric strap
- optionally EVA or thin foam
- a small screwdriver for the MT3608

Desirable:

- a more robust diode for the vibration motor
- a comfortable tool for cutting perfboards

---

## 20. Final summary

Yes, with the current materials it is possible to move into an arm-mounted test phase.

The recommended strategy is:

- **3 satellites**
- **main module on the forearm**
- **lightweight IMUs on the biceps and back of the hand**
- **perfboard instead of protoboard**
- **band + Velcro for mounting**
- **insulating layer + EVA under the boards**
- **MT3608 for stable power**

The main limitation is no longer electronic, but:

- physical assembly
- comfort
- sensor stability
- cable management

That is the real priority for this phase.

