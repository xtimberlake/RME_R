/** 
  * @file debug_task.c
  * @version 1.0
  * @date 1,21 2019
	*
  * @brief  调试用
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

//函数说明：MiniBalance上位机通过串口打印数据波形
//附加说明：直接在主函数中调用此函数（注意延时减少打印量）
//函数无返回 
void DataWave(UART_HandleTypeDef* huart)
{
		//Channel：选择通道（1-10）
		//传入单精度浮点数
	
//		DataScope_Get_Channel_Data(rotate.cnt_fdb, 1 );
//	  DataScope_Get_Channel_Data(rotate.cnt_ref, 2 );
//		DataScope_Get_Channel_Data(-moto_rotate[0].speed_rpm, 3 ); 
//	  DataScope_Get_Channel_Data(rotate.spd_ref, 4 ); 
		DataScope_Get_Channel_Data(slip.dist_fdb, 1 );
	  DataScope_Get_Channel_Data(slip.dist_ref, 2 );
		DataScope_Get_Channel_Data(moto_slip.speed_rpm, 3 ); 
	  DataScope_Get_Channel_Data(slip.spd_ref, 4 ); 

		CK.Send_Count = DataScope_Data_Generate(4);//串口需要发送的数据个数
	
	
	
	
	
//		if(huart == &huart1)
//		{
//			for( CK.DataCnt = 0 ; CK.DataCnt < CK.Send_Count; CK.DataCnt++) 
//			{
//			while((USART1->SR&0X40)==0);  
//			USART1->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt]; 
//			}
//		}
//		else if(huart == &huart6)
//		{
			for( CK.DataCnt = 0 ; CK.DataCnt < CK.Send_Count; CK.DataCnt++) 
			{
			while((UART5->SR&0X40)==0);  
			UART5->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt]; 
			}
//		}
//		else if(huart == &huart2)
//		{
//			for( CK.DataCnt = 0 ; CK.DataCnt < CK.Send_Count; CK.DataCnt++) 
//			{
//			while((USART2->SR&0X40)==0);  
//			USART2->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt]; 
//			}
//		}
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

