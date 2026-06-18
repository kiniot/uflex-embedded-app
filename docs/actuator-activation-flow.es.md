# Flujo de Aplicación y Activación de Actuadores

## Propósito

Este documento resume los principales hallazgos y decisiones arquitectónicas conversadas hasta ahora para responder una pregunta central del proyecto:

**¿Cómo saben los actuadores cuándo activarse y bajo qué parámetros?**

La idea es dejar una referencia clara para el equipo, especialmente porque todavía estamos aterrizando la arquitectura operativa del sistema completo (`embedded`, `mobile`, `edge`, `backend`).

---

## 1. Problema Principal

El proyecto `uFlex` no solo necesita leer sensores y transmitir datos. También necesita decidir:

- cuándo activar actuadores locales
- qué parámetros o umbrales usar
- qué nodo del sistema decide cada cosa
- cómo se coordinan `embedded`, `mobile`, `edge` y `backend`

Los actuadores actuales del firmware son:

- `RgbLed`
- `ActiveBuzzer`
- `VibrationMotor`

Todos ya fueron modelados como actuators de dominio y aplicados al entorno mediante adapters GPIO compartidos.

---

## 2. Hallazgos Principales

### 2.1 Los actuadores de tiempo real no deben depender del backend

Si el dispositivo necesita vibrar, sonar o prender un LED porque el paciente salió de rango o superó un umbral, esa decisión no debería esperar una respuesta remota.

Eso implica:

- no depender del backend para reaccionar
- no depender de internet para feedback inmediato
- evitar cadenas largas como `embedded -> edge -> backend -> edge -> embedded` para activar actuadores

### 2.2 El embedded sí debe conocer la configuración activa

El `embedded` no tiene que ser la fuente de verdad de la configuración, pero sí necesita conocer la configuración activa durante la sesión.

Eso incluye, por ejemplo:

- ROM mínimo y máximo
- márgenes de advertencia
- modo de feedback
- ejercicio o perfil activo

Esa configuración puede vivir inicialmente en RAM. Si el dispositivo reinicia, otro nodo del sistema puede reenviarla.

### 2.3 El backend no debería ejecutar lógica de reacción inmediata

El backend sí debe seguir siendo la fuente de verdad global para:

- parámetros clínicos
- perfiles de ejercicio
- configuraciones por paciente
- reglas globales del sistema

Pero no es el lugar correcto para disparar actuadores locales en tiempo real.

### 2.4 El edge sí debe conocer y persistir la configuración

Como el edge tendrá SQLite, es un muy buen candidato para funcionar como coordinador local:

- mantiene configuración local vigente
- guarda sesiones
- persiste muestras
- resincroniza con backend
- reenvía configuración al embedded cuando hace falta

### 2.5 El mobile no tiene que mandar nada al embedded necesariamente

No es obligatorio que el mobile controle directamente el embedded.

Para esta versión, la recomendación discutida es:

- el mobile conversa con el backend
- el edge coordina localmente con el embedded
- el embedded transmite datos al mobile por BLE

---

## 3. Flujo Recomendado para Esta Versión

### 3.1 Flujo de control de sesión

Para esta versión, el flujo recomendado es:

1. `mobile -> backend` para iniciar o finalizar sesión
2. `backend -> edge` para reflejar la sesión/configuración vigente
3. `edge -> embedded` para enviar:
   - `startSession`
   - `stopSession`
   - thresholds
   - parámetros clínicos
   - configuración de feedback

Esto significa que **no tomamos como flujo recomendado** `mobile -> edge -> embedded` para start/stop en esta versión del documento.

### 3.2 Flujo de datos de tiempo real

El flujo recomendado para telemetría inmediata es:

1. `embedded -> mobile` por BLE
2. `embedded -> edge` por Wi-Fi/HTTP, idealmente en lotes o con menor frecuencia
3. `edge -> backend` de forma agregada o periódica

### 3.3 Flujo de activación de actuadores

El flujo recomendado es:

1. el `backend` define los parámetros clínicos
2. el `edge` los recibe, los guarda en SQLite y los distribuye
3. el `embedded` recibe y mantiene la configuración activa en RAM
4. el `embedded` evalúa en tiempo real el estado del movimiento
5. el `embedded` activa actuadores localmente cuando corresponde

En otras palabras:

- el backend define la política
- el edge la distribuye y persiste
- el embedded la ejecuta en tiempo real

---

## 4. Qué Debe Saber Cada Nodo

### 4.1 Embedded

Debe saber:

- configuración activa de sesión
- thresholds/ROM/márgenes
- estado actual del movimiento
- lógica local de reacción inmediata

No debería ser la fuente de verdad de reglas clínicas persistentes.

Debe poder:

- medir sensores
- calcular métricas base
- decidir feedback local
- transmitir datos

### 4.2 Mobile

Debe saber:

- estado de la sesión
- datos de telemetría en tiempo real
- lo suficiente para presentar la experiencia al usuario

Para esta versión:

- inicia/finaliza sesión vía backend
- recibe datos en tiempo real desde el embedded por BLE

### 4.3 Edge

Debe saber:

- configuración vigente
- estado local de sesiones
- datos recibidos del embedded
- estado de sincronización con backend

Debe poder:

- almacenar en SQLite
- reenviar configuración al embedded
- guardar lotes
- sincronizar con backend

### 4.4 Backend

Debe saber:

- reglas clínicas
- parámetros del paciente
- configuración global
- historial central de sesiones

