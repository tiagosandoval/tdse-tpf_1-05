#ifndef APP_INC_APP_H_
#define APP_INC_APP_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdbool.h>
#include <stdint.h>

/********************** macros ***********************************************/

// Bench-test mode selector: change TEST_X and rebuild to run task_imu or
// task_servo in isolation, decoupled from each other, before running the
// combined closed-loop stabilizer.
#define TEST_NORMAL      (0)   // both tasks, closed-loop stabilizer
#define TEST_IMU_ONLY    (1)   // task_imu only -- check raw axes / tita_deg
#define TEST_SERVO_ONLY  (2)   // task_servo only -- open-loop sweep, no IMU
#define TEST_I2C_SCAN    (3)   // task_imu only -- scan I2C1 bus for any responding device
#define TEST_AXIS_CAL    (4)   // both tasks -- servo drives a slow known sweep while
                                // task_imu logs raw 6-axis data + commanded angle, for
                                // offline axis/sign determination via tools/axis_calibration.py

#define TEST_X (TEST_NORMAL)

/********************** typedef **********************************************/

typedef struct {
	float tita_deg;        // measured angle, written by task_imu (deg)
	bool  imu_ok;           // last I2C read succeeded, written by task_imu
	float setpoint_deg;     // target angle, written by task_servo
	float servo_cmd_deg;    // current commanded servo angle, written by task_servo
} shared_data_type;

/********************** external data declaration ****************************/


/********************** external functions declaration ***********************/

void app_init(void);
void app_update(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* APP_INC_APP_H_ */

/********************** end of file ******************************************/
