#ifndef __BSP_T_IMU_H
#define __BSP_T_IMU_H

#include "bsp_can.h"
#include "chassis_task.h"
#include "string.h"
#include "cmsis_os.h"
#include "task.h"
#include "main.h"

#define TIMU_PALSTANCE_ID 0x600
#define TIMU_ANGLE_ID   	0x601

typedef struct
{
	float pitch;
	float yaw;
	float wy;
	float wz;
}Taurus_imu_data_t;

extern Taurus_imu_data_t   imu_data_T;
void T_imu_calcu(uint32_t can_id,uint8_t * CAN_Rx_data);

#endif

