#ifndef TASK_INC_TASK_IMU_H_
#define TASK_INC_TASK_IMU_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
void task_imu_init(void *parameters);
void task_imu_update(void *parameters);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_IMU_H_ */

/********************** end of file ******************************************/
