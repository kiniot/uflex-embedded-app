# Plan de Ensamblaje para la Fase del Brazo

## Propósito

Este documento define un plan de ensamblaje físico **más detallado y operativo** para la fase del brazo de `uFlex`.

Su objetivo es que todo el equipo comparta la misma visión sobre:

- qué se va a montar
- cómo se va a montar
- qué decisiones tomar si aparecen dudas
- qué cosas son obligatorias y cuáles son opcionales
- cómo validar que el ensamblaje quedó bien

Este documento complementa:

- [Guía para la Fase del Brazo](arm-phase-guide.es.md)
- [Hardware Overview](hardware-overview.md)

---

## 1. Objetivo de esta fase

En esta fase **no** estamos construyendo el wearable final.

Estamos construyendo un **prototipo funcional para prueba sobre el brazo**, que debe cumplir estas condiciones:

1. Los 3 IMUs deben quedar firmes sobre segmentos corporales distintos.
2. El módulo principal debe funcionar con batería.
3. El cableado debe resistir movimiento moderado sin soltarse.
4. El sistema debe poder encenderse y apagarse de forma práctica.
5. El montaje debe ser suficientemente cómodo para hacer pruebas cortas.

---

## 2. Resultado esperado

Al final del ensamblaje deberíamos tener:

- un **satélite principal** en el antebrazo
- un **satélite IMU** en el bíceps
- un **satélite IMU** en el dorso de la mano
- los 3 sensores conectados y firmes
- el sistema encendiendo con batería
- el firmware leyendo sensores y activando actuadores

---

## 3. Arquitectura física acordada

## 3.1 Distribución general

### Satélite principal: antebrazo

Debe contener:

- ESP32
- batería
- TP4056
- MT3608
- switch
- LED RGB
- buzzer
- etapa del motor vibrador
- 1 IMU
- multiplexor I²C (TCA9548A)

### Satélite secundario 1: bíceps

Debe contener:

- 1 IMU
- base pequeña

### Satélite secundario 2: dorso de la mano

Debe contener:

- 1 IMU
- base pequeña

## 3.2 Principio general

El módulo principal va en el antebrazo porque:

- tiene más espacio
- tolera mejor peso y volumen
- simplifica alimentación y actuadores

Los otros dos satélites deben ser lo más livianos posible.

---

## 4. Materiales a usar en esta fase

## 4.1 Materiales electrónicos principales

- 3× MPU9250
- multiplexor I²C (TCA9548A)
- ESP32 WROOM-32 (placa de desarrollo)
- batería LiPo 3.7V
- TP4056 con protección
- MT3608
- motor vibrador tipo moneda
- buzzer activo 3.3V
- LED RGB
- 2N2222
- resistencias 1K y 220 ohms
- cables AWG 24 o jumpers según tramo
- borneras si aportan orden/removibilidad

## 4.2 Materiales mecánicos principales

- placas perforadas
- tela o banda elástica
- velcro
- opcionalmente EVA o espuma delgada
- silicona caliente
- cinta aislante
- termocontraíble

---

## 5. Criterios de diseño que debemos respetar

## 5.1 Criterio 1: estabilidad del sensor

Cada IMU debe quedar:

- firme
- orientada
- sin rotación libre
- sin desplazamiento evidente sobre el brazo

## 5.2 Criterio 2: mínimo peso en satélites secundarios

En bíceps y mano:

- solo IMU
- solo el cableado necesario
- base pequeña

## 5.3 Criterio 3: robustez del cableado

Ningún cable importante debe quedar:

- colgando libremente
- tirando directo de una soldadura
- tensado en una articulación

## 5.4 Criterio 4: comodidad suficiente para prueba

No buscamos máxima ergonomía todavía, pero sí evitar:

- puntas contra la piel
- placas duras directamente sobre el brazo
- peso excesivo en mano o bíceps

---

## 6. Decisiones de ensamblaje ya acordadas

## 6.1 No usar protoboard como wearable principal

Los protoboards sirven para mesa, no para brazo en movimiento.

## 6.2 Usar placa perforada

La placa perforada será la base del montaje real de esta fase.

## 6.3 Hacer soldadura mínima estratégica

No hace falta soldarlo todo de forma definitiva, pero sí conviene soldar:

- alimentación
- etapa del vibrador
- conexiones estructurales del módulo principal
- uniones que no deben soltarse con movimiento

## 6.4 Usar banda + velcro para cada satélite

El velcro no debe ser el elemento que “aplasta” el módulo.  
El módulo debe ir **montado sobre la banda**, y el velcro debe servir para **cerrar la banda alrededor del brazo**.

