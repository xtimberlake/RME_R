/** 
  * @file chassis_task.c
  * @version 1.1
  * @date Jan,6th 2019
	*
  * @brief  底盘姿态任务文件
	*
  *	@author lin kr
  *
  */
#define __CHASSIS_TASK_GLOBALS
#include "chassis_task.h"
#include "cmsis_os.h"
#include "usart.h"
#include "comm_task.h"
#include "string.h"
#include "modeswitch_task.h"
#include "remote_msg.h"
#include "pid.h"
#include "stdlib.h"
#include "bsp_can.h"
#include "data_processing.h"
#include "math_calcu.h"
#include "bsp_pump.h"
#include "bsp_motor.h"

chassis_t chassis;

#define chassis_ratio (409.6f)

extern TaskHandle_t can_msg_send_task_t;

/**
  * @brief chassis_task
  * @param     
  * @attention  
	* @note  
  */
void chassis_task(void const *argu)
{ 
	
	chassis.cnt_fdb =  moto_chassis[0].total_ecd/chassis_ratio  -  chassis.cnt_offset;
	
	
	switch(chassis.ctrl_mode)
	{
		case CHASSIS_STOP:
		{
			chassis.vx = 0;
			chassis.vy = 0;
			chassis.vw = 0;
		}break;
		
		case CHASSIS_REMOTE_NORMAL:
		{
			chassis.vx = (float)rc.ch2*10;
			chassis.vy = (float)rc.ch1*10;
			chassis.vw = (float)rc.ch3*(-10);
		}break;
		
		case CHASSIS_REMOTE_SLOW:
		{
			chassis.vx = (float)rc.ch2*3;
			chassis.vy = (float)rc.ch1*3;
			chassis.vw = (float)rc.ch3*(-3);
		}break;
		
		case CHASSIS_KB:
		{		
			chassis_ramp();
			chassis.vx = (float)(chassis_x_ramp.out);
			chassis.vy = (float)(chassis_y_ramp.out);
			if(abs(rc.mouse.x)>10)
			{
				chassis.vw=chassis_mouse_w_ramp.out;
			}
			else
			chassis.vw = 0.3*(double)chassis_w_ramp.out;
		}break;
		
		case CHASSIS_RC_NEAR: //激光测距方案贴资源岛行走
		{
			
			chassis.vx = pid_calc(&pid_chassis_auto_x,(float)chassis.cnt_fdb,(float)chassis.cnt_ref); //自动底盘
			//先找方向
			chassis.vy = (float)rc.ch1*3;
			chassis.vw = 0;
			//chassis.vw = pid_calc(&pid_chassis_near_w,(float)relay.dis1-(float)relay.dis2,0);//调平齐
		}break;		
		
		case CHASSIS_KB_NEAR: //激光测距方案贴资源岛行走
		{
			chassis_ramp();
			chassis.vx = -pid_calc(&pid_chassis_near_x,(float)relay.dis1+(float)relay.dis2,chassis.dis_ref);//调距离
			chassis.vy = (float)(chassis_y_ramp.out);
			chassis.vw = pid_calc(&pid_chassis_near_w,(float)relay.dis1-(float)relay.dis2,0);//调平齐
		}break;	
		
//		case CHASSIS_KB_LIMIT_TOUCH: //限位开关方案贴资源岛行走
//		{
//			//limit equals 1 when unpressed
//			chassis.LIMIT_LEFT=HAL_GPIO_ReadPin(INDUCTIVE_LEFT_GPIO_Port,INDUCTIVE_LEFT_Pin);
//			chassis.LIMIT_RIGHT=HAL_GPIO_ReadPin(INDUCTIVE_RIGHT_GPIO_Port,INDUCTIVE_RIGHT_Pin);
//			if(chassis.LIMIT_LEFT||chassis.LIMIT_RIGHT)
//				{
//					chassis.vy=chassis.target_vy-300;
//				}
//				else chassis.vy=chassis.target_vy;
//				chassis.vx = chassis.target_vx;
//				chassis.vw = chassis.target_vw;
//		}
//		break;
		default:
		break;
	}		
	
	mecanum_calc(chassis.vx,chassis.vy, chassis.vw, chassis.wheel_spd_ref); //麦轮结算
	
	for (uint8_t i = 0; i < 4; i++)
  {
    chassis.wheel_spd_fdb[i] = moto_chassis[i].speed_rpm; //获取电机当前速度
  }
	
	for (int i = 0; i < 4; i++)
	{
		chassis.current[i] = (int16_t)pid_calc(&pid_chassis_spd[i], chassis.wheel_spd_fdb[i], chassis.wheel_spd_ref[i]); //PID速度单环
	}

	if(chassis.ctrl_mode==CHASSIS_STOP)	
	{
		pid_chassis_spd[0].iout = 0;
		pid_chassis_spd[1].iout = 0;
		pid_chassis_spd[2].iout = 0;
		pid_chassis_spd[3].iout = 0;
		for (int i = 0; i < 4; i++)
		{
			chassis.current[i] = 0;
		}
	}
	memcpy(motor_cur.chassis_cur, chassis.current, sizeof(chassis.current));
	
	osSignalSet(can_msg_send_task_t, CHASSIS_MOTOR_MSG_SEND);
}
/**
  * @brief 麦轮解算函数
  * @param input : ?=+vx(mm/s)  ?=+vy(mm/s)  ccw=+vw(deg/s)
  *        output: every wheel speed(rpm)
  * @note  1=FL 2=FR 3=BL 4=BR
  */
void mecanum_calc(float vx, float vy, float vw, int16_t speed[])
{
  int16_t wheel_rpm[4];
  float   max = 0;
  
  wheel_rpm[0] =     vx + vy - vw;
  wheel_rpm[1] = -1*(vx - vy + vw);
  wheel_rpm[2] =     vx - vy - vw;
  wheel_rpm[3] = -1*(vx + vy + vw);

	//find max item
  for (uint8_t i = 0; i < 4; i++)
  {
    if (abs(wheel_rpm[i]) > max)
      max = abs(wheel_rpm[i]);
  }
  //equal proportion
  if (max > MAX_WHEEL_RPM)
  {
    float rate = MAX_WHEEL_RPM / max;
    for (uint8_t i = 0; i < 4; i++)
      wheel_rpm[i] *= rate;
  }
	memcpy(speed, wheel_rpm, 4*sizeof(int16_t));
}


void chassis_init()
{
	for(int i=0; i<4; i++)
	{
		PID_struct_init(&pid_chassis_spd[i], POSITION_PID, 15000, 15000,
									3.0f,	0.05f,	0.0f	);  
	}
		PID_struct_init(&pid_chassis_angle, POSITION_PID, 4800, 500,
									4.0f,	0.0f,0.0f	);  
	
		PID_struct_init(&pid_chassis_near_w, POSITION_PID, 4800, 500,
									20.0f,	0.0f,100.0f	);  

		PID_struct_init(&pid_chassis_near_x, POSITION_PID, 4800, 500,
									5.0f,	0.0f,15.0f	);  
	
	  PID_struct_init(&pid_chassis_auto_x, POSITION_PID, 4800, 500,
									15.0f,	0.0f,0.0f	);  
		chassis.cnt_offset =  moto_chassis[0].total_ecd;
}
