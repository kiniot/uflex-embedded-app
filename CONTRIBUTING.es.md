# Guía de Colaboración

Idiomas disponibles:
- [Inglés](CONTRIBUTING.md)
- [Español](CONTRIBUTING.es.md)

Este repositorio contiene el **firmware embebido de uFlex** basado en **ESP32**, **PlatformIO** y una arquitectura inspirada en **ModestIoT**.  
Esta guía define cómo colaborar en el proyecto de forma consistente a nivel de estructura, estilo y flujo de trabajo.

---

## 1) Introducción rápida

### Requisitos
- **Python 3.11+**
- **PlatformIO Core**
- Un IDE compatible como **VS Code** o **CLion**
- Para simulación: entorno configurado con **Wokwi**
- Para hardware real: una placa **ESP32** y, cuando corresponda, los sensores físicos

### Clonar el repositorio
```bash
git clone <REPOSITORY-URL>
cd <REPO-NAME>
```

### Configurar tu usuario de Git
```bash
git config --global user.name "Tu Nombre"
git config --global user.email "tu.email@ejemplo.com"
```

### Compilar el proyecto
Este proyecto usa múltiples environments de PlatformIO:
- `esp32_sim` para simulación
- `esp32_hw` para hardware real

```bash
# Compilar simulación
pio run -e esp32_sim

# Compilar hardware
pio run -e esp32_hw
```

> Si es tu primera contribución, crea primero una rama de trabajo antes de modificar código.

---

## 2) Workflow de Git

- Repositorio central: **GitHub**
- Rama de trabajo e integración: `develop`
- Rama de versiones finales estables: `main`
- Las nuevas funcionalidades y refactors deben desarrollarse en ramas de trabajo creadas desde `develop`

### Convención de ramas
- `feature/<tema>` para nuevas funcionalidades
- `fix/<tema>` para correcciones
- `refactor/<tema>` para cambios estructurales sin cambio funcional
- `docs/<tema>` para documentación

Ejemplos:
- `feature/ble-motion-stream`
- `fix/sim-runtime-init`
- `refactor/application-runtime`
- `docs/contributing-guide`

### Crear una rama de trabajo
```bash
git checkout develop
git pull origin develop
git checkout -b feature/<tu-tema>
```

### Mantener tu rama actualizada
```bash
git fetch origin
git merge origin/develop
```

---

## 3) Mensajes de commit

Usamos **Conventional Commits** en inglés.

### Formato
```text
<type>(<scope>): <message>
```

### Tipos comunes
- `feat` → nueva funcionalidad
- `fix` → corrección de bug
- `refactor` → mejora estructural sin cambiar comportamiento esperado
- `docs` → documentación
- `test` → tests
- `build` → cambios en PlatformIO, dependencias o configuración
- `style` → formato y estilo sin cambios lógicos

### Scopes sugeridos
- `domain`
- `application`
- `infrastructure`
- `sim`
- `hw`
- `transport`
- `ble`
- `wifi`
- `edge`
- `docs`
- `build`

### Ejemplos
```text
feat(transport): add motion payload batch serializer
fix(sim): correct secondary bus initialization
refactor(application): extract runtime factory from main
docs(domain): document imu and motion state
build(platformio): add ArduinoJson dependency
```

---

## 4) Pull Requests

- La rama base normal para PRs es `develop`.
- La rama `main` se reserva para versiones finales estables o hitos cerrados del proyecto.
- Todo PR debe estar enfocado en un cambio lógico claro.
- Evita mezclar refactor, documentación y nuevas funcionalidades no relacionadas en el mismo PR.

### Checklist antes de abrir un PR
- [ ] El proyecto compila en el environment afectado.
- [ ] Si tocaste código compartido, validaste al menos `pio run -e esp32_sim`.
- [ ] No se introdujeron rutas de include frágiles o hacks innecesarios.
- [ ] La documentación fue actualizada si el cambio lo requiere.
- [ ] Los commits siguen Conventional Commits.
- [ ] No se han subido secretos, claves Wi-Fi ni credenciales.

---

## 5) Principios arquitectónicos del proyecto

La arquitectura actual del proyecto se organiza alrededor de estas capas:
- `domain/`
- `application/`
- `infrastructure/`

### Regla importante
El proyecto **debe seguir el enfoque de ModestIoT** en el modelado del dominio.

Eso implica, en lo posible:
- usar interacciones dirigidas por eventos
- mantener la coordinación principal dentro del dispositivo de dominio
- modelar sensores de dominio alineados con `Sensor`
- modelar el dispositivo central alineado con `Device`
- evitar que `main.cpp` concentre lógica del dominio o de integración

### Qué va en cada capa
- `domain/`: modelos del problema, sensores de dominio, device central, cálculo de ángulos, estado de movimiento
- `application/`: coordinación de ejecución, selección de runtime, ciclo principal de la aplicación
- `infrastructure/`: adapters de simulación, hardware, transporte, red y futuras integraciones externas

