/*
 * @file   : task_imu.c
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

#define MPU_ADDR            (0x68 << 1)
#define REG_PWR_MGMT_1      0x6B
#define REG_GYRO_CONFIG     0x1B
#define REG_ACCEL_CONFIG    0x1C
#define REG_ACCEL_XOUT_H    0x3B

// Accel +-8g  => 4096 LSB/g
// Gyro  +-500 => 65.5 LSB/(deg/s)
#define ACC_LSB_PER_G       4096.0f
#define GYRO_LSB_PER_DPS    65.5f

// Raw atan2(ax_g, ay_g) frame reads -90 deg when the arm is physically
// vertical (measured empirically). Subtracting this offset re-zeroes the
// reported tita_deg so 0 = vertical -- recalibrate (rerun TEST_IMU_ONLY,
// hold the arm vertical, read the raw value) if the IMU is ever remounted.
#define TITA_VERTICAL_OFFSET_DEG (-90.0f)

/********************** internal data declaration ****************************/

typedef struct {
	float tita_deg;        // filtered angle (deg)
	float tita_prev_deg;
	uint32_t t_prev_ms;
} imu_filter_t;

/********************** internal functions declaration ***********************/

static int16_t be16(const uint8_t *p);
static HAL_StatusTypeDef mpu_write(uint8_t reg, uint8_t val);
static HAL_StatusTypeDef mpu_read(uint8_t reg, uint8_t *data, uint16_t len);
static void imu_filter_init(imu_filter_t *f);
static void imu_filter_update(imu_filter_t *f, float ax_g, float ay_g, float gz_dps);

/********************** internal data definition *****************************/

const char *p_task_imu = "Task IMU";

static imu_filter_t imu;

/********************** external data declaration *****************************/

extern I2C_HandleTypeDef hi2c1;

/********************** external functions definition ************************/

#if TEST_X == TEST_I2C_SCAN

// Sweeps every 7-bit I2C address once at boot and logs whichever ones ACK.
// Use this when mpu_read() fails on every call, to find out whether *any*
// device answers on the bus at all, and at what address (0x68 is what the
// rest of this file assumes -- 0x69 if the module's AD0 pin is tied high).
void task_imu_init(void *parameters)
{
	(void) parameters;

	LOGGER_LOG("  %s is running - I2C1 bus scan\r\n", GET_NAME(task_imu_init));

	for (uint8_t addr = 1; addr < 128; addr++) {
		if (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 1, 10)) {
			LOGGER_LOG("  found device at 0x%02X\r\n", addr);
		}
	}

	LOGGER_LOG("  scan done\r\n");
}

void task_imu_update(void *parameters)
{
	(void) parameters;
}

#elif TEST_X == TEST_AXIS_CAL

// Raw, unfiltered 6-axis logging alongside the commanded servo angle (see
// task_servo.c's TEST_AXIS_CAL branch) -- no fusion here, since the point is
// to let tools/axis_calibration.py determine the true shaft axis/sign from
// the raw channels, independent of imu_filter_update()'s current assumption.
void task_imu_init(void *parameters)
{
	(void) parameters;

	LOGGER_LOG("  %s is running - axis calibration\r\n", GET_NAME(task_imu_init));

	// Wake up
	mpu_write(REG_PWR_MGMT_1, 0x00);
	HAL_Delay(100);

	mpu_write(REG_ACCEL_CONFIG, (2 << 3));
	mpu_write(REG_GYRO_CONFIG,  (1 << 3));
	HAL_Delay(10);
}

void task_imu_update(void *parameters)
{
	shared_data_type *shared_data = (shared_data_type *) parameters;
	uint8_t raw[14];
	static uint32_t last_print_ms = 0;

	if (HAL_OK == mpu_read(REG_ACCEL_XOUT_H, raw, 14)) {

		int16_t ax_raw = be16(&raw[0]);
		int16_t ay_raw = be16(&raw[2]);
		int16_t az_raw = be16(&raw[4]);
		int16_t gx_raw = be16(&raw[8]);
		int16_t gy_raw = be16(&raw[10]);
		int16_t gz_raw = be16(&raw[12]);

		float ax_g = ax_raw / ACC_LSB_PER_G;
		float ay_g = ay_raw / ACC_LSB_PER_G;
		float az_g = az_raw / ACC_LSB_PER_G;
		float gx_dps = gx_raw / GYRO_LSB_PER_DPS;
		float gy_dps = gy_raw / GYRO_LSB_PER_DPS;
		float gz_dps = gz_raw / GYRO_LSB_PER_DPS;

		if (HAL_GetTick() - last_print_ms >= 50) {
			LOGGER_LOG("t=%lu,c=%.2f,ax=%.2f,ay=%.2f,az=%.2f,gx=%.1f,gy=%.1f,gz=%.1f\r\n",
					HAL_GetTick(), shared_data->servo_cmd_deg, ax_g, ay_g, az_g, gx_dps, gy_dps, gz_dps);
			last_print_ms = HAL_GetTick();
		}
	}
	else {
		LOGGER_LOG("error\r\n");
	}
}

