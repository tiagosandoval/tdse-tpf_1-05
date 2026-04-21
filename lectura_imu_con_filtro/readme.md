# Lectura de ángulo con MPU-6050 (Nucleo-F103RB + STM32CubeIDE)

Este proyecto lee un **MPU-6050** por **I2C**, calcula un **ángulo único** (“tita”, en grados) usando un **filtro complementario** (acelerómetro + giroscopio) y lo manda por **UART (USART2)** a la PC para verlo en un terminal serial.

> Nota: la configuración de periféricos del STM32 está en el `.ioc` del proyecto, pero igual se detalla cómo configurar las opciones por si se recrea desde cero.



## 1) Funcionamiento

### Qué hace el programa
- Imprime por serial líneas del estilo:
  - `tita(deg)=... ax=... ay=... az=... gx=...`
- `tita(deg)` es **un solo ángulo** (en grados) estimado mediante un **filtro complementario** a partir de:
  - **Acelerómetro** (dirección de la gravedad → estable, pero ruidoso)
  - **Giroscopio** (velocidad angular → suave y rápido, pero con *drift* en el tiempo)



## 2) Cableado: MPU-6050 ↔ Nucleo-F103RB

### Alimentación
- **VCC** del módulo MPU-6050:
  - Preferible: **3.3V** de la Nucleo (pin 3V3)
  - Muchos módulos (ej: GY-521) aceptan 5V porque traen regulador, pero no todos; si no hay certeza, se recomienda **3.3V**.
- **GND** a **GND**

### I2C (I2C1)
En este proyecto se usa **I2C1**:

- **SCL** → **PB8** (I2C1_SCL)  
  - En formato “Arduino” (mapeo estilo Arduino Core para STM32 / nomenclatura típica): **D15**
- **SDA** → **PB9** (I2C1_SDA)  
  - En formato “Arduino”: **D14**

> Importante: el número “Arduino Dxx” puede variar según el core/variant usado; la referencia sólida para STM32CubeIDE es **PB8/PB9**.

### Dirección I2C (pin AD0)
- Si **AD0 = GND** → dirección 0x68 (la más común)
- Si **AD0 = VCC** → dirección 0x69

El código asume **0x68** (AD0 a GND).

### Otros pines (opcionales)
- **INT**: no se usa por ahora (puede quedar sin conectar)
- **XDA/XCL**: no se usan

### Pull-ups
I2C necesita resistencias pull-up en **SDA** y **SCL**.

- Muchos módulos ya traen resistencias pull-up (típicamente ~**4.7kΩ** a VCC).
- Si el módulo **no** trae pull-ups, o si la carga total es rara, pueden aparecer lecturas inestables o directamente no comunicarse.



## 3) Configuración de la Nucleo-F103RB en CubeMX / `.ioc`

> El `.ioc` ya viene en el proyecto. Esto es para saber qué tiene que estar seteado.

### A) MCU / Board
- Seleccionar la placa **NUCLEO-F103RB** (o el MCU STM32F103RBTx equivalente).

### B) Pinout & Configuration

#### I2C1
Menú:
- **Connectivity → I2C1**
  - Mode: **I2C**
  - Pines típicos: **PB8 (SCL)** y **PB9 (SDA)**  
    (si Cube propone otros, se puede cambiar, pero el cableado tiene que coincidir)

Parámetros sugeridos:
- I2C Speed: 100 kHz o 400 kHz (100 kHz es más “tranqui” para arrancar)

#### USART2 (para prints)
Menú:
- **Connectivity → USART2**
  - Mode: **Asynchronous**
  - Baud rate: **115200**
  - Pines: **PA2 = TX**, **PA3 = RX**

Esto se conecta al **ST-LINK Virtual COM Port** (VCP), así que no hace falta un conversor USB-Serial externo.

#### SYS / Debug (recomendado)
Menú:
- **System Core → SYS**
  - Debug: **Serial Wire** (SWD)

### C) Clock Configuration
En general puede dejarse lo que genera Cube para la Nucleo, pero conviene verificar que:
- el micro corra a una frecuencia razonable (ej: 72 MHz típicamente)
- no haya errores en “Clock Configuration”

### D) Project Manager
- Toolchain / IDE: **STM32CubeIDE**
- Generate code: default




## 4) Habilitar impresión de floats (printf con %f)

