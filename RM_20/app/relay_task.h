/**
  * @file relay_task.h
  * @version 1.0
  * @date March,24 2019
  *
  * @brief  
  *
  *	@author li zh
  *
  */
#ifndef __RELAY_TASK_H__
#define __RELAY_TASK_H__
#ifdef  __RELAY_TASK_GLOBALS
#define __RELAY_TASK_EXT
#else
#define __RELAY_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"
#include "bsp_pump.h"

#define RELAY_TASK_PERIOD 5

typedef struct
{
	int16_t yaw;
	int16_t pitch; //初始化值
}view_data;

typedef struct
{
	view_data view_tx;
	int8_t status[8];//串口发送气阀和电气状态 前两个字节表示数据帧头
	uint8_t gas_status;//气阀控制位
	uint8_t electrical_status; //电气控制位
	uint16_t dis1;		//激光
	uint16_t dis2;
	uint8_t bullet_left;
	uint8_t bullet_right;
	int16_t enc;
	
} relay_t;

__RELAY_TASK_EXT relay_t relay;

void relay_task(void const *argu);
void relay_init(void);
__RELAY_TASK_EXT void get_relay_message(uint8_t CAN_Rx_data[8]);

#endif
