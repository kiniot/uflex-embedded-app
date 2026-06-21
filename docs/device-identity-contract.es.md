# Contrato de Identidad del Dispositivo

Este documento es el **acuerdo entre repositorios** sobre cómo se identifica un
kit uFlex en toda la plataforma: firmware embebido, edge gateway, backend REST
API y la app móvil del paciente. Existe para que el mismo dispositivo físico
siempre pueda correlacionarse con su sesión de terapia y sus mediciones de
movimiento, en cada servicio y sin ambigüedad.

Es canónico para todos los repositorios `uflex-*`. Si algún repo se desvía de
este contrato, ese repo está equivocado.

---

## 1. La única fuente de verdad: `kitSerial`

Existe exactamente **un identificador de negocio** que cruza todos los
servicios: el **número de serie del kit** (`kitSerial`), un string estable y
legible como `uflex-kit-001`.

Se **acuña una sola vez** por la autoridad de provisión (el equipo/operador
hoy; un servicio de enrollment de manufactura en producción) y luego se
escribe hacia afuera a cada lugar que lo necesita. **Ningún componente de
runtime lo genera** — o lo tienen grabado al provisionar, o lo aprenden en
runtime.

### El invariante

Los siguientes valores son, en todo momento, el **mismo string**:

```
UFLEX_DEVICE_ID            (build flag del embedded)
  == Device.serialNumber     (registro de devices del backend)
  == Device.advertisedName   (lo que anuncia el firmware por BLE)
  == TherapySession.iotDeviceId  (contexto de terapia del backend)
  == device_id               (credencial / movement records del edge)
```

Si esta cadena se rompe en cualquier punto, el dispositivo físico deja de
poder atarse a su sesión o sus mediciones.

---

## 2. Modelo de identidad en dos niveles

### Nivel 1 — Identificador de negocio (cross-service)

| Identificador | Tipo | Rol |
|---------------|------|-----|
| `kitSerial` | string legible | La única identidad cross-service del dispositivo. Correlaciona firmware ↔ edge ↔ sesión de terapia ↔ registro de devices. |

### Nivel 2 — Identificadores de transporte / hardware (se quedan en su capa)

| Identificador | Tipo | Rol | ¿Cruza servicios? |
|---------------|------|-----|-------------------|
| `macAddress` | `XX:XX:XX:XX:XX:XX` | Dirección a nivel BLE. La usa la app **solo en Android** para conectar/reconectar, y se guarda en el registro del backend. **No usable en iOS** (Apple la oculta). El edge no la usa. | No |
| `advertisedName` | string | Lo que el firmware pone en el advertisement BLE. **Debe ser igual a `kitSerial`** para que la app encuentre el device asignado por nombre (funciona en iOS y Android). | No (pero es igual a `kitSerial`) |
| `DeviceId` | UUID v7 | Clave primaria surrogate interna del aggregate `Device`. **Nunca** sale del backend; nunca se usa como handle del device en embedded/edge/mobile. | No |
| `api_key` | string | Secreto compartido por kit para auth del edge (`X-API-Key`). Es una credencial, no un identificador. | No |

**La MAC nunca es la identidad cross-service.** Es inutilizable en iOS y
frágil en Android. Trátala solo como optimización opcional de reconexión en
Android y como atributo de registro/debugging.

---

## 3. Origen y flujo

### Fase 1 — Provisión (escritura única, fan-out)

La autoridad de provisión acuña `kitSerial` una vez por kit físico y lo
escribe a cuatro destinos en un mismo acto de provisión (idealmente con un
único script):

1. **Firmware** — como build flag `UFLEX_DEVICE_ID` al flashear.
2. **Backend** — `POST /api/v1/devices` con `serialNumber = kitSerial`
   (y `advertisedName = kitSerial`).
3. **Edge gateway** — una fila de credencial provisionada (`device_id =
   kitSerial` más su `api_key`).
4. **Etiqueta física** — impresa en el kit para humanos/soporte.

### Fase 2 — Runtime (nadie lo regenera)

| Componente | Cómo conoce `kitSerial` |
|------------|-------------------------|
| **Embedded** | Compilado en el binario (`UFLEX_DEVICE_ID`). Es el **emisor**: lo anuncia por BLE y lo manda en cada request al edge. |
| **Edge** | Fila provisionada en su base de datos; compara el `device_id` entrante contra ella para autenticar. |
| **Backend** | Fila `Device` registrada; expuesto como `serialNumber`, reutilizado por terapia como `iotDeviceId`. |
| **Mobile** | **Aprendido en runtime** vía `GET /api/v1/devices/my-assigned`, que devuelve el `serialNumber` del device asignado (+ `macAddress`, `advertisedName`). La app nunca lo hardcodea. |

