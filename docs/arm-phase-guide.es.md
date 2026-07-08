# Guía para la Fase del Brazo

## Propósito

Este documento resume lo necesario para pasar del prototipo de mesa a una **fase 2 de prueba en brazo** para `uFlex`.

El foco aquí no es el firmware ni la simulación, sino el **prototipo físico wearable**:

- qué materiales actuales sí alcanzan
- qué faltantes son realmente importantes
- cómo distribuir los módulos en el brazo
- cómo alimentarlo con batería
- cuándo conviene soldar y cuándo no
- cómo sujetar sensores, cables y módulo principal

Esta guía complementa:

- [Hardware Overview](hardware-overview.md)
- [Flujo de Aplicación y Activación de Actuadores](actuator-activation-flow.es.md)

---

## 1. Objetivo de esta fase

La fase del brazo busca validar:

- lectura de movimiento en una postura real
- comportamiento de los 3 IMUs sobre segmentos corporales
- comodidad y estabilidad del montaje
- respuesta de actuadores locales
- funcionamiento con batería

**No** se busca todavía un producto final ni una carcasa industrial.

---

## 2. Conclusión rápida

Sí, con los materiales actuales **se puede pasar a una fase del brazo**.

Pero:

- no conviene usar protoboard como solución wearable principal
- no hace falta mandar a fabricar un PCB todavía
- sí conviene usar **placa perforada**
- sí conviene hacer **algo de soldadura mínima estratégica**
- el mayor reto ya no es electrónico, sino **mecánico y ergonómico**

---

## 3. Qué materiales actuales sí sirven

Con lo ya comprado, se puede construir un prototipo funcional de prueba:

- 3× MPU9250
- multiplexor I²C (TCA9548A)
- ESP32 WROOM-32 en placa de desarrollo
- batería LiPo 3.7V
- TP4056 con protección
- MT3608 step-up
- motor vibrador tipo moneda
- buzzer activo 3.3V
- LED RGB
- transistor 2N2222
- resistencias
- cable flexible AWG 24
- placas perforadas
- borneras
- slider switches
- cautín / soldadura / pasta
- termocontraíble
- silicona caliente
- cinta aislante

---

## 4. Qué faltantes son realmente importantes

Los faltantes más relevantes no son de lógica digital sino de **montaje físico**:

- sistema de sujeción al brazo
- mejor fijación mecánica del cableado
- herramienta cómoda para ajustar el MT3608

### Faltantes prioritarios recomendados

- velcro
- banda elástica o tira de tela
- opcionalmente EVA o espuma delgada
- destornillador pequeño para ajustar el MT3608

### Faltantes deseables, pero no bloqueantes

- extractor de soldadura
- lija
- sierra pequeña para cortar placas
- una caja ligera para el módulo principal

---

## 5. Arquitectura física recomendada

La idea de usar **3 satélites** sobre el brazo es buena para esta fase.

### Distribución recomendada

#### Satélite 1: antebrazo

Aquí conviene ubicar el módulo principal:

- ESP32
- batería
- carga/protección
- alimentación
- actuadores
- 1 IMU
- multiplexor I²C (TCA9548A)

> **Por qué el multiplexor (TCA9548A).** Cada uno de los 3 MPU9250 lleva un
> magnetómetro AK8963 en la dirección I²C fija `0x0C`, así que colisionan si se
> ponen en el mismo bus. El TCA9548A aísla cada IMU en su propio canal, de modo
> que los tres magnetómetros se vuelven legibles — que es lo que habilita la
> detección de compensación con señal real (un valor de yaw). Los 3 IMUs van por
> el mux en un solo bus I²C; los dos satélites remotos (bíceps, mano) se conectan
> a canales del mux por el cable flexible.

Ventajas:

- más espacio
- mejor superficie de apoyo
- mejor lugar para el módulo más pesado

#### Satélite 2: bíceps

Aquí conviene dejar solo:

