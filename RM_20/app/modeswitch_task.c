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


extern osTimerId chassis_timer_id;
extern osTimerId uplift_timer_id;
extern osTimerId slip_timer_id;
extern osTimerId rotate_timer_id;
extern osTimerId judge_sendTimer_id;


#define PIT_PWM_DOWN_LIMTI	800
#define PIT_PWM_UP_LIMIT 		1650
#define YAW_PWM_LEFT_LIMIT	2500
#define YAW_PWM_RIGHT_LIMIT 500

#define PIT_PWM_MID	1170
#define PIT_PWM_LCD 1350
#define YAW_PWM_LCD 2117
#define YAW_PWM_MID 856

#define PIT_PWM_HELP 1563
#define YAW_PWM_HELP 835

float chassis_speed; //底盘速度选择 此定义在斜坡函数中会再次用到

int speed_normal=4000;
int speed_fast =8000;
int speed_slow =2000;

float servo_pit_pwm=1170;
float servo_yaw_pwm=2080;
//servo_yaw_pwm pwm值增大逆时针转 最左为500pwm，最右为2500

uint8_t rc_kb_X_flag=0;
uint8_t rc_kb_R_flag=0;
uint8_t rc_ch4_flag=0;
uint8_t ready_flag=0;


#define MOUSE_SPEED_SLOW		50
#define MOUSE_SPEED_NORMAL	100
#define MOUSE_SPEED_FAST		150

uint32_t handle_cnt=0,ready_step_flag=0;

uint32_t handle_fetch_time=0,handle_fetch_time2=0;

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
//					glb_ctrl_mode = RC_MOVE_MODE;
					rc_move_handle();
				}break;
				
				
				case RC_MI:
				{
					//遥控取弹测试模式					
					static uint8_t init_sign2=0;
					glb_ctrl_mode = DEBUG_MODE;
					chassis.ctrl_mode=CHASSIS_REMOTE_SLOW;
										
					if(!init_sign2 || last_glb_ctrl_mode == SAFETY_MODE || bullet_setp == VOID_HANDLE)
					{
						init_sign2=1;
					  chassis.cnt_offset =  moto_chassis[0].total_ecd/409.6f;
						chassis.cnt_fdb =  moto_chassis[0].total_ecd/409.6f  -  chassis.cnt_offset;
						chassis.cnt_ref = chassis.cnt_fdb;
					}//重置底盘
					chassis.ctrl_mode = CHASSIS_RC_NEAR;
					
//					glb_ctrl_mode = RC_BULLET_MODE; //设置全局取弹模式
					rc_bullet_handle();  //取弹遥控调试函数
					//get_bullet_ctrl_mode(); //取弹回调函数
					keyborad_bullet_handle_controller();
				}break;
				
				
				case RC_DN:
				{
					//调试pid
					
					static uint8_t init_sign=0;
					glb_ctrl_mode = DEBUG_MODE;
					
					slip.state = SLIP_KNOWN;
					slip.ctrl_mode = SLIP_AUTO;
					
					rotate.state = ROTATE_KNOWN;
					rotate.ctrl_mode = ROTATE_AUTO;
					
					
					if(!init_sign || last_glb_ctrl_mode == SAFETY_MODE)
					{
						
						init_sign=1;
					  chassis.cnt_offset =  moto_chassis[0].total_ecd/409.6f;
						chassis.cnt_fdb =  moto_chassis[0].total_ecd/409.6f  -  chassis.cnt_offset;
						chassis.cnt_ref = chassis.cnt_fdb;
						
						rotate.ecd_fdb = moto_rotate[0].total_ecd - rotate.ecd_offset;
						rotate.cnt_fdb = -rotate.ecd_fdb/82.0f;
						rotate.cnt_ref = rotate.cnt_fdb; 
						
					}//重置
					chassis.ctrl_mode = CHASSIS_RC_NEAR;					
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
			
		//kb_handle();
			
			keyboard_handle();
		}break;
		
		
		default:
		break;
  }
	#else
	
	keyboard_handle();	
		
	#endif
}


//遥控底盘移动模式下对遥控器的操作处理，对各个机构切换状态
void rc_move_handle(void)
{	
	slip.ctrl_mode = SLIP_ADJUST;   					//横移电机可微调
	uplift.ctrl_mode = UPLIFT_ADJUST;					//抬升机构可微调，ch5
	
	uplift.ctrl_mode = UPLIFT_ADJUST;
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
			ready_flag = 0;
			bullet_setp = BULLET_SAFETY_MODE;
			rc5_flag = 0;				
	}
	else if(rc5_flag==2&&rc.ch5>-250&&rc.ch5<250)
	{
			ready_flag = 1;	//取弹标志位进一
			rc5_flag = 0;		
	}
}