---

## 4. Descubrimiento BLE vs. confirmación

Son dos momentos distintos y no deben confundirse.

### Descubrimiento (durante el scan, antes de conectar)

La app encuentra y elige el dispositivo usando solo lo que está en el
advertisement:

1. **Filtrar** por el **Service UUID** custom → solo aparecen kits uFlex.
2. **Elegir** el asignado por **`advertisedName` (== `kitSerial`)**.

Funciona en iOS y Android y el firmware ya lo cubre.

### Confirmación (después de conectar)

Los nombres anunciados por sí solos no son una identidad única confiable, y en
iOS la MAC no está disponible. Así que tras conectar, la app **lee una
característica con el número de serie** por GATT y verifica que sea igual al
`kitSerial` esperado. Ese es el match autoritativo.

### Superficie BLE requerida

| Elemento | UUID / valor | Propósito |
|----------|--------------|-----------|
| Service | `a1f7b2c0-3e4d-4a5b-8c6d-1f2e3a4b5c6d` | Anunciado; permite a la app filtrar kits uFlex. |
| Característica de telemetría | `a1f7b2c1-3e4d-4a5b-8c6d-1f2e3a4b5c6d` (NOTIFY) | Frame de telemetría de movimiento de 53 bytes. |
| Característica de identidad | `a1f7b2c2-3e4d-4a5b-8c6d-1f2e3a4b5c6d` (READ) | Devuelve `kitSerial` para que la app confirme la identidad. |
| Nombre anunciado | `kitSerial` | Filtro de descubrimiento; igual a `advertisedName` en el registro. |

El `DeviceId` (UUID) del backend **nunca** se expone por BLE. En la capa BLE,
la identidad del dispositivo **es** `kitSerial`.

---

## 5. Idempotencia / números de secuencia (no confundir)

Existen dos contadores separados; no están relacionados:

- **`sequenceNumber`** (`uint16`, en el frame de telemetría BLE) — ordena el
  stream en vivo para la app. Local al canal BLE.
- **`edgeSequenceId`** (UUID, `CompletedRepetition` del backend, enviado como
  header `X-Edge-Sequence-Id`) — token de idempotencia para registrar una
  repetición validada en una sesión de terapia. **Lo genera el edge gateway**
  al forwardear una repetición, no se deriva del sequence number del BLE.

---

## 6. Pendientes abiertos (no resueltos por este contrato)

Huecos conocidos que dependen de que la identidad esté cerrada primero:

1. **Propagación de sesión / serie.** Para que una medición se ate a la `Serie`
   correcta, el `serieId` activo debe llegar hasta donde se mide el ángulo (el
   "contrato de sesión edge → embedded", ver
   [`actuator-activation-flow.es.md`](actuator-activation-flow.es.md)).
2. **Forwarding edge → backend.** El edge gateway hoy es standalone. Para
   forwardear repeticiones a una sesión de terapia necesitará un lookup en el
   backend de la **sesión activa por `kitSerial`** (hoy solo existe el lookup
   por `patientId`).
3. **`macAddress` obligatoria vs opcional.** El backend hoy exige `macAddress`
   (no nula, única). Decidir si se mantiene obligatoria (registro físico
   completo, reconexión Android) o se relaja a opcional (ya que `kitSerial` es
   la identidad e iOS no puede usar la MAC).

---

## 7. Referencia rápida

| Pregunta | Respuesta |
|----------|-----------|
| ¿Qué identifica un kit en todos lados? | `kitSerial` (ej. `uflex-kit-001`) |
| ¿Quién lo genera? | La autoridad de provisión, una vez por kit |
| ¿De dónde lo saca el backend? | Lo importa al registrar (genera `DeviceId`, no `serialNumber`) |
| ¿Cómo lo conoce mobile? | `GET /api/v1/devices/my-assigned` en runtime |
| ¿Cómo encuentra mobile el device por BLE? | Scan por Service UUID, elegir por nombre anunciado (== serial) |
| ¿Cómo confirma mobile el device correcto? | Lee la característica de identidad (== serial); MAC solo como optimización Android |
| ¿El UUID del backend va por BLE / al edge? | No — solo interno del backend |