#else

void task_imu_init(void *parameters)
{
	shared_data_type *shared_data = (shared_data_type *) parameters;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_imu_init), p_task_imu);

	imu_filter_init(&imu);

	// Wake up
	mpu_write(REG_PWR_MGMT_1, 0x00);
	HAL_Delay(100);

	// Accel range +-8g  => ACCEL_CONFIG[4:3] = 10
	// Gyro  range +-500 => GYRO_CONFIG[4:3]  = 01
	mpu_write(REG_ACCEL_CONFIG, (2 << 3));
	mpu_write(REG_GYRO_CONFIG,  (1 << 3));
	HAL_Delay(10);

	shared_data->tita_deg = 0.0f;
	shared_data->imu_ok = false;
}

void task_imu_update(void *parameters)
{
	shared_data_type *shared_data = (shared_data_type *) parameters;
	uint8_t raw[14];

	if (HAL_OK == mpu_read(REG_ACCEL_XOUT_H, raw, 14)) {

		int16_t ax_raw = be16(&raw[0]);
		int16_t ay_raw = be16(&raw[2]);
		int16_t gz_raw = be16(&raw[12]);

		float ax_g = ax_raw / ACC_LSB_PER_G;
		float ay_g = ay_raw / ACC_LSB_PER_G;
		float gz_dps = gz_raw / GYRO_LSB_PER_DPS;

		imu_filter_update(&imu, ax_g, ay_g, gz_dps);

		float tita_deg = imu.tita_deg - TITA_VERTICAL_OFFSET_DEG; // 0 = vertical
		shared_data->tita_deg = tita_deg;
		shared_data->imu_ok = true;

#if TEST_X == TEST_IMU_ONLY
		// Confirmed via TEST_AXIS_CAL + tools/axis_calibration.py: shaft axis
		// is IMU-Z (gz_dps), tilt pair is atan2(ax_g, ay_g). Watch gz/tita
		// here if re-verifying after a remount (tita should read ~0 when
		// the arm is held vertical).
		static uint32_t last_print_ms = 0;
		if (HAL_GetTick() - last_print_ms >= 200) {
			LOGGER_LOG("ax=%.2f ay=%.2f gz=%.2f tita=%.2f\r\n",
					ax_g, ay_g, gz_dps, tita_deg);
			last_print_ms = HAL_GetTick();
		}
#endif
	}
	else {
		LOGGER_LOG("error\r\n");
		shared_data->imu_ok = false;
	}
}

#endif

static int16_t be16(const uint8_t *p)
{
	return (int16_t)((p[0] << 8) | p[1]);
}

static HAL_StatusTypeDef mpu_write(uint8_t reg, uint8_t val)
{
	return HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef mpu_read(uint8_t reg, uint8_t *data, uint16_t len)
{
	return HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

static void imu_filter_init(imu_filter_t *f)
{
	f->tita_deg = 0.0f;
	f->tita_prev_deg = 0.0f;
	f->t_prev_ms = HAL_GetTick();
}

// Complementary filter: fuses accel-based angle with gyro rate about Z --
// confirmed via TEST_AXIS_CAL + tools/axis_calibration.py (accel variance on
// az was ~20x lower than ax/ay, since gravity has no component along a
// horizontal shaft; atan2(ax_g, ay_g) tracked the commanded sweep at
// corr=0.987). tita_deg is the absolute angle relative to gravity/vertical,
// regardless of whether the rotation came from the servo itself or an
// external disturbance.
static void imu_filter_update(imu_filter_t *f,
                              float ax_g, float ay_g,
                              float gz_dps)
{
	uint32_t now = HAL_GetTick();
	float dt = (now - f->t_prev_ms) / 1000.0f;
	if (dt <= 0.0f) dt = 0.001f;
	f->t_prev_ms = now;

	float accel_tita = atan2f(ax_g, ay_g) * (180.0f / (float)M_PI);

	float alpha = 0.98f;
	f->tita_deg = alpha * (f->tita_prev_deg + gz_dps * dt) + (1.0f - alpha) * accel_tita;
	f->tita_prev_deg = f->tita_deg;
}

/********************** end of file ******************************************/