### Qué evitar
- meter lógica de negocio en `main.cpp`
- mezclar detalles de hardware dentro del dominio
- acoplar el dominio a JSON, HTTP, BLE o librerías de transporte
- introducir nuevas capas innecesarias sin necesidad real

---

## 6) Estilo de código y convenciones

Este proyecto usa **C++** sobre **Arduino/ESP32** con compilación por PlatformIO.

### Reglas generales
- Usar codificación **UTF-8**
- Indentación de **4 espacios**
- No usar tabs
- Mantener líneas razonablemente cortas
- Evitar clases demasiado grandes
- Evitar código comentado innecesario
- Preferir cambios pequeños y cohesivos

### Convenciones de nombres
- **Clases / structs**: `PascalCase`
  - `UflexDevice`
  - `ImuSample`
  - `MotionPayload`
- **Funciones y variables**: `camelCase`
  - `updateSample`
  - `getMotionState`
  - `middleLowerAngle`
- **Constantes/macros**: `UPPER_SNAKE_CASE`
  - `UFLEX_TARGET_SIM`
  - `READ_INTERVAL_MS`
- **Archivos y carpetas del código propio**: `snake_case`
  - `imu_sample.h`
  - `motion_payload_serializer.cpp`

### Includes
- Preferir includes públicos del proyecto:
```cpp
#include "uflex/domain/devices/uflex_device.h"
```
- Evitar includes relativos frágiles cuando exista una ruta pública más clara.

### Headers y documentación
- Mantener guards de include claros y consistentes.
- Conservar la cabecera generada por el IDE cuando ya se usa en el archivo:
```cpp
//
// Created by <nombre del miembro del equipo> on <fecha>.
//
```
- Documentar archivos importantes con `@file`, `@brief`, autor, fecha y versión.

---

## 7) ModestIoT en este proyecto

El framework **ModestIoT** forma parte de la base conceptual del proyecto y debe respetarse en el diseño.

### En la práctica
- `Imu` debe seguir la idea de un `Sensor`
- `UflexDevice` debe seguir la idea de un `Device`
- cuando el dominio lo requiera, también se puede modelar salida mediante `Actuator`
- la coordinación debe ocurrir en el dispositivo de dominio y no en el punto de entrada

### Importante
- `ModestIoT` debe guiar el modelo del dominio
- el framework también permite trabajar con clases como `Sensor`, `Actuator` y `Device`
- no se debe forzar su ejemplo de LED como plantilla literal para todo
- los ejemplos de `lib/ModestIoT/` pueden servir como referencia, pero el código de `uFlex` debe responder a las necesidades reales del proyecto

---

## 8) Simulación y hardware

El proyecto soporta dos ambientes principales:
- `esp32_sim`
- `esp32_hw`

### Simulación
Usa el adapter de simulación y el runtime de simulación.

```bash
pio run -e esp32_sim
```

### Hardware
Usa el runtime de hardware y los adapters físicos.

```bash
pio run -e esp32_hw
```

### Regla de contribución
Si modificas código que afecta ambos entornos, procura no romper la separación entre:
- runtime de simulación
- runtime de hardware
- dominio compartido

---

## 9) Testing y validación

Actualmente la validación principal del proyecto es:
- compilación por environment
- prueba manual en simulación
- futura validación en hardware real

### Validaciones mínimas recomendadas
```bash
pio run -e esp32_sim
```

Si el cambio toca hardware o infraestructura compartida, también conviene validar:
```bash
pio run -e esp32_hw
```

### Qué revisar manualmente
- que el target correcto compile
- que los includes no rompan otros módulos
- que `main.cpp` siga siendo liviano
- que la separación `domain/application/infrastructure` se mantenga clara

---

## 10) Buenas prácticas para contribuir

- Un cambio lógico por rama o PR
- Preferir refactors incrementales
- Documentar decisiones estructurales cuando no sean obvias
- Mantener el dominio libre de detalles de transporte
- Diseñar pensando en simulación y hardware como adapters de un mismo modelo
- No agregar dependencias externas sin justificar su necesidad

---

## 11) Resolución de conflictos

Actualiza con frecuencia tu rama para reducir conflictos:
```bash
git fetch origin
git merge origin/develop
```

Si hay conflictos:
- resuélvelos localmente
- revisa con cuidado archivos sensibles como:
  - `platformio.ini`
  - `src/main.cpp`
  - `include/build_config.h`
  - componentes de `application/runtime/`
  - adapters de `infrastructure/`

Evita `force push` salvo que el equipo lo tenga acordado.

---

## 12) Comunicación

- Si haces cambios de arquitectura, descríbelos claramente en el PR.
- Si un cambio toca `domain/`, explica cómo sigue alineado con ModestIoT.
- Si introduces infraestructura nueva como BLE, Wi‑Fi o edge integration, deja explícito qué parte es preliminar y qué parte ya está validada.

---
