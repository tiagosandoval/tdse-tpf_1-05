/*
 * @file   : task_servo.c
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include <math.h>
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"

/********************** macros and definitions *******************************/

// --- SERVO PWM on TIM2 CH2 (PA1) ---
// Timer tick = 1 us, ARR = 19999 (20 ms period)

#define SERVO_MIN_US        1000
#define SERVO_MAX_US        2000
#define SERVO_CENTER_US     1500

// Control law: incremental PI on error = setpoint_deg - tita_deg,
// added to the servo's own last commanded angle (unity feedback --
// no separate disturbance/self-motion bookkeeping is needed, since
// tita_deg already reports the net absolute angle regardless of cause).
// Gains are per SECOND (see task_servo_update(), which measures real dt via
// HAL_GetTick() rather than assuming a fixed call period) -- retune from
// scratch if you change these, the old per-call values don't carry over.
#define SETPOINT_DEG         0.0f   // arbitrary hardcoded target (0 = vertical)
#define KP                   0.07f
#define KI                   0.0f
#define INTEGRAL_CLAMP      20.0f
#define DEADBAND_DEG         1.0f
#define MAX_RATE_DEG_PER_SEC 600.0f // deg/s -- the key safety/slew-rate knob
#define SERVO_CTRL_MIN_DEG  10.0f   // actuator-space clamp (raw servo-horn deg)
#define SERVO_CTRL_MAX_DEG 170.0f
#define TITA_LIMIT_DEG      80.0f   // max |setpoint| from vertical -- clamped in task_servo_init()

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

static void servo_write_us(uint16_t us);
static void servo_write_deg(float deg);
static void servo_write_speed(float cmd);
static float clampf(float x, float min, float max);

/********************** internal data definition *****************************/

const char *p_task_servo = "Task Servo";

static float cur_cmd_deg;
static float integral;
static uint32_t last_update_ms;

/********************** external data declaration *****************************/

extern TIM_HandleTypeDef htim2;

/********************** external functions definition ************************/

void task_servo_init(void *parameters)
{
	shared_data_type *shared_data = (shared_data_type *) parameters;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_servo_init), p_task_servo);

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	cur_cmd_deg = 90.0f;
	integral = 0.0f;
	last_update_ms = HAL_GetTick();
	servo_write_deg(cur_cmd_deg);

	shared_data->setpoint_deg = clampf(SETPOINT_DEG, -TITA_LIMIT_DEG, TITA_LIMIT_DEG);
	shared_data->servo_cmd_deg = cur_cmd_deg;
}

#if TEST_X == TEST_SERVO_ONLY

// Open-loop sweep, decoupled from the IMU -- exercises the PWM/wiring only,
// and lets you confirm which physical direction corresponds to 0 vs 180 deg.
// Rate-limited by HAL_GetTick() rather than "once per call", since the
// scheduler calls task_update() far faster than a servo can physically move.
void task_servo_update(void *parameters)
{
	(void) parameters;
	static uint32_t last_step_ms = 0;
	static uint32_t last_print_ms = 0;
	static int8_t direction = 1;

	if (HAL_GetTick() - last_step_ms < 50) {
		return; // step every 50 ms
	}
	last_step_ms = HAL_GetTick();

	cur_cmd_deg += direction * 2.0f;
	if (cur_cmd_deg >= SERVO_CTRL_MAX_DEG) { cur_cmd_deg = SERVO_CTRL_MAX_DEG; direction = -1; }
	if (cur_cmd_deg <= SERVO_CTRL_MIN_DEG) { cur_cmd_deg = SERVO_CTRL_MIN_DEG; direction = 1; }

	servo_write_deg(cur_cmd_deg);

	if (HAL_GetTick() - last_print_ms >= 200) {
		LOGGER_LOG("cmd=%.2f\r\n", cur_cmd_deg);
		last_print_ms = HAL_GetTick();
	}
}

#elif TEST_X == TEST_AXIS_CAL

