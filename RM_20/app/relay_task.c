/** 
  * @file relay_task.c
  * @version 1.1
  * @date Mar,24th 2019
	*
  * @brief  与气阀测距扩展板的通讯任务
	*
  *	@author li zh
  *
  */
#define __RELAY_TASK_GLOBALS
#include "relay_task.h"
#include "cmsis_os.h"
#include "usart.h"
#include "comm_task.h"
#include "stdlib.h"
#include "bsp_can.h"
#include "bsp_pump.h"
#include "uplift_task.h"

extern TaskHandle_t can_msg_send_task_t;

/**
  * @brief realy_task
  * @param     
  * @attention  
	* @note  
					1 完成
					2 添加电气状态发送，串口发送两个字节
  */
void relay_task(void const *argu)
{
	uint32_t relay_wake_time = osKernelSysTick();
	for(;;)
	{
		help_executed();
		press_executed();
		throw_executed();
		bracket_executed();
		rotate_executed();	
		magazine_executed();
		
		camera_executed();
		
		relay.status[0]=relay.gas_status;
		relay.status[1]=relay.electrical_status;
		
		taskENTER_CRITICAL();
		HAL_UART_Transmit(&huart6, (uint8_t *)&relay.status, 2, 10);
		taskEXIT_CRITICAL();
		osDelayUntil(&relay_wake_time, RELAY_TASK_PERIOD);
		
	}

}




