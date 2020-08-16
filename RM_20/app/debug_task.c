/** 
  * @file debug_task.c
  * @version 1.0
  * @date 1,21 2019
	*
  * @brief  ������
	*
  *	@author chen wb
  *
  */
#define __DEBUG_TASK_GLOBALS
#include "debug_task.h"
#include "cmsis_os.h"
#include "comm_task.h"
#include "usart.h"
#include "DataScope_DP.h"
#include "chassis_task.h"
#include "pid.h"
#include "bsp_motor.h"
#include "bsp_can.h"
#include "slip_task.h"
#include "uplift_task.h"
#include "rotate_task.h"
#include "remote_msg.h"

//����˵����MiniBalance��λ��ͨ�����ڴ�ӡ���ݲ���
//����˵����ֱ�����������е��ô˺�����ע����ʱ���ٴ�ӡ����
//�����޷��� 
void DataWave(UART_HandleTypeDef* huart)
{
	//Channel��ѡ��ͨ����1-10��
	//���뵥���ȸ�����

	DataScope_Get_Channel_Data(moto_uplift[0].speed_rpm, 1 );
	DataScope_Get_Channel_Data(-moto_uplift[1].speed_rpm, 2 );
//	DataScope_Get_Channel_Data(uplift.height_fdb[0], 1 );
	DataScope_Get_Channel_Data(uplift.height_fdb[0]*10, 3 ); 
	DataScope_Get_Channel_Data(-uplift.height_fdb[1]*10, 4 );
	
//	DataScope_Get_Channel_Data(uplift.height_ref[1], 4 ); 

	CK.Send_Count = DataScope_Data_Generate(4);//������Ҫ���͵����ݸ���
	
//	if(huart == &huart1)
//	{
//	for( CK.DataCnt = 0 ; CK.DataCnt < CK.Send_Count; CK.DataCnt++) 
//	{
//	while((USART1->SR&0X40)==0);  
//	USART1->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt]; 
//	}
//	}
//	else if(huart == &huart6)
//	{
	for( CK.DataCnt = 0 ; CK.DataCnt < CK.Send_Count; CK.DataCnt++) 
	{
	while((UART5->SR&0X40)==0);  
	UART5->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt]; 
	}
//	}
//	else if(huart == &huart2)
//	{
//		for( CK.DataCnt = 0 ; CK.DataCnt < CK.Send_Count; CK.DataCnt++) 
//		{
//		while((USART2->SR&0X40)==0);  
//		USART2->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt]; 
//		}
//	}
}

void debug_task(void const *argu)
{
	for(;;)
	{
		taskENTER_CRITICAL();
		DataWave(&huart5);
	  taskEXIT_CRITICAL();
//		osDelay(5);
		osDelay(20);
	}
}