---

## 7. Preparación previa antes de ensamblar

Antes de empezar a soldar o pegar, el equipo debe hacer esta preparación:

1. Confirmar que el firmware actual sigue funcionando en protoboard.
2. Confirmar pines actuales:
   - RGB: `18`, `19`, `23`
   - buzzer: `27`
   - vibration motor: `32`
3. Confirmar topología de sensores usada por el firmware real.
4. Confirmar estrategia de alimentación final de esta fase.
5. Decidir qué placa perforada será:
   - principal
   - bíceps
   - mano

---

## 8. Estrategia de alimentación recomendada

La recomendación para esta fase es:

1. batería LiPo
2. TP4056 con protección
3. MT3608 ajustado
4. salida del MT3608 al `VIN` o `5V` del ESP32

## 8.1 Antes de integrarlo al sistema

El MT3608 debe ajustarse **antes** de integrarlo.

### Qué se necesita

- destornillador pequeño
- multímetro idealmente

### Procedimiento mínimo

1. Conectar la entrada del MT3608.
2. Medir la salida.
3. Ajustar el potenciómetro hasta el valor deseado.
4. Recién entonces conectarlo al resto del sistema.

## 8.2 Switch

El slider switch se usará como interruptor general de encendido/apagado del sistema.

---

## 9. Plan de ensamblaje por módulos

## 9.1 Módulo principal del antebrazo

### Componentes que deben ir aquí

- ESP32
- batería
- TP4056
- MT3608
- switch
- LED RGB
- buzzer
- transistor + resistor del vibrador
- diodo del vibrador
- una IMU
- multiplexor I²C (TCA9548A)
- conectores o borneras para sensores remotos si aportan orden

> **Por qué el multiplexor (TCA9548A).** Cada MPU9250 lleva un magnetómetro
> AK8963 en la dirección I²C fija `0x0C`, así que las 3 IMUs colisionan en un bus
> compartido. El TCA9548A aísla cada IMU en su propio canal y vuelve legibles los
> tres magnetómetros — requisito para la detección de compensación con señal
> real. Los 3 IMUs van por el mux en un solo bus I²C (la IMU local del antebrazo
> más los dos satélites remotos por el cable flexible), en lugar de la topología
> de dos buses del prototipo de banco.

### Tamaño de placa

Usar una placa de `5×7 cm` completa si hace falta.

### Objetivo de esta placa

Debe ser el “centro” del prototipo físico.

### Recomendación de layout

Separar visualmente:

- zona de alimentación
- zona de ESP32
- zona de actuadores
- zona de conexiones hacia IMUs remotas

### Qué soldar sí o sí aquí

- línea de alimentación principal
- etapa del motor vibrador
- switch
- puntos de distribución importantes

### Qué puede quedar removible

- algunos conectores hacia satélites
- reemplazos de actuadores si el equipo quiere iterar rápido

---

## 9.2 Satélite de bíceps

### Componentes

- 1 IMU
- base pequeña

### Recomendación

No agregar:

- batería
- actuadores
- componentes pesados

### Tamaño de placa

Si es posible, usar un recorte más pequeño que `5×7 cm`.

### Prioridad

- poco peso
- poca altura
- buena estabilidad

---

## 9.3 Satélite del dorso de la mano

### Componentes

- 1 IMU
- base pequeña

### Prioridad

- que no moleste la muñeca
- que no impida cerrar la mano
- que no tire del cable al mover dedos o muñeca

### Recomendación

Hacerlo más pequeño y ligero incluso que el de bíceps, si es posible.

---

## 10. Cómo construir físicamente cada satélite

## 10.1 Capas del satélite

De arriba hacia abajo:

1. componentes
2. placa/base
3. cara inferior con soldaduras
4. capa aislante intermedia
5. EVA o espuma
6. banda elástica o tela
7. brazo

## 10.2 Material de la capa intermedia

Puede ser:

- plástico delgado
- cartón firme
- cartón plastificado

Su función es:

- aislar
- distribuir presión
- evitar que soldaduras toquen o rompan la espuma

## 10.3 Uso de la EVA o espuma

La EVA o espuma no va directamente contra una cara con soldaduras expuestas.  
Debe ir **después** de la capa intermedia.

---

## 11. Cómo sujetar cada satélite al brazo

## 11.1 Método recomendado

1. preparar una banda de tela o elástico
2. fijar el satélite sobre la banda
3. usar velcro para cerrar la banda alrededor del brazo