Si por UART se ve algo tipo:
- `tita(deg)=  ax= ay= az=  gx=`
(sin números)

Eso suele pasar porque `printf` “nano” viene sin soporte de float por defecto.

### Cómo habilitarlo en STM32CubeIDE
1. Click derecho sobre el **proyecto** (no sobre `main.c`)
2. **Properties**
3. **C/C++ Build → Settings**
4. Pestaña **Tool Settings**
5. Buscar:
   - **MCU/MPU GCC Linker → Miscellaneous**
6. En “Linker flags / Other flags” agregar:
   - `-u _printf_float`
7. Aplicar, cerrar, y recompilar (si hace falta: **Project → Clean**)

> Esto aumenta un poco el tamaño del binario, pero habilita `%.2f`, `%.3f`, etc.



## Cómo correrlo y ver la salida

1) Compilar y flashear (Run o Debug).  
2) Si se está en Debug y el programa queda parado en `main()`, usar **Resume (F8)**.  
3) Abrir un terminal serial (Tera Term / PuTTY):
- Puerto: **STMicroelectronics STLink Virtual COM Port (COMx)**
- Baud: **115200**
- 8N1, flow control: **None**



## Anexo: detalles de la lectura del MPU-6050 y su procesamiento

### Qué lee del MPU-6050
Se hace una lectura “burst” de **14 bytes** desde el registro `ACCEL_XOUT_H (0x3B)` que trae, en este orden:

1. Accel X, Y, Z (cada uno 16 bits con signo)
2. Temperatura (16 bits con signo)
3. Gyro X, Y, Z (16 bits con signo)

Con eso se obtienen `ax_raw`, `ay_raw`, `az_raw`, `gx_raw`, etc.

### Rangos (full-scale) del MPU-6050
Se configura escribiendo registros del **MPU-6050 por I2C** en el `main.c`.

Actualmente el código configura:
- Acelerómetro **±8g** (registro `ACCEL_CONFIG = 0x1C`, bits `AFS_SEL = 2`)
- Giroscopio **±500 °/s** (registro `GYRO_CONFIG = 0x1B`, bits `FS_SEL = 1`)

Y por eso usa estas constantes:
- `ACC_LSB_PER_G = 4096.0` (para ±8g)
- `GYRO_LSB_PER_DPS = 65.5` (para ±500 °/s)

> Importante: si se cambia el rango en los registros del MPU, también tienen que actualizarse las constantes de escala, si no los valores en unidades físicas quedan mal.

### Filtro complementario para el cálculo del ángulo
Se combina:
- una estimación rápida basada en integrar el **giroscopio**, y
- una corrección lenta basada en el **acelerómetro** (gravedad),

Esto permite buena respuesta a movimientos rápidos sin que el ángulo derive ("driftee") indefinidamente.
1) **Ángulo por acelerómetro**  
Se calcula un ángulo “instantáneo” usando la gravedad (en grados), por ejemplo:

- `accel_tita = atan( ay / sqrt(ax² + az²) ) * 180/pi`

Esto es estable en el tiempo, pero **ruidoso** y se “ensucia” si hay aceleraciones lineales (vibraciones, golpes, etc.).

2) **Ángulo por giroscopio (integración)**  
Se integra la velocidad angular:

- `tita_gyro = tita_prev + gx_dps * dt`

Esto es suave y responde rápido, pero **deriva** (bias/offset del gyro).

3) **Fusión (filtro complementario)**  
Se combinan ambas mediciones:

- `tita = 0.98 * (tita_prev + gx_dps*dt) + 0.02 * accel_tita`

La idea es:
- El **giroscopio** manda en lo rápido (0.98)
- El **acelerómetro** “corrige” lento para evitar deriva (0.02)

`dt` se calcula con `HAL_GetTick()` (milisegundos desde que arranca).

### Cosas a tener en cuenta del MPU-6050
- El giroscopio casi siempre tiene **offset**; si se busca un ángulo más estable, conviene calibrar (promediar unos segundos quieto y restar).
- El acelerómetro mide **gravedad + aceleraciones**; si el sistema vibra o acelera fuerte, el ángulo por acelerómetro se distorsiona.
- El cálculo de un solo ángulo depende de **qué eje** se usa (`gx`, `ay`, etc.) y de cómo esté orientado el módulo.