- 1 IMU
- base pequeña

Debe ser lo más ligero posible.

#### Satélite 3: dorso de la mano

Aquí también conviene dejar solo:

- 1 IMU
- base pequeña

Debe ser liviano y poco invasivo para no estorbar el movimiento de la muñeca/mano.

---

## 6. Placa perforada: cuándo usarla

### Sí conviene usar placa perforada

Para la fase del brazo, la placa perforada es mejor que el protoboard porque:

- resiste mejor el movimiento
- reduce falsos contactos
- permite fijar módulos y cableado
- da una base física más confiable

### No hace falta PCB impreso todavía

No es necesario diseñar ni fabricar un PCB final en este momento.

La placa perforada es suficiente para esta fase de validación.

---

## 7. Tamaño de placas

### Placas de 5×7 cm

Las placas perforadas de `5 cm x 7 cm` sirven, pero no igual para todo.

#### Para el módulo principal

Ese tamaño está bien para:

- ESP32
- alimentación
- actuadores
- switch
- conexiones principales

#### Para bíceps y dorso de mano

`5×7 cm` suele ser grande para solo una IMU.

Recomendación:

- usar una parte más pequeña
- cortar una placa
- o aceptar temporalmente un tamaño mayor si aún no quieren cortar nada

---

## 8. Cómo cortar una placa perforada

Si quieren reducir el tamaño para los satélites pequeños:

- lo ideal es una sierra fina o segueta pequeña
- si no la tienen, se puede marcar una línea de agujeros y partir con cuidado

### Recomendación práctica

Si no tienen herramienta cómoda todavía:

- no se bloqueen
- usen la placa completa temporalmente
- o consigan una hoja de sierra metálica fina antes de cortar

---

## 9. Cómo sujetar cada satélite al brazo

La solución más simple y razonable para esta fase es:

- una **base rígida pequeña**
- una **banda elástica o de tela**
- **velcro** para cerrar la banda
- **EVA o espuma** opcional para comodidad

### Idea clave

El satélite **no debería depender de que el velcro pase por encima del módulo**.

Lo mejor es:

1. fijar el satélite **sobre la banda**
2. usar el velcro para **cerrar la banda alrededor del brazo**

### Estructura general

```text
[ satélite / componentes ]
[ base rígida ]
[ capa aislante ]
[ EVA o espuma ]
[ banda elástica o tela ]
[ brazo ]
```

El velcro cierra la banda; no tiene que prensar directamente el módulo.

---

## 10. Cómo fijar la base a la banda

Sí, se puede usar **silicona caliente** como ayuda mecánica.

### Usos válidos de la silicona

- pegar la base del satélite a una pieza de tela
- reforzar salida de cables
- alivio de tensión
- evitar que el cable tire de una soldadura

### Lo que no debería hacer la silicona

- reemplazar una conexión eléctrica confiable
- ser el único soporte de una pieza pesada colgante

Si se puede, mejor combinar:

- silicona
- fijación por agujeros de la placa
- y/o costura/amarre a la banda

---

## 11. Capa inferior: por qué hace falta

La cara inferior de una placa perforada suele tener:

- soldaduras
- patas cortadas
- puentes

Por eso **no conviene poner la EVA directamente contra esa cara** si tiene puntas o uniones expuestas.

### Hace falta una capa intermedia

Sirve para:

- aislar eléctricamente
- mejorar comodidad
- evitar que una punta rompa la espuma o toque la piel

### Materiales útiles para esa capa

- plástico delgado
- cartón firme
- cartón plastificado
- una lámina rígida ligera no metálica

---

## 12. Capas recomendadas por satélite

### Satélite pequeño (IMU en bíceps o mano)

De arriba hacia abajo:

1. IMU y conexiones
2. placa/base pequeña
3. cara inferior con soldaduras
4. capa aislante intermedia
5. EVA o espuma
6. banda de tela/elástica
7. brazo

### Módulo principal (antebrazo)

