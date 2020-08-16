/** 
  * @file modeswitch_task.c
  * @version 1.0
  * @date Nar,1 2018
	*
  * @brief  模式选择任务文件
	*
  *	@author li zh
  *
  */
#define __MODESW_TASK_GLOBALS
#include "modeswitch_task.h"
#include "chassis_task.h"
#include "uplift_task.h"
#include "rotate_task.h"
#include "cmsis_os.h"
#include "remote_msg.h"
#include "bsp_can.h"
#include "pid.h"
#include "data_processing.h"
#include "bsp_motor.h"
#include "slip_task.h"
#include "keyboard_handle.h"
#include "math.h"

#define I_have_a_remote 1

float chassis_speed; //底盘速度选择 此定义在斜坡函数中会再次用到

uint32_t handle_fetch_time=0;
uint32_t handle_fetch_time2=0;

void mode_switch_task(void const *argu)
{

  uint32_t mode_wake_time = osKernelSysTick();
	
	for(;;)
	{
		taskENTER_CRITICAL();
		//全局控制模式下，针对遥控器与鼠标进行分级
		//依照以上的操作再对各个执行机构切换状态
		get_main_ctrl_mode();    
    get_global_last_mode();
	  taskEXIT_CRITICAL();

		osDelayUntil(&mode_wake_time, INFO_GET_PERIOD);
	}
}

void get_main_ctrl_mode(void)
{
	#if(I_have_a_remote)
	switch (rc.sw1)
  {
		case RC_UP:
		{
			
			switch (rc.sw2)
			{
				case RC_UP:
				{			
					//遥控底盘行走模式
					glb_ctrl_mode = RC_MOVE_MODE;
					rc_move_handle();
				}break;
				
				
				case RC_MI:
				{
					//遥控取弹调试模式					
					glb_ctrl_mode = DEBUG_MODE;
					chassis.ctrl_mode=CHASSIS_STAY;
					
//					glb_ctrl_mode = RC_BULLET_MODE; //设置全局取弹模式
//					rc_bullet_handle();  //取弹遥控调试函数
//					get_bullet_front();
					get_bullet_single();
				}break;
				
				
				case RC_DN:
				{
//				//调试模式/初始化全部
					
					slip.state = SLIP_KNOWN;
					slip.ctrl_mode = SLIP_AUTO;
					
					rotate.state = ROTATE_KNOWN;
					rotate.ctrl_mode = ROTATE_AUTO;

					chassis.ctrl_mode = CHASSIS_STAY;
//					chassis_speed = SPEED_NORMAL;
//					chassis.ctrl_mode = CHASSIS_KB;					
				}break;
			}
			
		}break;
		
		
		case RC_MI:
		{
			//保护
			glb_ctrl_mode = SAFETY_MODE;
			safety_mode_handle();			
		}break;
		
		
		case RC_DN:
		{
			//键盘模式
			glb_ctrl_mode = KB_MODE;	
			if(last_glb_ctrl_mode != KB_MODE)
			{
				rotate.ctrl_mode = ROTATE_AUTO;
				slip.ctrl_mode = SLIP_AUTO;
				uplift.ctrl_mode = UPLIFT_AUTO;
			}
			keyboard_handle();
		}break;
		
		
		default:
		break;
  }
	#else
	
	keyboard_handle();	
		
	#endif
}

void get_global_last_mode(void)
{
	last_glb_ctrl_mode = glb_ctrl_mode;
	last_func_mode = func_mode;
}

//遥控底盘移动模式下对遥控器的操作处理，对各个机构切换状态
void rc_move_handle(void)
{	
	slip.ctrl_mode = SLIP_ADJUST;   					//横移电机可微调
	uplift.ctrl_mode = UPLIFT_ADJUST;					//抬升机构可微调，ch5
	
	chassis.ctrl_mode = CHASSIS_REMOTE_NORMAL;//底盘正常速度行走模式ch1.ch2,ch3
	
//	//给弹
//	if(rc.ch4>500)
//		pump.bullet1_ctrl_mode = ON_MODE;
//	else
//		pump.bullet1_ctrl_mode = OFF_MODE;
	
	
//	//救援
//	if(rc.ch4<-500)
//		pump.help_ctrl_mode = ON_MODE;
//	else
//		pump.help_ctrl_mode = OFF_MODE;


//	//图传视角正中
//	relay.view_tx.pitch = PIT_PWM_MID;
//	relay.view_tx.yaw = YAW_PWM_MID;	
}


//安全模式
void safety_mode_handle(void)
{
	chassis.ctrl_mode = CHASSIS_STOP;
	uplift.ctrl_mode = UPLIFT_STOP;
	slip.ctrl_mode = SLIP_STOP;
	rotate.ctrl_mode = ROTATE_STOP;
}