Debe actuar como:

- fuente de verdad global
- coordinador de negocio

---

## 5. Recomendación Sobre el Procesamiento de Sensores

### 5.1 Qué hace hoy el embedded

Hoy el firmware:

- lee 3 IMUs
- calcula ángulos relativos entre segmentos
- expone `pitchDeg` y `rollDeg`

Actualmente esos ángulos salen principalmente del acelerómetro.

### 5.2 Qué se recomienda a futuro

Se recomienda que el `embedded` sea el nodo que haga el procesamiento base de orientación, no el edge.

Eso significa que, a futuro, el embedded debería:

- leer acelerómetro + giroscopio + magnetómetro
- aplicar calibración básica
- aplicar fusión sensorial
- producir orientación útil
- derivar métricas como:
  - Euler
  - cuaterniones
  - ángulos relativos entre segmentos

### 5.3 Por qué no dejar esto al edge

Porque el embedded es el único nodo que:

- tiene las muestras crudas en tiempo real
- controla actuadores
- necesita reaccionar con mínima latencia

El edge puede enriquecer, almacenar o resumir, pero no debería ser el primer responsable del cálculo principal de orientación si queremos feedback local robusto.

---

## 6. Payloads y Ritmos de Envío

### 6.1 Embedded -> Mobile por BLE

Objetivo:

- feedback inmediato
- visualización en tiempo real

Recomendación:

- payload liviano
- alta frecuencia
- métricas ya procesadas

### 6.2 Embedded -> Edge por Wi-Fi/HTTP

Objetivo:

- persistencia local
- consolidación
- sincronización posterior

Recomendación:

- envío por lotes o a intervalos más largos
- no necesariamente cada muestra individual

### 6.3 Edge -> Backend

Objetivo:

- historial central
- resiliencia ante caídas de red
- reducción de overhead

Recomendación:

- lotes
- sincronización periódica
- eventos importantes inmediatos solo si realmente aporta

---

## 7. Flujos Ideales

### 7.1 Flujo ideal recomendado para esta versión

1. `mobile -> backend`
   - iniciar sesión
   - finalizar sesión
2. `backend -> edge`
   - configuración activa
   - estado de sesión
3. `edge -> embedded`
   - `startSession`
   - `stopSession`
   - thresholds
   - perfil de ejercicio
4. `embedded -> mobile`
   - stream BLE en tiempo real
5. `embedded -> edge`
   - muestras o lotes
6. `edge -> backend`
   - sincronización

### 7.2 Flujo ideal de activación de actuadores

1. el embedded recibe configuración activa
2. el embedded calcula movimiento en tiempo real
3. el embedded compara contra thresholds locales
4. el embedded decide activar:
   - LED
   - buzzer
   - vibración

---

## 8. Flujos Alternativos Considerados

### 8.1 Alternativa: `mobile -> embedded`

Ventajas:

- menos latencia para control directo
- útil para MVPs o debugging

Desventajas:

- más acoplamiento entre app móvil y firmware
- menos coordinación local centralizada
- complica la fuente operativa de verdad

### 8.2 Alternativa: `mobile -> edge -> embedded`

Fue considerada como opción operativa local interesante, especialmente para start/stop, pero **no es el flujo recomendado para esta versión del documento**.

### 8.3 Alternativa: `embedded -> edge -> mobile`

No se recomienda como flujo principal de tiempo real, porque mete al edge en la ruta de menor latencia y complica la experiencia inmediata del usuario.

---

## 9. Recomendaciones Arquitectónicas

### 9.1 Recomendación general

Mantener esta separación conceptual:

- `domain/`: reglas del problema y actuators
- `application/`: política de sesión, tiempos, coordinación
- `infrastructure/`: sensores, actuators físicos, transporte, canales de entrada/salida

### 9.2 Recomendación sobre entradas externas al embedded

Si el embedded va a recibir configuración o comandos desde afuera, eso debe modelarse como adapters de entrada en infraestructura.

Ejemplos futuros:

- adapter BLE de configuración
- adapter HTTP/Wi‑Fi de configuración
- adapter de comandos de sesión

### 9.3 Recomendación sobre memoria del embedded

Para una primera versión:

- basta con mantener configuración activa en RAM
- si el embedded reinicia, el edge puede reenviarla

A futuro:

- podría evaluarse persistencia local usando NVS/flash del ESP32

---

## 10. Respuesta Corta a la Pregunta Central

### ¿Cómo saben los actuadores cuándo activarse y bajo qué parámetros?

Los actuadores deben activarse cuando el `embedded`, usando configuración activa previamente recibida, detecta en tiempo real que se cumple una condición local relevante.

Eso implica:

- los parámetros clínicos nacen en backend
- el edge los recibe, persiste y distribuye
- el embedded los mantiene activos en memoria durante la sesión
- el embedded aplica esos parámetros localmente sobre el movimiento medido
- el embedded decide cuándo activar actuadores

En una frase:

**El backend define, el edge distribuye y persiste, y el embedded ejecuta en tiempo real.**

---

## 11. Próximos Pasos Recomendados

1. Definir una estructura formal de configuración activa para el embedded.
2. Definir el contrato de mensajes `edge -> embedded`.
3. Definir el payload BLE mínimo hacia mobile.
4. Definir el payload Wi‑Fi/HTTP hacia edge.
5. Reemplazar los comportamientos actuales de prueba de actuadores por criterios clínicos o funcionales reales.
