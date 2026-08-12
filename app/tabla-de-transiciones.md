# tdse-tpf-integrado — Tabla de transición de estados

Tabla de transición de estados

## task_sensor — debouncing (XX = ENT/NEX/ESC)

| Estado actual | Evento | Guarda | Siguiente estado | Acción |
|---|---|---|---|---|
| `ST_BTN_XX_UP` | `EV_BTN_XX_UP` | — | `ST_BTN_XX_UP` | — |
| `ST_BTN_XX_UP` | `EV_BTN_XX_DOWN` | — | `ST_BTN_XX_FALLING` | `tick = tick_max` |
| `ST_BTN_XX_FALLING` | `EV_BTN_XX_UP` | — | `ST_BTN_XX_UP` | `tick = 0` *(ruido)* |
| `ST_BTN_XX_FALLING` | `EV_BTN_XX_DOWN` | `[tick == 0]` | `ST_BTN_XX_DOWN` | `put_event_task_menu(cfg.signal_down)` *(botón pulsado)* |
| `ST_BTN_XX_FALLING` | `EV_BTN_XX_DOWN` | `[tick > 0]` | `ST_BTN_XX_FALLING` | `tick--` |
| `ST_BTN_XX_DOWN` | `EV_BTN_XX_UP` | — | `ST_BTN_XX_RISING` | `tick = tick_max` |
| `ST_BTN_XX_DOWN` | `EV_BTN_XX_DOWN` | — | `ST_BTN_XX_DOWN` | — |
| `ST_BTN_XX_RISING` | `EV_BTN_XX_DOWN` | — | `ST_BTN_XX_DOWN` | `tick = 0` *(ruido)* |
| `ST_BTN_XX_RISING` | `EV_BTN_XX_UP` | `[tick == 0]` | `ST_BTN_XX_UP` | `put_event_task_menu(cfg.signal_up)` *(botón soltado)* |
| `ST_BTN_XX_RISING` | `EV_BTN_XX_UP` | `[tick > 0]` | `ST_BTN_XX_RISING` | `tick--` |

**Notas**
- `tick_max` = `DEL_BTN_XX_MAX` = 50 (ticks del *task scheduler de 1 ms = 50 ms de ventana para el *debounce*). Se configura por instancia (`task_sensor_cfg_t.tick_max`), actualmente 50 para los tres botones.


  | Instancia | `signal_down` (al presionar) | `signal_up` (al soltar) |
  |---|---|---|
  | `ID_BTN_ENT` | `EV_MEN_ENT_ACTIVE` | `EV_MEN_ENT_IDLE` |
  | `ID_BTN_NEX` | `EV_MEN_NEX_ACTIVE` | `EV_MEN_NEX_IDLE` |
  | `ID_BTN_ESC` | `EV_MEN_ESC_ACTIVE` | `EV_MEN_ESC_IDLE` |

## task_menu — screen navigation

| Estado actual | Evento | Guarda | Siguiente estado | Acción |
|---|---|---|---|---|
| `ST_MEN_MAIN_CONFIG` | `EV_MEN_NEX_ACTIVE` | — | `ST_MEN_MAIN_ACTION` | — |
| `ST_MEN_MAIN_CONFIG` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_CONFIG_SETPOINT` | — |
| `ST_MEN_MAIN_ACTION` | `EV_MEN_NEX_ACTIVE` | — | `ST_MEN_MAIN_CONFIG` | — |
| `ST_MEN_MAIN_ACTION` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_ACTION_CONTROL` | — |
| `ST_MEN_CONFIG_SETPOINT` | `EV_MEN_NEX_ACTIVE` | — | `ST_MEN_CONFIG_REPORT` | — |
| `ST_MEN_CONFIG_SETPOINT` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_SETPOINT_VALUE` | — |
| `ST_MEN_CONFIG_SETPOINT` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_MAIN_CONFIG` | — |
| `ST_MEN_CONFIG_REPORT` | `EV_MEN_NEX_ACTIVE` | — | `ST_MEN_CONFIG_SETPOINT` | — |
| `ST_MEN_CONFIG_REPORT` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_REPORT_MODE` | — |
| `ST_MEN_CONFIG_REPORT` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_MAIN_CONFIG` | — |
| `ST_MEN_SETPOINT_VALUE` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_SETPOINT_CONFIRM` | `setpoint_angle = setpoint_preview` *(lectura en tiempo real del pote)* |
| `ST_MEN_SETPOINT_VALUE` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_CONFIG_SETPOINT` | — |
| `ST_MEN_SETPOINT_CONFIRM` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_SETTINGS_SAVED` | `shared_data->setpoint_deg = (float) setpoint_angle` |
| `ST_MEN_SETPOINT_CONFIRM` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_SETPOINT_VALUE` | — |
| `ST_MEN_SETTINGS_SAVED` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_CONFIG_SETPOINT` | — |
| `ST_MEN_SETTINGS_SAVED` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_CONFIG_SETPOINT` | — |
| `ST_MEN_REPORT_MODE` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_CONFIG_REPORT` | — |
| `ST_MEN_ACTION_CONTROL` | `EV_MEN_NEX_ACTIVE` | — | `ST_MEN_ACTION_REPORT` | — |
| `ST_MEN_ACTION_CONTROL` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_PID_CONTROL` | `pid_enabled = true` |
| `ST_MEN_ACTION_CONTROL` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_MAIN_ACTION` | — |
| `ST_MEN_ACTION_REPORT` | `EV_MEN_NEX_ACTIVE` | — | `ST_MEN_ACTION_CONTROL` | — |
| `ST_MEN_ACTION_REPORT` | `EV_MEN_ENT_ACTIVE` | — | `ST_MEN_ANGLE_REPORT` | — |
| `ST_MEN_ACTION_REPORT` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_MAIN_ACTION` | — |
| `ST_MEN_PID_CONTROL` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_ACTION_CONTROL` | `pid_enabled = false` |
| `ST_MEN_ANGLE_REPORT` | `EV_MEN_ESC_ACTIVE` | — | `ST_MEN_ACTION_REPORT` | — |