## 11.2 Qué NO hacer

- no depender de cinta aislante como sujeción principal
- no usar el velcro para apretar directamente el módulo por encima
- no dejar el satélite colgando solo de cables

## 11.3 Cómo fijar el satélite a la banda

Opciones válidas:

- silicona caliente
- pasar amarre o hilo por los agujeros de la placa
- costura sobre una base intermedia
- combinación de varias

La silicona es válida como ayuda mecánica, no como única garantía estructural para algo pesado.

---

## 12. Cableado entre satélites

## 12.1 Reglas generales

- dejar holgura donde haya articulaciones
- evitar cables tensos
- fijar el cable en puntos intermedios
- no permitir que el movimiento tire directamente de la IMU

## 12.2 Qué evitar

- cables colgando libres
- cableado muy largo sin guías
- cables rígidos cerca de codo o muñeca

## 12.3 Qué material usar

Preferir:

- cable flexible AWG 24

Usar jumpers solo si el tramo es corto y temporal.

---

## 13. Uso de termocontraíble y cinta aislante

## 13.1 Termocontraíble

Usarlo en:

- empalmes
- alimentación
- motor vibrador
- buzzer
- uniones que se moverán

## 13.2 Cinta aislante

Usarla solo como apoyo:

- agrupación temporal
- protección puntual
- sujeción provisional

No usarla como mecanismo principal de montaje al cuerpo.

---

## 14. ¿Hace falta caja?

## 14.1 Satélites pequeños

No necesariamente.  
De hecho, en muchos casos es mejor no usar caja para no agregar volumen.

## 14.2 Módulo principal

Puede tener una cobertura liviana si ayuda a:

- proteger cables
- proteger batería
- ordenar visualmente el conjunto

Puede ser de:

- plástico delgado
- cartón provisional

Evitar metal.

---

## 15. Secuencia recomendada de trabajo

## Etapa 1: validación de topología

1. Confirmar firmware funcionando en mesa.
2. Confirmar lectura de 3 sensores.
3. Confirmar actuadores funcionando.

## Etapa 2: alimentación

1. Preparar batería + TP4056 + MT3608 + switch.
2. Ajustar voltaje del MT3608.
3. Validar encendido estable de la ESP32.

## Etapa 3: módulo principal

1. Montar ESP32 y alimentación sobre placa perforada.
2. Montar actuadores.
3. Montar IMU principal.
4. Preparar salidas hacia satélites remotos.

## Etapa 4: satélites secundarios

1. Preparar satélite de bíceps.
2. Preparar satélite de mano.
3. Verificar que ambos queden livianos y firmes.

## Etapa 5: integración mecánica

1. Preparar bandas.
2. Montar capas inferiores.
3. Fijar cada satélite a su banda.
4. Guiar y fijar cableado.

## Etapa 6: prueba sobre brazo

1. Colocar bíceps.
2. Colocar antebrazo.
3. Colocar mano.
4. Encender.
5. Validar lectura en reposo.
6. Validar lectura en movimiento suave.
7. Ajustar fijación si hay rotación o desplazamiento.

---

## 16. Criterios de aceptación del ensamblaje

El ensamblaje de esta fase se considera aceptable si:

1. El sistema enciende con batería.
2. Los 3 IMUs se leen correctamente.
3. Los actuadores responden.
4. Los satélites no se desplazan visiblemente durante movimientos moderados.
5. Ningún cable tira directamente de una soldadura crítica.
6. El usuario puede usarlo por una prueba corta sin molestia grave.

---

## 17. Errores comunes a evitar

- dejar componentes pesados en bíceps o mano
- usar demasiados jumpers flojos
- no planificar la ruta del cable antes de fijar los satélites
- montar placas con puntas hacia la piel
- confiar demasiado en la cinta aislante
- no ajustar el MT3608 antes de conectarlo
- no separar claramente el módulo principal de los satélites ligeros

---

## 18. Resumen final para el equipo

La decisión compartida debería ser esta:

- construir un **módulo principal** en antebrazo sobre placa perforada
- construir **dos satélites livianos** para bíceps y mano
- usar **banda + velcro** para la sujeción
- usar **capa aislante + EVA** debajo de las placas
- usar **alimentación con batería + TP4056 + MT3608**
- aceptar **soldadura mínima estratégica**
- priorizar estabilidad mecánica por encima de “acabado bonito”

Esta fase se trata de lograr un prototipo:

- estable
- entendible
- repetible
- suficientemente cómodo

No de lograr todavía un wearable final.

