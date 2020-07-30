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
	//pump.magazine_ctrl_mode = OFF_MODE;uint32_t relay_wake_time = osKernelSysTick();
	
	relay.view_tx.yaw		= 1480;
	relay.view_tx.pitch = 1150; //初始化图传角度
	
	relay.status[0]=0XAA; //进入任务后标志位一直不会改变
	relay.status[1]=0XBB;
	
	for(;;)
	{	
		help_executed();
		press_executed();
		bracket_executed();
		throw_executed();
		bullet1_executed();
		bullet2_executed();

		camera_executed();
		rotate_executed();
		
		relay_safe_executed();
		
		relay.status[2] = relay.gas_status;
		relay.status[3] = relay.electrical_status;
		
		relay.status[4] = relay.view_tx.yaw >> 8;
		relay.status[5] = relay.view_tx.yaw;
		relay.status[6] = relay.view_tx.pitch >> 8;
		relay.status[7] = relay.view_tx.pitch;
		
		
		taskENTER_CRITICAL();
		HAL_UART_Transmit(&huart6, (uint8_t *)&relay.status, 8, 0xff);
		osDelay(10); //延时一段时间，让气阀板检测空闲
		taskEXIT_CRITICAL();

	}

}