void rc_bullet_handle(void)
{	
	static uint8_t rc5_flag ;
	
	if(rc.ch5>500)//往下拨动一次。回到对位态
	{
		rc5_flag = 1;
	}
	else if(rc.ch5<-500)//往上拨动，开启下一个状态
	{
		rc5_flag = 2;
	}
	if(rc5_flag==1&&rc.ch5>-250&&rc.ch5<250)
	{
		safety_mode_handle();
		pump.press_ctrl_mode = OFF_MODE;
		pump.throw_ctrl_mode = OFF_MODE;
		rc5_flag = 0;				
	}
	else if(rc5_flag==2&&rc.ch5>-250&&rc.ch5<250)
	{
			rc5_flag = 0;		
	}
}


/**
  * @brief  自动取一箱子弹
  * @param  void
  * @retval void
  * @attention  当调用这个函数,bullet_step_single为SINGLE_READY则开始取单箱弹
  */
void get_bullet_single(void)
{
	switch(bullet_step_single)
	{
		case SINGLE_READY:
		{
			rotate.cnt_ref = 600;
			
			if(fabs((float)(rotate.cnt_fdb-600.0f))<10)
			{
				bullet_step_single = ROT_OUT;
				pump.press_ctrl_mode = ON_MODE;						
			}
			else if(rotate.cnt_fdb>300)
			{
				pump.press_ctrl_mode = ON_MODE;
			}
			
		}break;

		case ROT_OUT:
		{
			rotate.cnt_ref = 1000;
			if(fabs((double)(rotate.cnt_fdb-1000))<40)
			{
				bullet_step_single = ROT_OFF;
				handle_fetch_time=0;
			}
		}break;
		
		case ROT_OFF:
		{
			handle_fetch_time++;
			pump.press_ctrl_mode =OFF_MODE;
			
			if(handle_fetch_time>65)
			{
				rotate.cnt_ref = 170;
				
				if(fabs((double)(rotate.cnt_fdb-170))<15)
				{
					bullet_step_single = TAKE_OFF;
					handle_fetch_time=0;
				}					
		 }
		}break;
		
		case TAKE_OFF:
		{
			handle_fetch_time++;
			pump.press_ctrl_mode = ON_MODE;
				
			if(handle_fetch_time>58)
			{
				pump.throw_ctrl_mode = ON_MODE;
				rotate.cnt_ref = 400;				
					
				if(fabs((float)(rotate.cnt_fdb-400))<40)
				{	
					handle_fetch_time=0;
					bullet_step_single = BULLET_RESET_STEP;					
				}			
			}	
			
		}break;
		
		case BULLET_RESET_STEP:
		{
			rotate.cnt_ref = 600;	
			handle_fetch_time=0;
			pump.throw_ctrl_mode = OFF_MODE;
			pump.press_ctrl_mode = OFF_MODE;
			bullet_step_single = VOID_HANDLE;
		}break;
				
		default:
		break;
	}
}

/**
  * @brief  自动取前面三箱子弹
  * @param  void
  * @retval void
  * @attention  当调用这个函数,把bullet_step_front改为AIM_BULLET后进入取弹瞄准,把bullet_flag1置1开始取弹
  */
