/**
	* @file bsp_T_imu.c
	* @version 1.0
	* @date 2020.1.4
  *
  * @brief  TaurusÍÓÂÝÒÇÄÚ²â°æ
  *
  *	@author YY
  *
  */
	
#include "bsp_T_imu.h"
#include "bsp_can.h"


float 	palstance_buffer[2];
float 	angle_buffer[2];
Taurus_imu_data_t   imu_data_T;

void T_imu_calcu(uint32_t can_id,uint8_t * CAN_Rx_data)
{
	switch(can_id)
  {
		case TIMU_PALSTANCE_ID:	//½ÇËÙ¶È
		{
			memcpy(palstance_buffer,CAN_Rx_data,8);
			imu_data_T.wy = palstance_buffer[0];
			imu_data_T.wz = palstance_buffer[1];
//			gimbal.sensor.pit_palstance = -1.0f * imu_data_T.wy ;
//			gimbal.sensor.yaw_palstance = imu_data_T.wz ;
		}
		break;
		case TIMU_ANGLE_ID:			//½Ç¶È
		{
			memcpy(angle_buffer,CAN_Rx_data,8);
			imu_data_T.pitch = angle_buffer[0];
			imu_data_T.yaw 	 = angle_buffer[1];
		}
		break;
	}
}

