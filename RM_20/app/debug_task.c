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
#include "gimbal_task.h"
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
	
		DataScope_Get_Channel_Data(uplift.height_fdb[0], 1 );
		DataScope_Get_Channel_Data(uplift.height_ref[0], 2 ); 
		DataScope_Get_Channel_Data(uplift.spd_ref[0], 3 );
		DataScope_Get_Channel_Data(uplift.spd_ref[0], 4 );   
		DataScope_Get_Channel_Data(uplift.height_fdb[1], 5 );
		DataScope_Get_Channel_Data(uplift.height_ref[1], 6 ); 
		DataScope_Get_Channel_Data(uplift.spd_ref[1], 7 );
		DataScope_Get_Channel_Data(uplift.spd_ref[1], 8 ); 		
//		DataScope_Get_Channel_Data(rotate.ecd_ref / 100.0f, 1 );
//		DataScope_Get_Channel_Data(rotate.ecd_fdb / 100.0f, 2 );
//		DataScope_Get_Channel_Data(rotate.spd_ref * 10.0f, 3 ); 
//		DataScope_Get_Channel_Data(moto_rotate[0].speed_rpm * 10.0f, 4 );   
//		DataScope_Get_Channel_Data(gimbal.pid.yaw_spd_fdb, 5 );
//		DataScope_Get_Channel_Data(gimbal.pid.yaw_spd_ref , 6 );
	
//		DataScope_Get_Channel_Data(chassis.current_fdb[0]/1000.0*24.0, 1 );
//		DataScope_Get_Channel_Data(chassis.current_fdb[1]/1000.0*24.0, 2 );
//		DataScope_Get_Channel_Data(chassis.current_fdb[2]/1000.0*24.0, 3 ); 
//		DataScope_Get_Channel_Data(chassis.current_fdb[3]/1000.0*24.0, 4 );   
//		DataScope_Get_Channel_Data(chassis.war_sum, 5 );
//		DataScope_Get_Channel_Data(pid_chassis_angle.get[0], 6 );
//		DataScope_Get_Channel_Data(pid_chassis_angle.set[0], 7 );
	
	
//		DataScope_Get_Channel_Data(uplift.spd_ref[0],1);
//		DataScope_Get_Channel_Data(moto_uplift[0].speed_rpm,2);
//		DataScope_Get_Channel_Data(slip.dist_ref,3);
//		DataScope_Get_Channel_Data(slip.dist_fdb,4);
		
		
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
			while((USART6->SR&0X40)==0);  
			USART6->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt]; 
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
//		taskENTER_CRITICAL();
//		DataWave(&huart6);
//		taskEXIT_CRITICAL();
//		osDelay(5);
		osDelay(500);
	}
}

