/** 
  * @file relay_task.c
  * @version 1.1
  * @date Mar,24th 2019
	*
  * @brief  �����������չ���ͨѶ����
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
					1 ���
					2 ��ӵ���״̬���ͣ����ڷ��������ֽ�
  */
void relay_task(void const *argu)
{
	uint32_t relay_wake_time = osKernelSysTick();
	
	relay.status[0]=0XAA; //����������־λһֱ����ı�
	relay.status[1]=0XBB;
	
	for(;;)
	{
		help_executed();
		bracket_executed();
		magazine_executed();
		throw_executed();
		press_executed();
		rotate_executed();
		interact_executed();
		
		camera_executed();
		laser_executed();
		
		relay.status[2]=relay.gas_status;
		relay.status[3]=relay.electrical_status;
		
		taskENTER_CRITICAL();
		HAL_UART_Transmit(&huart6, (uint8_t *)&relay.status, 2, 0x10);
		taskEXIT_CRITICAL();
		osDelayUntil(&relay_wake_time, RELAY_TASK_PERIOD);

	}

}




