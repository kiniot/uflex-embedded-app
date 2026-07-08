# Diseño de conexionado y alimentación — fase de brazo

Este documento es el complemento **eléctrico** del armado de la fase de brazo: el diseño de
conexionado final (cadena de alimentación, multiplexor I²C TCA9548A, sensores, actuadores) y
una guía paso a paso de cableado para llevar el prototipo de banco a un dispositivo con batería
y 3 satélites montado en el brazo.

Complementa los docs mecánicos/ergonómicos y el de sensado:

- [Resumen de Hardware](hardware-overview.md) — topología de sensores y pipeline de movimiento
- [Guía de la Fase de Brazo](arm-phase-guide.es.md) — materiales y estrategia física
- [Plan de Ensamblaje de la Fase de Brazo](arm-phase-assembly-plan.es.md) — secuencia de montaje
- [Flujo de Activación de Actuadores](actuator-activation-flow.es.md) — cuándo/por qué disparan

> **Alcance.** Este es un documento de **diseño + cableado**. **No** cambia el firmware. El
> firmware hoy usa una topología de dos buses; pasar al mux requiere un cambio de firmware que
> se registra en [§9](#9-delta-de-firmware-fuera-de-alcance-aquí).

---

## 1. Diagrama del sistema

![Conexionado del sistema uFlex — fase de brazo](assets/arm-phase-wiring/system-diagram.svg)

> Fuente editable: [`assets/arm-phase-wiring/system-diagram.puml`](assets/arm-phase-wiring/system-diagram.puml)

Vista de bloques:

```text
                       ┌──────────────────────────── riel 3V3 ───────────────────────────┐
                       │              (regulador a bordo del ESP32)                        │
                       ▼                          ▼                ▼                       ▼
 LiPo ─▶ TP4056 ─▶ SW ─▶ MT3608 ─▶ ESP32 ─ I²C ─▶ TCA9548A ─┬─ ch0 ─▶ MPU9250 #1 (antebrazo)
 3.7V    -PROT          (5.0 V)    (21/22)   (0x70)         ├─ ch1 ─▶ MPU9250 #2 (bíceps)
 1000mAh                                                    └─ ch2 ─▶ MPU9250 #3 (mano)
                          │
                          ├─ GPIO32 ─▶ motor vibrador (2N2222 + 1N4148)
                          ├─ GPIO27 ─▶ buzzer activo   (2N2222)
                          └─ GPIO18/19/23 ─▶ LED RGB   (220 Ω c/u)
```

---

## 2. Mapa de pines (ground truth del firmware)

Estos son los pines definidos en el hardware runtime del firmware
(`lib/uflex/include/uflex/application/runtime/hw_uflex_runtime.h`). El cableado **debe**
coincidir con ellos — no inventar pines.

| Función | GPIO | Notas |
| --- | --- | --- |
| I²C SDA (primario → upstream del mux) | `21` | bus único al TCA9548A |
| I²C SCL (primario → upstream del mux) | `22` | bus único al TCA9548A |
| RGB Rojo / Verde / Azul | `18` / `19` / `23` | cátodo común, 220 Ω por canal |
| Buzzer activo (3.3 V) | `27` | vía 2N2222 |
| Motor vibrador | `32` | vía 2N2222 + 1N4148 (flyback) |
| (I²C secundario, bus viejo) | `25` / `26` | **liberados** — el mux reemplaza el 2º bus |

> El `diagram.json` de banco (Wokwi) y el firmware actual usan **dos** buses I²C. El diseño de
> la fase de brazo enruta los 3 IMUs por el mux en el **único** bus primario (21/22), lo que
> libera `GPIO25/26`.

---

## 3. Topología I²C con el TCA9548A

```text
ESP32 GPIO21(SDA)/22(SCL) ──▶ TCA9548A upstream (SDA/SCL)   addr 0x70
                              ├── SD0/SC0 ──▶ MPU9250 #1  (antebrazo, local)
                              ├── SD1/SC1 ──▶ MPU9250 #2  (bíceps, remoto)
                              └── SD2/SC2 ──▶ MPU9250 #3  (mano, remoto)
```

**Cableado del mux**

| Pin TCA9548A | Conectar a | Por qué |
| --- | --- | --- |
| `VIN` | riel 3V3 | lógica + alimentación del bus |
| `GND` | GND (común) | |
| `SDA` / `SCL` | `GPIO21` / `GPIO22` | bus upstream desde el ESP32 |
| `A0` / `A1` / `A2` | GND | fija la dirección I²C **0x70** |
| `RST` | 3V3 (o un GPIO libre) | fuera de reset; GPIO solo si quieres reset por SW |
| `SC0..2` / `SD0..2` | una MPU9250 cada uno | canales downstream |

**Cada MPU9250**: `VCC → 3V3`, `GND → GND`, `AD0 → GND`. Con el mux, las tres quedan en la misma
dirección `0x68` porque solo un canal está seleccionado a la vez.

**Por qué el mux.** Cada MPU9250 lleva un magnetómetro AK8963 en la dirección I²C fija `0x0C`,
así que los tres colisionan en un bus compartido (y la contención multi-master en master mode
hace que la 2ª IMU del bus falle el init). El TCA9548A aísla cada IMU en su propio canal, de modo
que **los tres magnetómetros se vuelven legibles** — requisito para la detección de compensación
con señal real (un valor de yaw). Ver la nota del blocker en
[Resumen de Hardware](hardware-overview.md#status--notes).

**Nota de cable largo.** Los IMUs de bíceps y mano quedan a ~30–40 cm por cable flexible. Correr
I²C a **100 kHz**. Los módulos MPU9250 traen pull-ups de ~4.7 kΩ, que ahora quedan **downstream
por canal** (bien — el mux aísla la capacitancia de cada segmento). Verificar que el breakout del
mux tenga pull-ups **upstream** en SDA/SCL (la mayoría de placas HW-617 los traen); si no, añadir
~4.7 kΩ en el bus upstream.

---

## 4. Cadena de alimentación (batería 1000 mAh)

```text
LiPo 3.7 V 1000 mAh ──▶ TP4056-PROT (B+/B-)
TP4056-PROT OUT+ ──▶ slider switch ──▶ MT3608 IN+
TP4056-PROT OUT- ───────────────────▶ MT3608 IN-
MT3608 OUT+ (ajustado a 5.0 V) ──▶ ESP32 "5V" / "VIN"
MT3608 OUT- ──▶ ESP32 GND
ESP32 3V3 (regulador a bordo) ──▶ TCA9548A + 3× MPU9250 + lado alto de actuadores
```

- **Usar el TP4056-PROT** (el que trae protección de batería), no el TP4056 simple.
- **El AMS1117-3.3 no se usa** como alimentación principal: su dropout de ~1.1 V no puede regular
  una batería de 3.7 V (necesitaría ≥4.4 V de entrada). El regulador a bordo del ESP32 ya da 3V3.
- **Estimación de autonomía (1000 mAh):** ~**4–6 h** típica, ~3 h con Wi-Fi intenso
  (≈120 mA @ 5 V → ≈0.19 A de la batería → ≈5 h). Cómodo para sesiones de prueba.
- **Desacople:** un electrolítico de **100 µF** en el riel de 5 V cerca del ESP32 y uno de
  **10 µF** en 3V3 suavizan los picos de corriente del Wi-Fi. Ambos están en el inventario.

---

## 5. Sub-circuitos de los actuadores

Todos los actuadores van en la placa principal del **antebrazo**.

**Motor vibrador** — GPIO32

```text
GPIO32 ──[1 kΩ]── B
                  │ 2N2222 (NPN)
              C ──┴── motor(−)        motor(+) ── 3V3
              E ───── GND
       1N4148 sobre el motor: cátodo → 3V3, ánodo → colector  (flyback)
```

**Buzzer activo (3.3 V)** — GPIO27

```text
GPIO27 ──[1 kΩ]── B
                  │ 2N2222 (NPN)
              C ──┴── buzzer(−)       buzzer(+) ── 3V3
              E ───── GND
```

Vía transistor porque un buzzer activo tira ~30 mA (> los ~20 mA recomendados por GPIO).

**LED RGB (cátodo común)** — GPIO18/19/23

```text
GPIO18 ──[220 Ω]── R ┐
GPIO19 ──[220 Ω]── G ├─ LED RGB   COM ── GND
GPIO23 ──[220 Ω]── B ┘
```

Disparo directo (cada canal < 20 mA), sin transistor.

**Chequeo de inventario:** 2× 2N2222, 2× 1 kΩ, 2× 1N4148 (1 para el motor + 1 repuesto), 3× 220 Ω — encaja justo. ✔

---

## 6. Cableado entre satélites

Cada satélite **remoto** (bíceps, mano) se conecta a su canal del mux con **4 conductores** en
cable de silicona flexible AWG24:

| Cable | Señal | Color sugerido |
| --- | --- | --- |
| 1 | `3V3` | rojo |
| 2 | `GND` | negro |
| 3 | `SDx` (SDA del canal) | azul |
| 4 | `SCx` (SCL del canal) | (4º color) |

Dejar holgura en codo y muñeca; agregar strain relief para que el movimiento nunca tire de una
soldadura ni del IMU. El montaje mecánico (bandas, EVA, capas) está en el
[Plan de Ensamblaje de la Fase de Brazo](arm-phase-assembly-plan.es.md).

---

## 7. Advertencias de seguridad

1. **Ajustar el MT3608 a 5.0 V con multímetro ANTES de conectar el ESP32.** Sale cerca de su
   máximo; baja el trimpot, mide, fija 5.0 V y *recién ahí* conéctalo. Un MT3608 sin ajustar
   destruye el ESP32.
2. **No alimentar por USB y batería-5V a la vez.** Los devkits baratos unen el 5V del USB con el
   pin 5V sin diodo → back-feed. USB para flashear, batería para correr.
3. **GND común** en todo (batería, boost, ESP32, mux, actuadores).
4. **Verificar la polaridad** de la batería en `B+/B-` del TP4056.

---

## 8. Guía paso a paso de cableado

Esta es la secuencia **eléctrica**; combínala con la secuencia mecánica del
[Plan de Ensamblaje](arm-phase-assembly-plan.es.md).

**Etapa 0 — Línea base en banco**
1. Confirmar que el firmware actual lee los IMUs y dispara los actuadores en el banco.

**Etapa 1 — Etapa de potencia**
1. Cablear LiPo → TP4056-PROT → slider switch → MT3608.
2. Con el MT3608 **desconectado del ESP32**, aliméntalo y ajusta la salida a **5.0 V**.
3. Conectar MT3608 5 V → ESP32 `5V/VIN`; confirmar que el ESP32 arranca con batería.

**Etapa 2 — Mux + IMU local**
1. Cablear el TCA9548A: `VIN→3V3`, `GND→GND`, `SDA→GPIO21`, `SCL→GPIO22`, `RST→3V3`, `A0/A1/A2→GND`.
2. Cablear el MPU9250 del antebrazo al canal 0 (`SD0/SC0`), `AD0→GND`.

**Etapa 3 — Actuadores**
1. Armar el driver del motor (con el flyback 1N4148), el driver del buzzer y las resistencias del
   RGB según [§5](#5-sub-circuitos-de-los-actuadores).

**Etapa 4 — Satélites remotos**
1. Tender 4 hilos de silicona al IMU del bíceps → canal 1, y al IMU de la mano → canal 2.

**Etapa 5 — Chequeos de encendido**
1. I²C scan **por canal** (seleccionar canal del 0x70 y escanear): cada canal muestra un dispositivo en `0x68`.
2. Leer el magnetómetro por canal (`HX/HY/HZ` no-cero).
3. Disparar cada actuador (pulso de motor, pulso de buzzer, colores del RGB).

---

## 9. Delta de firmware (fuera de alcance aquí)

Este documento describe el **cableado objetivo**. El firmware **todavía no** maneja el mux:

- Hoy: dos buses I²C (`Wire` 21/22, `Wire1` 26/25), AK8963 leído en **I²C master mode**.
- Objetivo: un solo bus a través del TCA9548A, **seleccionando el canal** (`escribir 1<<n a 0x70`)
  antes de hablar con cada IMU. Con el mux aislando cada AK8963, la lectura del magnetómetro puede
  volver a **bypass por canal** en lugar de master mode.

Ese cambio de firmware es un follow-up aparte y **no** forma parte de este documento de diseño.
