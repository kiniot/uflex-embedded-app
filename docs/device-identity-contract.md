# Device Identity Contract

This document is the **cross-repository agreement** for how a uFlex kit is
identified across the whole platform: embedded firmware, edge gateway, REST
API backend, and the patient mobile app. It exists so that the same physical
device can always be correlated with its therapy session and its movement
measurements, in every service, without ambiguity.

It is canonical for all `uflex-*` repositories. If any repo diverges from this
contract, that repo is wrong.

---

## 1. The single source of truth: `kitSerial`

There is exactly **one business identifier** that crosses every service: the
**kit serial number** (`kitSerial`), a stable, human-readable string such as
`uflex-kit-001`.

It is **minted once** by the provisioning authority (the team/operator today;
a manufacturing enrollment service in production) and then written outward to
every place that needs it. **No runtime component ever generates it** — they
either have it baked in at provisioning time, or they learn it at runtime.

### The invariant

The following values are, at all times, the **same string**:

```
UFLEX_SERIAL_NUMBER        (embedded #define; value of the BLE serial characteristic)
  == Device.serialNumber     (backend device registry)
  == TherapySession.iotDeviceId  (backend therapy context)
  == device_id               (edge gateway credential / movement records)
```

If this chain breaks at any point, the physical device can no longer be tied
to its session or its measurements.

Note that `advertisedName` (the BLE broadcast name) is **not** part of this
identity chain — it is a transport detail that may differ from the serial (see
§2 and §4).

---

## 2. Two-tier identity model

### Tier 1 — Business identifier (cross-service)

| Identifier | Type | Role |
|------------|------|------|
| `kitSerial` | human-readable string | The single cross-service device identity. Correlates firmware ↔ edge ↔ therapy session ↔ device registry. |

### Tier 2 — Transport / hardware identifiers (stay in their own layer)

| Identifier | Type | Role | Crosses services? |
|------------|------|------|-------------------|
| `macAddress` | `XX:XX:XX:XX:XX:XX` | BLE-level address. Used by the mobile app **on Android only** to connect/reconnect, and stored in the backend registry. **Not usable on iOS** (Apple hides it). The edge does not use it. | No |
| `advertisedName` | string (≤ 26 chars) | The BLE broadcast name (firmware `UFLEX_BLE_ADVERTISED_NAME`). **Transport only — may differ from `kitSerial`.** Discovery is by service UUID, not by name, so the name is not load-bearing; it is registry metadata with a BLE-size limit. | No |
| `DeviceId` | UUID v7 | Backend-internal surrogate primary key for the `Device` aggregate. **Never** leaves the backend; never used as the device handle by embedded/edge/mobile. | No |
| `api_key` | string | Per-kit shared secret for edge authentication (`X-API-Key`). A credential, not an identifier. | No |

**The MAC address is never the cross-service identity.** It is unusable on
iOS and fragile on Android. Treat it as an optional Android reconnect
optimization and a registry/debugging attribute only.

---

## 3. Provenance and flow

### Phase 1 — Provisioning (write-once fan-out)

The provisioning authority mints `kitSerial` once per physical kit and writes
it to four sinks in a single provisioning action (ideally via one script):

1. **Firmware** — as the `UFLEX_SERIAL_NUMBER` `#define` at flash time. The BLE
   broadcast name is a separate `UFLEX_BLE_ADVERTISED_NAME` `#define`.
2. **Backend** — `POST /api/v1/devices` with `serialNumber = kitSerial`.
   `advertisedName` is optional and defaults to the serial when omitted; it may
   be set to a different (≤ 26-char) value.
3. **Edge gateway** — a provisioned credential row (`device_id = kitSerial`
   plus its `api_key`).
4. **Physical label** — printed on the kit for humans/support.

### Phase 2 — Runtime (nobody regenerates it)

| Component | How it knows `kitSerial` |
|-----------|--------------------------|
| **Embedded** | Compiled into the binary (`UFLEX_SERIAL_NUMBER`). It is the **emitter**: it exposes it on the BLE serial characteristic and sends it on every edge request. |
| **Edge** | Provisioned row in its database; matches the incoming `device_id` against it to authenticate. |
| **Backend** | Registered `Device` row; exposed as `serialNumber`, reused by therapy as `iotDeviceId`. |
| **Mobile** | **Learned at runtime** via `GET /api/v1/devices/my-assigned`, which returns the assigned device's `serialNumber` (+ `macAddress`, `advertisedName`). The app never hardcodes it. |