De arriba hacia abajo:

1. ESP32, batería, actuadores, IMU
2. placa perforada 5×7
3. cara inferior con soldaduras
4. capa aislante intermedia más firme
5. EVA o espuma
6. banda más ancha
7. antebrazo

---

## 13. Alimentación recomendada

### Opción recomendada con los materiales actuales

Usar:

1. batería LiPo 3.7V
2. TP4056 con protección
3. MT3608 ajustado
4. salida del MT3608 hacia `VIN` o `5V` de la placa ESP32

### Por qué usar el MT3608

No es estrictamente obligatorio, pero sí es la opción más conveniente con lo que ya se tiene:

- estabiliza mejor la alimentación del sistema
- hace más razonable alimentar la placa ESP32 de desarrollo

### Qué hace falta para usarlo bien

- un destornillador pequeño
- idealmente un multímetro

Hay que ajustar su salida antes de conectarlo al sistema final.

---

## 14. Switch de encendido

Los slider switches que ya tienen **sí sirven** como interruptor general de encendido/apagado.

No hace falta comprar otro tipo de switch para esta fase.

La función del switch es:

- interrumpir o permitir el paso de corriente al sistema
- hacer más cómodo el encendido/apagado sin desconectar cables

---

## 15. Termocontraíble: para qué sirve

El termocontraíble no es obligatorio, pero sí muy recomendable.

Sirve para:

- aislar empalmes
- proteger soldaduras
- reforzar uniones
- evitar cortocircuitos
- mejorar la resistencia mecánica del cableado

### Dónde usarlo primero

- motor vibrador
- alimentación
- buzzer
- empalmes de cables
- cualquier unión que vaya a moverse

---

## 16. ¿Hace falta caja?

### Para los satélites pequeños

No es obligatoria y, en muchos casos, no conviene.

Puede:

- agregar volumen
- agregar peso
- volver incómodo el montaje

### Para el módulo principal

Sí podría tener sentido una pequeña carcasa o cobertura ligera.

Puede ser:

- plástico delgado
- cartón provisional

Mientras no sea metálica ni bloquee la antena del ESP32 de forma extraña, no debería afectar de forma importante la señal BLE/Wi‑Fi.

---

## 17. Qué no conviene hacer

- usar protoboard como montaje final de brazo
- llevar muchos jumpers sueltos al cuerpo
- dejar cables colgando sin alivio de tensión
- fijar todo solo con cinta aislante
- montar IMUs pesadas o voluminosas en bíceps/mano
- poner la cara con soldaduras directamente contra la piel o espuma

---

## 18. Qué sí conviene hacer

- usar placa perforada para el módulo principal
- reducir peso en satélites pequeños
- usar banda + velcro para cada satélite
- usar EVA o espuma para comodidad
- usar una capa aislante intermedia debajo de las soldaduras
- sujetar cables en puntos intermedios
- dejar holgura en zonas de articulación
- usar algo de soldadura mínima estratégica

---

## 19. Lista mínima recomendada para comprar o conseguir

Para pasar bien a la fase del brazo, lo más importante de conseguir es:

- velcro
- banda elástica o tela/correa
- opcionalmente EVA o espuma delgada
- destornillador pequeño para el MT3608

Deseable:

- un diodo más robusto para el motor vibrador
- herramienta cómoda para cortar placas

---

## 20. Resumen final

Sí, con los materiales actuales se puede pasar a una fase de prueba en brazo.

La estrategia recomendada es:

- **3 satélites**
- **módulo principal en antebrazo**
- **IMUs ligeras en bíceps y dorso de mano**
- **placa perforada en vez de protoboard**
- **banda + velcro para sujeción**
- **capa aislante + EVA debajo de las placas**
- **MT3608 para alimentación estable**

La principal limitación ya no es electrónica sino:

- montaje físico
- comodidad
- estabilidad del sensor
- manejo de cableado

Esa es la prioridad real para esta fase.

