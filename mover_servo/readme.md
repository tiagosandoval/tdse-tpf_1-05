# Control de servo por PWM (NUCLEO-F103RB + STM32CubeIDE)

Este proyecto genera una señal **PWM a 50 Hz** con un temporizador del STM32 (TIM2) para comandar servos hobby. Se prueba el movimiento de un **SG90 (posicional)** o el giro/paro de un **DM‑S0090D (rotación continua)**.

La intención a futuro es combinarlo con el proyecto de IMU (MPU‑6050) para que el servo reaccione automáticamente y mantenga un ángulo objetivo respecto de la vertical (control en lazo cerrado).

---

## 1) Qué hace el proyecto y cómo funciona

### Señal PWM de servo (concepto)
Un servo hobby se comanda con pulsos repetidos a ~**50 Hz** (período ~**20 ms**). Dentro de cada período, se define un ancho de pulso alto típico:

- **~1000 µs**: extremo/velocidad en un sentido
- **~1500 µs**: centro (servo posicional) o **STOP** (servo rotación continua)
- **~2000 µs**: extremo/velocidad en el otro sentido

En este proyecto:
- Se configura **TIM2_CH1** para tener **tick = 1 µs** y período de 20 ms.
- Se cambia el registro **CCR1** para fijar el ancho del pulso en microsegundos (por ejemplo 1500).

---

## 2) Conexión del servo a la Nucleo

### Cableado típico del servo (3 hilos)
- **Rojo**: +5V
- **Marrón/Negro**: GND
- **Naranja/Amarillo/Blanco**: Señal PWM

### Conexión recomendada
- Servo **Señal** → **PA0** (TIM2_CH1)
- Servo **GND** → **GND** de la Nucleo
- Servo **+5V** → **fuente externa de 5V** (por ejemplo cargador de celular USB)

> Condición obligatoria: **GND de la fuente externa y GND de la Nucleo deben estar conectados** (masa común), si no la señal PWM no tiene referencia y el servo puede comportarse errático.

### Desacople (muy recomendado)
Cerca del servo (en la línea de 5V y GND):
- **100 µF a 470 µF** electrolítico (más es mejor para picos)
- opcional: **100 nF** cerámico en paralelo

Esto reduce reinicios y ruidos cuando el servo arranca o se traba.

---

### **Advertencia:** alimentación del servo externa (por qué NO conviene desde la Nucleo)
Un micro-servo como el SG90 puede demandar picos grandes al arrancar o cuando se bloquea mecánicamente (“stall”). La NUCLEO‑F103RB, cuando se alimenta por USB (con ST‑LINK), tiene límites de corriente para el conjunto **placa + expansiones**: el manual indica un máximo del orden de **300 mA** en la configuración típica por USB. ([manualshelf.com](https://www.manualshelf.com/manual/stmicroelectronics/nucleo-f103rb/user-manual-english.html?utm_source=openai))  

Por otro lado, un SG90 se alimenta típicamente a **4.8–6 V** (5 V típico), y aunque la hoja de datos de algunos proveedores resume torque/voltaje, en la práctica el consumo puede subir mucho bajo carga; por ejemplo, se reportan corrientes de **stall** del orden de cientos de mA para servos micro (dependiendo del modelo/versión). ([datasheet4u.com](https://datasheet4u.com/datasheets/Tower-Pro/SG90/791970?utm_source=openai))  


**Conclusión operativa:** para evitar resets, el servo debe alimentarse con **5V externo** (cargador de celular / fuente 5V 1A o más) y se mantiene masa común con la Nucleo.

---

## 3) Diferencias de control: SG90 vs DM‑S0090D

### SG90 (servo posicional ~180°)
- El ancho de pulso se interpreta como **posición objetivo**.
- Aproximación típica:
  - 1000 µs ≈ 0°
  - 1500 µs ≈ 90°
  - 2000 µs ≈ 180°
- Sirve para “mantener un ángulo” mecánico (posicionamiento).

### DM‑S0090D (servo de rotación continua 360°)
El DM‑S0090D se describe como servo de **rotación continua 360°**, donde la señal PWM controla **dirección y velocidad**, no un ángulo absoluto. ([arduwiki.com](https://arduwiki.com/wiki/Servo_Motor%28DM-S0090D%2C360%29?utm_source=openai))  

Interpretación típica (estilo Arduino `Servo.write()`):
- alrededor de **1500 µs** / “90” → **STOP**
- menor que eso → gira en un sentido (más rápido cuanto más se aleja)
- mayor que eso → gira en el otro sentido

**Consecuencia para el proyecto IMU + servo:** con DM‑S0090D no existe “posición” interna; para sostener un ángulo sería necesario un control externo (IMU + lazo de control) y aun así el actuador se comporta más como “motor con caja reductora” que como servo posicional.

---

## 4) Configuración del `.ioc` (STM32CubeMX / STM32CubeIDE)

### A) TIM2 PWM en PA0
En **Pinout & Configuration**:
- **Timers → TIM2**
  - **Channel1**: `PWM Generation CH1`

En la vista de pines:
- **PA0** debe quedar como **TIM2_CH1**.

### B) Clock Configuration (para tick de 1 µs)
En el clock tree (ejemplo típico de este proyecto):
- APB1 Timer clocks: **64 MHz** (TIM2 corre a 64 MHz)

### C) Parámetros TIM2 (PWM servo 50 Hz)
En **TIM2 → Parameter Settings**:
- **Prescaler (PSC)**: `64-1`  
  (deja el timer a 1 MHz → 1 µs por tick)
- **Counter Period (ARR)**: `20000-1`  
  (20 ms → 50 Hz)
- **Pulse (CCR1 inicial)**: `1500`  
  (1.5 ms: centro/stop)

La combinación anterior hace que:
- `CCR1 = 1000` → 1.0 ms
- `CCR1 = 1500` → 1.5 ms
- `CCR1 = 2000` → 2.0 ms

### D) UART (opcional, recomendado para debug)
- **Connectivity → USART2 → Asynchronous**
- 115200 8N1, sin flow control

---

## 5) Lógica de control en el código (resumen)

- Se inicia el PWM con `HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);`
- Se actualiza el pulso escribiendo `CCR1` (vía `__HAL_TIM_SET_COMPARE`):
  - SG90: se mapea grados → microsegundos
  - DM‑S0090D: se mapea comando de velocidad (−1..+1) → microsegundos alrededor de 1500

> Nota: los límites reales (mínimo/máximo) pueden requerir ajuste según el servo (para evitar zumbidos o saturación). La calibración típica es empezar con 1000–2000 µs y refinar.