// Slow, known triangle-wave sweep for axis calibration -- task_imu logs this
// commanded angle alongside all six raw IMU channels each sample, so
// tools/axis_calibration.py can correlate them offline (see the plan).
void task_servo_update(void *parameters)
{
	shared_data_type *shared_data = (shared_data_type *) parameters;
	static uint32_t last_step_ms = 0;
	static int8_t direction = 1;

	if (HAL_GetTick() - last_step_ms < 100) {
		return; // step every 100 ms -- ~10 deg/s, ~16 s per direction
	}
	last_step_ms = HAL_GetTick();

	cur_cmd_deg += direction * 1.0f;
	if (cur_cmd_deg >= SERVO_CTRL_MAX_DEG) { cur_cmd_deg = SERVO_CTRL_MAX_DEG; direction = -1; }
	if (cur_cmd_deg <= SERVO_CTRL_MIN_DEG) { cur_cmd_deg = SERVO_CTRL_MIN_DEG; direction = 1; }

	servo_write_deg(cur_cmd_deg);
	shared_data->servo_cmd_deg = cur_cmd_deg;
}

#else

void task_servo_update(void *parameters)
{
	shared_data_type *shared_data = (shared_data_type *) parameters;

	// Measure real elapsed time rather than assuming a fixed call period --
	// the scheduler's actual rate is governed by SysTick (~1 ms) further
	// slowed by task_imu's blocking I2C read, not a fixed 20 ms. Without
	// this, integral windup and the step-rate limit below would both scale
	// with however often the scheduler happens to call this function.
	uint32_t now = HAL_GetTick();
	float dt = (now - last_update_ms) / 1000.0f;
	last_update_ms = now;
	if (dt <= 0.0f) dt = 0.001f;
	if (dt > 0.1f) dt = 0.1f; // clamp after a stall/gap (e.g. imu_ok was false for a while)

	if (!shared_data->imu_ok) {
		return; // hold last commanded position -- no fallback to a default angle
	}

	// El actuador solamente estabiliza dentro de la pantalla Control PID.
	// En el resto del menu conserva la ultima posicion comandada.
	if (!shared_data->pid_enabled) {
		integral = 0.0f;
		return;
	}

	float error = shared_data->setpoint_deg - shared_data->tita_deg;
	// float error = shared_data->tita_deg - shared_data->setpoint_deg;
	if (fabsf(error) < DEADBAND_DEG) {
		error = 0.0f;
	}

	integral = clampf(integral + error * dt, -INTEGRAL_CLAMP, INTEGRAL_CLAMP);

	float delta = KP * error + KI * integral;
	float max_step = MAX_RATE_DEG_PER_SEC * dt;
	delta = clampf(delta, -max_step, max_step);

	cur_cmd_deg = clampf(cur_cmd_deg + delta, SERVO_CTRL_MIN_DEG, SERVO_CTRL_MAX_DEG);
	servo_write_deg(cur_cmd_deg);

	shared_data->servo_cmd_deg = cur_cmd_deg;
}

#endif

static void servo_write_us(uint16_t us)
{
	if (us < SERVO_MIN_US) us = SERVO_MIN_US;
	if (us > SERVO_MAX_US) us = SERVO_MAX_US;
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, us);
}

// For a normal 180 deg servo (SG90):
static void servo_write_deg(float deg)
{
	if (deg < 0.0f) deg = 0.0f;
	if (deg > 180.0f) deg = 180.0f;
	uint16_t us = (uint16_t)(SERVO_MIN_US + (deg / 180.0f) * (SERVO_MAX_US - SERVO_MIN_US));
	servo_write_us(us);
}

// For a continuous-rotation servo (DM-S0090D style), not wired up yet:
// command in [-1..+1]: negative = one direction, positive = the other, 0 = stop
static void servo_write_speed(float cmd)
{
	if (cmd < -1.0f) cmd = -1.0f;
	if (cmd >  1.0f) cmd =  1.0f;

	float span = 400.0f; // us, gives 1100..1900
	uint16_t us = (uint16_t)(SERVO_CENTER_US + cmd * span);
	servo_write_us(us);
}

static float clampf(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

/********************** end of file ******************************************/