//键盘模式下的操作处理，对各个机构切换状态
void kb_handle(void)
{
	float mouse_speed;
	
	//uplift.ctrl_mode = UPLIFT_AUTO;
	uplift.ctrl_mode = UPLIFT_ADJUST; //抬升暂时未调试
	chassis.ctrl_mode = CHASSIS_KB;	

	if(rc.mouse.l)		//	视角	r正常 shift r取弹 ctrl r 登岛
	{
		//两轴云台归中
		if(rc.kb.bit.SHIFT)
		{
			servo_pit_pwm = PIT_PWM_LCD;
			servo_yaw_pwm = YAW_PWM_LCD;
			electrical.camera_ctrl_mode = OFF_MODE;
			camera_mode = CAMERA_BULLET_MODE;
			camera_flag = -1;
		}
		else if(rc.kb.bit.CTRL)
		{
			servo_pit_pwm = PIT_PWM_LCD;
			servo_yaw_pwm = YAW_PWM_LCD;
			electrical.camera_ctrl_mode = ON_MODE;
			camera_mode = CAMERA_CLIMB_MODE;
			camera_flag = -1;
		}
		else
		{
			servo_pit_pwm = PIT_PWM_MID;
			servo_yaw_pwm = YAW_PWM_MID;
			camera_mode = CAMERA_NORMAL_MODE;
			camera_flag = 1;
		}
	}	
	if(rc.mouse.r) //未调试过
	{
			//启动挪动鼠标移动云台操作
			relay.view_tx.yaw 	-= 0.5*rc.mouse.x;
			relay.view_tx.pitch	-= 0.3*rc.mouse.y;
			if(relay.view_tx.yaw > 2200) relay.view_tx.yaw = 2200;
		  if(relay.view_tx.yaw < 850) relay.view_tx.yaw = 850;
			if(relay.view_tx.pitch > 1600) relay.view_tx.pitch = 1600;
		  if(relay.view_tx.pitch < 800) relay.view_tx.pitch = 800;
			//HAL_GPIO_TogglePin(LED_A_GPIO_Port,LED_A_Pin);g
	}
	else if(rc.kb.bit.G)
	{
			relay.view_tx.yaw 	= 1480;
			relay.view_tx.pitch	= 1150;
		
		//图传初始状态
	}


	if(rc.kb.bit.CTRL)	{chassis_speed = 1800; mouse_speed = MOUSE_SPEED_SLOW;}
	else if(rc.kb.bit.SHIFT)	{chassis_speed = 8200; mouse_speed = MOUSE_SPEED_FAST;}
	else 								{chassis_speed = 3800; mouse_speed = MOUSE_SPEED_NORMAL;}

	
	if(rc.sw2==RC_UP)func_mode = GET_BULLET1_MODE;
	else if(rc.sw2==RC_MI)func_mode = GET_BULLET2_MODE;

	if(rc.kb.bit.Z)	//各种复位		z
	{
		bullet.ctrl_mode = BULLET_RESET;
	}
	if(rc.kb.bit.X&&rc_kb_X_flag==0)	//取弹下一步   x
	{
		rc_kb_X_flag = 1;
		if(rc.sw2==RC_UP)func_mode = GET_BULLET1_MODE;
		else if(rc.sw2==RC_MI)func_mode = GET_BULLET2_MODE;
//		if(rc.kb.bit.SHIFT)	func_mode = GET_BULLET2_MODE;
//		else func_mode = GET_BULLET1_MODE;
	}
	if(rc.kb.bit.X==0&&rc_kb_X_flag)
	{
		rc_kb_X_flag = 0;
		bullet.step_flag = 1;
	}
}

void get_global_last_mode(void)
{
	last_glb_ctrl_mode = glb_ctrl_mode;
	last_func_mode = func_mode;
}