void get_bullet_front(void)
{	
	switch(bullet_step_front)
	{
		case AIM_BULLET:
		{			
			slip.dist_ref = 5.2f;
			rotate.cnt_ref = 600;
			
			if(fabs((float)(rotate.cnt_ref-600.0f))<40)
			{pump.press_ctrl_mode = ON_MODE;}
//			chassis.ctrl_mode = CHASSIS_RC_NEAR;//激光贴墙对位
			chassis.ctrl_mode = CHASSIS_REMOTE_SLOW;
			
		}break;
		
//		case LEFT_POS:
//		{

//			chassis.ctrl_mode = CHASSIS_STAY;
////			rotate.cnt_ref = 600;
//			
//			if(fabs((float)(slip.dist_fdb-5.2f))<10)
//			{
////					bullet_flag1=0;
//				bullet_step_front = ROT_OUT_LEFT;
//				pump.press_ctrl_mode = ON_MODE;
//						
//			}
//			else if(fabs((double)(slip.dist_fdb))<420)
//			{
//				pump.press_ctrl_mode = ON_MODE;
//			}
//				
//		}break;
		
		case ROT_OUT_LEFT:
		{
		
			rotate.cnt_ref = 1000;
			
			if(fabs((double)(rotate.cnt_fdb-1000))<40)
			{
				bullet_step_front = ROT_OFF_LEFT;
				handle_fetch_time=0;
			}

		}break;
		
		case ROT_OFF_LEFT:
		{
			
			handle_fetch_time++;
			pump.press_ctrl_mode =OFF_MODE;
			
			if(handle_fetch_time>65)
			{
				rotate.cnt_ref = 170;
				
				if(fabs((double)(rotate.cnt_fdb-170))<15)
				{
					bullet_step_front = TAKE_OFF_AND_OUT;
					handle_fetch_time=0;
				}
				else if(fabs((double)(rotate.cnt_fdb-170))<120)
				{
					slip.dist_ref = 493.6f;
				}						
		 }

		}break;
		
		case TAKE_OFF_AND_OUT:
		{
			
			slip.dist_ref = 493.6f;
			
			if(fabs((float)(slip.dist_fdb-493.6f))<15)
			{
				handle_fetch_time++;
				pump.press_ctrl_mode = ON_MODE;
				
				if(handle_fetch_time>58)
				{
					pump.throw_ctrl_mode = ON_MODE;
					rotate.cnt_ref = 1000;				
						
					if(fabs((float)(rotate.cnt_fdb-1000))<40)
					{			
						handle_fetch_time2=0;
						handle_fetch_time=0;
						bullet_step_front = ROT_OFF_MID;
						
					}			
				}			
			}
		}break;
		
		
		case ROT_OFF_MID:
		{
			
			handle_fetch_time++;
			pump.press_ctrl_mode =OFF_MODE;
			
			if(handle_fetch_time>35)
			pump.throw_ctrl_mode = OFF_MODE;

			if(handle_fetch_time>70)
			{
				rotate.cnt_ref = 170;
				
				if(fabs((double)(rotate.cnt_fdb-170))<15)
				{			
					bullet_step_front = TAKE_OFF_AND_OUT_MID;
					handle_fetch_time=0;
					handle_fetch_time2=0;
					
				}
				else if(fabs((double)(rotate.cnt_fdb-170))<120)
				{
					slip.dist_ref = 992.6f;
				}	
			}
						
		}break;
		
		case TAKE_OFF_AND_OUT_MID:
		{
			slip.dist_ref = 992.4f;
			
			if(fabs((float)(slip.dist_fdb-992.6f))<15)
			{
				handle_fetch_time++;
				pump.press_ctrl_mode = ON_MODE;
				
				if(handle_fetch_time>58)
				{
					pump.throw_ctrl_mode = ON_MODE;
					rotate.cnt_ref = 1000;
				
					
					if(fabs((double)(rotate.cnt_fdb-1000))<20)
					{					
						handle_fetch_time2=0;
						handle_fetch_time=0;
						bullet_step_front = ROT_OFF_FINAL;	
						
					}
				}
				
			}

		}break;
		
		case ROT_OFF_FINAL:
		{
		
				handle_fetch_time++;
				pump.press_ctrl_mode =OFF_MODE;
				
				if(handle_fetch_time>35)
				pump.throw_ctrl_mode = OFF_MODE;
				
				if(handle_fetch_time>70)
				{
				rotate.cnt_ref = 170;
					
				if(fabs((double)(rotate.cnt_fdb-170))<15)
				{
					bullet_step_front = TAKE_OFF_FINAL;
					handle_fetch_time=0;
					handle_fetch_time2=0;
						
				}
				else if(fabs((double)(rotate.cnt_fdb-170))<120)
				{
					slip.dist_ref = 493.5f;
				}	
			}
				
		
		}break;
		
		case TAKE_OFF_FINAL:
		{
		
			slip.dist_ref = 493.5f;
			
			handle_fetch_time++;
			pump.press_ctrl_mode = ON_MODE;
			
			if(handle_fetch_time>58)
			{
				pump.throw_ctrl_mode = ON_MODE;
				rotate.cnt_ref = 500;		
				
				if(fabs((double)(rotate.cnt_fdb-500))<50)
				{
						handle_fetch_time2++;
						
					
						if(handle_fetch_time2>60)
						{
							pump.throw_ctrl_mode = OFF_MODE;
							rotate.cnt_ref = 200;
							handle_fetch_time=0;
							handle_fetch_time2=0;
							bullet_step_front = BULLET_RESET_STEP;												
						}			
				}
			}
			
		}break;
		
		
		case BULLET_RESET_STEP:
		{
//			uplift.ctrl_mode = UPLIFT_STOP;
//			slip.ctrl_mode = SLIP_STOP;
//			rotate.ctrl_mode = ROTATE_STOP;
			
			pump.press_ctrl_mode = OFF_MODE;
			pump.throw_ctrl_mode = OFF_MODE;

			bullet_step_front = VOID_HANDLE;
			handle_fetch_time2=0;
			handle_fetch_time=0;			
			
		}break;
		
		default:
		break;
		
	}

}


