/** 
  * @file uplift_task.c
  * @version 1.1
  * @date Mar,2 2019
	*
  * @brief  抬升机构任务
	*
  *	@author li zh
  *
  */
#define __UPLIFT_TASK_GLOBALS
#include "uplift_task.h"
#include "pid.h"
#include "bsp_can.h"
#include "bsp_motor.h"
#include "cmsis_os.h"
#include "string.h"
#include "remote_msg.h"
#include "math.h"
#include "keyboard_handle.h"

#define uplift_ratio (8192.0f)

uint16_t uplift_test;
extern TaskHandle_t can_msg_send_task_t;

void uplift_init()
{
		//抬升的PID  机械有问题,老板说这个有点难,但是他通个宵就能调好
		PID_struct_init(&pid_uplift_spd[0], POSITION_PID, 15000, 3000,
									10.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_spd[1], POSITION_PID, 15000, 3000,
									10.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_height[0], POSITION_PID, 2800, 500,
									800.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_height[1], POSITION_PID, 2800, 500,
									800.0f,	0.0f,	0.0f	);  
	
	
	uplift.state = UPLIFT_UNKNOWN;

	//初始化两个高度反馈
	
	uplift.height_get_bullet_REF = HEIGHT_GET;//取弹高度
	uplift.height_give_hero_REF  = 0;					//给弹高度
	uplift.calibra_flag1=1;				//初始化标志位
	uplift.calibra_flag2=1;
	uplift.height_offset[0] = 0;	//高度补偿值
	uplift.height_offset[1] = 0;
}
/**
  * @brief uplift_task
  * @param     
  * @attention  未调试
	* @note  软件定时器
  */
void uplift_task(void const *argu)
{
	taskENTER_CRITICAL();
	
	uplift.height_fdb[0] =  moto_uplift[0].total_ecd/uplift_ratio  -  uplift.height_offset[0];
	uplift.height_fdb[1] =  moto_uplift[1].total_ecd/uplift_ratio  -  uplift.height_offset[1];
	
	if(uplift.state != UPLIFT_KNOWN && uplift.ctrl_mode == UPLIFT_AUTO) //自动模式且未知状态下进行校准 
	{
		switch(uplift.state)
		{
			case UPLIFT_UNKNOWN:
			{
				uplift.state = UPLIFT_CALIBRA;
			}break;
			case UPLIFT_CALIBRA:
			{
				uplift.height_offset[0]=moto_uplift[0].total_ecd/uplift_ratio;
				uplift.height_offset[1]=moto_uplift[1].total_ecd/uplift_ratio;
				uplift.state = UPLIFT_KNOWN;
			}break;
			case UPLIFT_KNOWN:
			{
						
			}break;
		}	
	}
	else
	{
		switch(uplift.ctrl_mode)
		{
			case UPLIFT_ADJUST:
			{

				uplift.height_ref[0] -= 0.001*rc.ch5; 
				uplift.height_ref[1] =  -uplift.height_ref[0];
				
//				//设置上下限
				if(uplift.height_ref[0]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[0] = HEIGHT_DOWN_LIMIT;
				if(uplift.height_ref[1]>=HEIGHT_DOWN_LIMIT)	uplift.height_ref[1] = HEIGHT_DOWN_LIMIT;
//				if(uplift.height_ref[0]>=HEIGHT_UP_LIMIT)	uplift.height_ref[0] = HEIGHT_UP_LIMIT;
//				if(uplift.height_ref[1]>=HEIGHT_UP_LIMIT)	uplift.height_ref[1] = HEIGHT_UP_LIMIT;
				
				uplift.spd_ref[0] = pid_calc(&pid_uplift_height[0],uplift.height_fdb[0],uplift.height_ref[0]); //高度环
				uplift.current[0] = pid_calc(&pid_uplift_spd[0],moto_uplift[0].speed_rpm,uplift.spd_ref[0]); //速度环

				uplift.spd_ref[1] = pid_calc(&pid_uplift_height[1],uplift.height_fdb[1],uplift.height_ref[1]); //高度环
				uplift.current[1] = pid_calc(&pid_uplift_spd[1],moto_uplift[1].speed_rpm,uplift.spd_ref[1]); //速度环		

			}break;
			case UPLIFT_STOP:
			{
				uplift.current[0] = 0;
				uplift.current[1] = 0;
				uplift.height_ref[0] =uplift.height_fdb[0] ; //设置当前位置为目标位置
				uplift.height_ref[1] =uplift.height_fdb[1] ; 
			}break;
			case UPLIFT_AUTO:
			{
				//设置上下限
				if(uplift.height_ref[0]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[0] = HEIGHT_DOWN_LIMIT;
				if(uplift.height_ref[1]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[1] = HEIGHT_DOWN_LIMIT;
				if(uplift.height_ref[0]>=HEIGHT_UP_LIMIT)	uplift.height_ref[0] = HEIGHT_UP_LIMIT;
				if(uplift.height_ref[1]>=HEIGHT_UP_LIMIT)	uplift.height_ref[1] = HEIGHT_UP_LIMIT;
								
				uplift.spd_ref[0] = pid_calc(&pid_uplift_height[0],uplift.height_fdb[0],uplift.height_ref[0]); //高度环
				uplift.current[0] = pid_calc(&pid_uplift_spd[0],moto_uplift[0].speed_rpm,uplift.spd_ref[0]); //速度环
				
				uplift.height_ref[1] =  -uplift.height_ref[0];

				uplift.spd_ref[1] = pid_calc(&pid_uplift_height[1],uplift.height_fdb[1],uplift.height_ref[1]); //高度环
				uplift.current[1] = pid_calc(&pid_uplift_spd[1],moto_uplift[1].speed_rpm,uplift.spd_ref[1]); //速度环						
			}break;
			case UPLIFT_KEYBORAD:
			{
				//keyboard_handle();
			}break;
		}
	
	}
	
	uplift.last_ctrl_mode = uplift.ctrl_mode;
	
	taskEXIT_CRITICAL();
	
	memcpy(motor_cur.uplift_cur, uplift.current, sizeof(uplift.current));
	osSignalSet(can_msg_send_task_t, UPLIFT_MOTOR_MSG_SEND);
}