void keyborad_bullet_handle_controller(void)
{	
	switch(bullet_setp)
	{
	case VOID_HANDLE:
	{
		if(ready_flag)
		{
			slip.ctrl_mode = SLIP_AUTO;
			rotate.ctrl_mode = ROTATE_AUTO;
			if(slip.state==SLIP_KNOWN && rotate.state==ROTATE_KNOWN)
			{
				ready_flag = 0;
				bullet_setp = LEFT_POS;
			}
		}
	}break;
	
	case LEFT_POS:
	{
		if(ready_flag==1)
		{
			slip.dist_ref = 5.2f;
			rotate.cnt_ref = 600;
			if(fabs((float)(slip.dist_fdb-5.2f))<10)
			{
				ready_flag=0;
			  bullet_setp = ROT_OUT;
				pump.press_ctrl_mode = ON_MODE;
				
				
//				/*自动进入下一步*/
//				ready_flag=1;
//				/*自动进入下一步*/
				
				
			}
			else if(fabs((double)(slip.dist_fdb))<420)
			{
				pump.press_ctrl_mode = ON_MODE;
			}
		}

	}break;
	
	case ROT_OUT:
	{
		if(ready_flag==1)
		{
			
			
			rotate.cnt_ref = 1000;
			
			if(fabs((double)(rotate.cnt_fdb-1000))<40)
			{
				ready_flag=0;
				bullet_setp = ROT_OFF;
				handle_fetch_time=0;
				
//			/*自动进入下一步*/
//			ready_flag=1;
//			/*自动进入下一步*/
				
			}
		}

	}break;
	case ROT_OFF:
	{
		if(ready_flag==1)
		{
			handle_fetch_time++;
			pump.press_ctrl_mode =OFF_MODE;
			
			if(handle_fetch_time>65)
			{
				rotate.cnt_ref = 170;
				if(fabs((double)(rotate.cnt_fdb-170))<15)
				{
					ready_flag=0;
					bullet_setp = TAKE_OFF_AND_OUT;
					handle_fetch_time=0;

//					/*自动进入下一步*/
//					ready_flag=1;
//					/*自动进入下一步*/
				}
				else if(fabs((double)(rotate.cnt_fdb-170))<120)
				{
					slip.dist_ref = 493.6f;
				}						
		 }
			
		}

	}break;
	
	case TAKE_OFF_AND_OUT:
	{
		
		slip.dist_ref = 493.6f;
			if(ready_flag==1 && fabs((float)(slip.dist_fdb-493.6f))<15)
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
					ready_flag=0;
					bullet_setp = ROT_OFF_MID;
					
//					/*自动进入下一步*/
//					ready_flag=1;
//					/*自动进入下一步*/
				}			
			}			
		}
	}break;
	
	
	case ROT_OFF_MID:
	{
		if(ready_flag==1)
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
					ready_flag=0;
					bullet_setp = TAKE_OFF_AND_OUT_MID;
					handle_fetch_time=0;
					handle_fetch_time2=0;
					
//					/*自动进入下一步*/
//					ready_flag=1;
//					/*自动进入下一步*/
				}
				else if(fabs((double)(rotate.cnt_fdb-170))<120)
				{
					slip.dist_ref = 992.6f;
				}	
			}
			
		}
	}break;
	
	case TAKE_OFF_AND_OUT_MID:
	{
		slip.dist_ref = 992.4f;
		if(ready_flag==1 && fabs((float)(slip.dist_fdb-992.6f))<15)
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
					ready_flag=0;
					bullet_setp = ROT_OFF_FINAL;	
					
//					/*自动进入下一步*/
//					ready_flag=1;
//					/*自动进入下一步*/
				}
			}
			
		}

	}break;
	
	case ROT_OFF_FINAL:
	{
	
			if(ready_flag==1)
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
				ready_flag=0;
				bullet_setp = TAKE_OFF_FINAL;
				handle_fetch_time=0;
				handle_fetch_time2=0;
				
				
//				/*自动进入下一步*/
//				ready_flag=1;
//				/*自动进入下一步*/				
			}
			else if(fabs((double)(rotate.cnt_fdb-170))<120)
			{
				slip.dist_ref = 493.5f;
			}	
		}
			
		}
	
	}break;
	
	case TAKE_OFF_FINAL:
	{
	
			slip.dist_ref = 493.5f;
					if(ready_flag==1 )
		{
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
							ready_flag=0;
							handle_fetch_time=0;
							handle_fetch_time2=0;
							bullet_setp = BULLET_HOLD_ON;
								
//							/*自动进入下一步*/
//							ready_flag=1;
//							/*自动进入下一步*/					
						}			
				}
			}
		}
	}break;
	
	case BULLET_HOLD_ON:
	{		
		if(ready_flag)
		{
			bullet_setp = BULLET_RESET_STEP;
			ready_flag=0;
		}	
	}break;
	
	case BULLET_RESET_STEP:
	{

		uplift.ctrl_mode = UPLIFT_STOP;
		slip.ctrl_mode = SLIP_STOP;
		rotate.ctrl_mode = ROTATE_STOP;
		
		pump.press_ctrl_mode = OFF_MODE;
		pump.throw_ctrl_mode = OFF_MODE;
				
		rotate.state = ROTATE_UNKNOWN;
		rotate.ecd_offset = moto_rotate[0].total_ecd;
		rotate.ecd_offset=0;
		rotate.cnt_ref=0;
		slip.state = SLIP_UNKNOWN;
		slip.dist_offset=0;
		
		if(ready_flag)
		{	
			bullet_setp = VOID_HANDLE;
			
			ready_flag=0;
			handle_fetch_time2=0;
			handle_fetch_time=0;			
		}
		
	}break;
	
	case BULLET_SAFETY_MODE:
	{
		chassis.ctrl_mode = CHASSIS_STOP;
		uplift.ctrl_mode = UPLIFT_STOP;
		slip.ctrl_mode = SLIP_STOP;
		rotate.ctrl_mode = ROTATE_STOP;
		
		pump.press_ctrl_mode = OFF_MODE;
		pump.throw_ctrl_mode = OFF_MODE;

	}break;
	
	}

}