---

## 4. BLE discovery vs. confirmation

These are two distinct moments and must not be conflated.

### Discovery (during the scan, before connecting)

The mobile app finds the device by the custom **Service UUID** in the
advertisement → only uFlex kits appear. The service UUID is the reliable filter
because the device name often does not fit alongside the 128-bit service UUID in
the 31-byte advertisement. The advertised name is only an optional fallback; the
authoritative selection happens at confirmation (below).

This works on both iOS and Android.

### Confirmation (after connecting)

Advertised names alone are not a trustworthy unique identity, and on iOS the
MAC is not available. So after connecting, the app **reads a serial-number
characteristic** over GATT and checks it equals the expected `kitSerial`. This
is the authoritative match.

### Required BLE surface

| Element | UUID / value | Purpose |
|---------|--------------|---------|
| Service | `a1f7b2c0-3e4d-4a5b-8c6d-1f2e3a4b5c6d` | Advertised; lets the app filter for uFlex kits. |
| Telemetry characteristic | `a1f7b2c1-3e4d-4a5b-8c6d-1f2e3a4b5c6d` (NOTIFY) | 53-byte motion telemetry frame. |
| Identity characteristic | `a1f7b2c2-3e4d-4a5b-8c6d-1f2e3a4b5c6d` (READ) | Returns `kitSerial` so the app can confirm device identity. |
| Advertised name | `UFLEX_BLE_ADVERTISED_NAME` | Transport only; optional fallback selector, not the discovery filter. May differ from `kitSerial`. |

The backend's `DeviceId` (UUID) is **never** exposed over BLE. At the BLE
layer, the device identity **is** `kitSerial`.

---

## 5. Idempotency / sequence numbers (do not conflate)

Two separate counters exist; they are unrelated:

- **`sequenceNumber`** (`uint16`, in the BLE telemetry frame) — orders the live
  stream for the mobile app. Local to the BLE channel.
- **`edgeSequenceId`** (UUID, backend `CompletedRepetition`, sent as the
  `X-Edge-Sequence-Id` header) — idempotency token for recording a validated
  repetition into a therapy session. **Generated by the edge gateway** when it
  forwards a repetition, not derived from the BLE sequence number.

---

## 6. Open items (not resolved by this contract)

These are known gaps that depend on identity being settled first:

1. **Session / serie propagation.** For a measurement to be tied to the right
   `Serie`, the active `serieId` must reach the point where the angle is
   measured (the "edge → embedded session contract", see
   [`actuator-activation-flow.md`](actuator-activation-flow.md)).
2. **Edge → backend forwarding.** The edge gateway is currently standalone. To
   forward repetitions to a therapy session it will need a backend lookup of
   the **active session by `kitSerial`** (today only lookup-by-`patientId`
   exists).
3. **`macAddress` mandatory vs optional.** The backend currently requires
   `macAddress` (non-null, unique). Decide whether to keep it mandatory (full
   physical registry, Android reconnect) or relax it to optional (since
   `kitSerial` is the identity and iOS cannot use the MAC).

---

## 7. Quick reference

| Question | Answer |
|----------|--------|
| What identifies a kit everywhere? | `kitSerial` (e.g. `uflex-kit-001`) |
| Who generates it? | The provisioning authority, once per kit |
| Where does the backend get it? | Imported at registration (it generates `DeviceId`, not `serialNumber`) |
| How does mobile know it? | `GET /api/v1/devices/my-assigned` at runtime |
| How does mobile find the device over BLE? | Scan by Service UUID (advertised name is only a fallback) |
| How does mobile confirm the right device? | Read the identity characteristic (== serial); MAC only as Android optimization |
| Is `advertisedName` part of identity? | No — transport only, may differ from the serial (≤ 26 chars) |
| Does the backend UUID go over BLE / to the edge? | No — backend-internal only |
