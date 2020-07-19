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
#include "gimbal_task.h"
#include "pid.h"
#include "data_processing.h"
#include "bsp_motor.h"
#include "slip_task.h"

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



#define MOUSE_SPEED_SLOW		50
#define MOUSE_SPEED_NORMAL	100
#define MOUSE_SPEED_FAST		150

#define slope_scale 100
uint32_t handle_cnt=0,ready_step_flag=0;
uint32_t climb_handle_cnt=0,climb_ready_step_flag=0;

int speed_choose_x=1,speed_choose_y=1,speed_choose_w=1,speed_choose_m=1;
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
	switch (rc.sw1)
  {
		case RC_UP:
		{
			switch (rc.sw2){
				case RC_UP:
				{			
					//遥控底盘行走模式

					glb_ctrl_mode = RC_MOVE_MODE;
					rc_move_handle();
				}break;
				case RC_MI:
				{
					//遥控取弹給弹测试模式
					glb_ctrl_mode = RC_BULLET_MODE; //设置全局取弹模式
					//rc_bullet_handle();  //取弹遥控调试函数
					//get_bullet_ctrl_mode(); //取弹回调函数
				}break;
				case RC_DN:
				{
				
				}break;
			}
		}
		break;
		case RC_MI:
		{
			//保护
			glb_ctrl_mode = SAFETY_MODE;
			safety_mode_handle();			
		}
		break;
		case RC_DN:
		{
		//键盘模式
			glb_ctrl_mode = KB_MODE;
			kb_handle();
		}
		break;
		default:
		break;
  }
}
	
void safety_mode_handle(void)
{
	chassis.ctrl_mode = CHASSIS_STOP;
	uplift.ctrl_mode = UPLIFT_STOP;
	slip.ctrl_mode = SLIP_STOP;
	rotate.ctrl_mode = ROTATE_SAFE_MODE;
	
}
//遥控底盘移动模式下对遥控器的操作处理，对各个机构切换状态
void rc_move_handle(void)
{	
	slip.ctrl_mode = SLIP_ADJUST;   //横移电机可微调
	uplift.ctrl_mode = UPLIFT_ADJUST;					//抬升机构可微调，ch5
	chassis.ctrl_mode = CHASSIS_REMOTE_NORMAL;//底盘正常速度行走模式ch1.ch2,ch3
	//ch4打到最下卡扣，救援机构闭合，其他模式缩回
	if(rc.ch4<-500)
		pump.help_ctrl_mode = ON_MODE;
	else
		pump.help_ctrl_mode = OFF_MODE;
	//图传视角正中
	servo_pit_pwm = PIT_PWM_MID;
	servo_yaw_pwm = YAW_PWM_MID;	
	TIM1->CCR3 = servo_yaw_pwm;
	TIM1->CCR4 = servo_pit_pwm;
	//关闭涵道
	climb.jdq = 0;
	climb.pwm = 800;
	climb.jdq?HAL_GPIO_WritePin(TURBINE_JDQ_GPIO_Port,TURBINE_JDQ_Pin,GPIO_PIN_SET):
	HAL_GPIO_WritePin(TURBINE_JDQ_GPIO_Port,TURBINE_JDQ_Pin,GPIO_PIN_RESET);
	TIM3->CCR3 = climb.pwm;

}

//遥控测试模式下对遥控器的操作处理，对各个机构切换状态
void rc_bullet_handle(void)
{	
//取弹給弹测试模式
//视角自动给到取弹屏幕
//（ch4打到最下卡扣，取第二排模式，中间取第一排
//打到最上，给子弹
//ch3低速旋转  ch2低速前进  ch1低速横移
//ch5向上打 一次 进行取弹的下一次操作
//向下打一次 复位到取弹的最初始状态
	static uint8_t rc5_flag ;
	slip.ctrl_mode = SLIP_AUTO;
	uplift.ctrl_mode = UPLIFT_ADJUST; 
//	uplift.ctrl_mode = UPLIFT_AUTO;		//抬升机构不微调，自动到达指定位置
	chassis.ctrl_mode = CHASSIS_REMOTE_SLOW;	//底盘慢速移动
//	ch4打到最下卡扣，取第二排/岛上子弹模式
//					中间，取第一排模式
//					最上，给子弹模式
	if(rc.ch4<-500)
		func_mode = GET_BULLET2_MODE;
	else if (rc.ch4>-150&&rc.ch4<150)
		func_mode = GET_BULLET1_MODE;
	else if (rc.ch4>500)
		func_mode = GIVE_BULLET_MODE;
	
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
		if(func_mode==GET_BULLET1_MODE||func_mode==GET_BULLET2_MODE)
		{
			bullet.ctrl_mode = BULLET_RESET;
			rc5_flag = 0;		
		}
	}
	else if(rc5_flag==2&&rc.ch5>-250&&rc.ch5<250)
	{
		if(func_mode==GET_BULLET1_MODE||func_mode==GET_BULLET2_MODE)
		{
			bullet.step_flag = 1;	//取弹标志位进一
			rc5_flag = 0;
		}
	}
	
	if(func_mode == GIVE_BULLET_MODE)			//补弹   c抬到给弹高度 shift c 开弹仓 ctrl c 关弹仓
	{
		//传输子弹时爪子收起
		uplift.height_ref[0] = uplift.height_give;
		uplift.height_ref[1] = uplift.height_give;
	}	

	//图传视角对屏幕，屏幕视角为取弹
	servo_pit_pwm = PIT_PWM_LCD;
	servo_yaw_pwm = YAW_PWM_LCD;	
	TIM1->CCR3 = servo_yaw_pwm;
	TIM1->CCR4 = servo_pit_pwm;
	electrical.camera_ctrl_mode = OFF_MODE;

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
		climb.climb_up_mode = CLIMB_UP_DONE;
		climb.climb_down_mode = CLIMB_DOWN_DONE;
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

void get_bullet_ctrl_mode(void)
{
	if(chassis.ctrl_mode==CHASSIS_KB_NEAR)
	{
		if(func_mode==GET_BULLET1_MODE)
			chassis.dis_ref = bullet.dis_ref1;
		else if(func_mode==GET_BULLET2_MODE)
			chassis.dis_ref = bullet.dis_ref2;
	}
		
	switch (bullet.ctrl_mode)
	{
		case BULLET_N:
		{
			ready_step_flag = 1;
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				handle_cnt = 0;
				bullet.step_flag = 0;
				ready_step_flag = 0;
				bullet.ctrl_mode = BULLET_AIM;
			}			
		}
		break;
		case BULLET_AIM://对位
//			pump.help_ctrl_mode = OFF_MODE;	
			uplift.height_ref[0] = uplift.height_aim; 
			uplift.height_ref[1] = uplift.height_aim;
			if(ABS(pid_uplift_height[0].err[0])<1.5f&&ABS(pid_uplift_height[1].err[0])<1.5f)//对位成功
			{
				handle_cnt++;
				if(handle_cnt>5)
				{
					ready_step_flag = 1;
				}
			}
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				handle_cnt = 0;
				if(func_mode == GET_BULLET1_MODE)
					bullet.ctrl_mode = BULLET_ROTATE_OUT;
				else if(func_mode == GET_BULLET2_MODE)
					bullet.ctrl_mode = BULLET_STICK;
				bullet.step_flag = 0;
				ready_step_flag = 0;
			}			
			break;
			
		case BULLET_STICK://（取第二排模式）伸出悬架
			pump.bracket_ctrl_mode = ON_MODE;
			handle_cnt++;
			if(handle_cnt>20)
			{
				ready_step_flag = 1;
			}
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				handle_cnt = 0;
				bullet.ctrl_mode = BULLET_ROTATE_OUT;
				bullet.step_flag = 0;
				ready_step_flag = 0;
			}
			break;
			
		case BULLET_ROTATE_OUT://转动爪子，进入弹药箱
			pump.rotate_ctrl_mode = ON_MODE;
			handle_cnt++;
			if(handle_cnt>20)
			{
				ready_step_flag = 1;
			}
			
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				handle_cnt = 0;
				bullet.ctrl_mode = BULLET_PRESS;
				bullet.step_flag = 0;
				ready_step_flag = 0;
			}				
			break;

		case BULLET_PRESS://夹取弹药箱
			
			pump.press_ctrl_mode = ON_MODE;
			handle_cnt++;
			if(handle_cnt>5)
			{
				ready_step_flag = 1;
			}
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				handle_cnt = 0;
				bullet.ctrl_mode = BULLET_GET;
				bullet.step_flag = 0;
				ready_step_flag = 0;
			}			
			break;
		case BULLET_GET://抬升弹药箱取出
			uplift.height_ref[0] = uplift.height_get;
			uplift.height_ref[1] = uplift.height_get;
			if(pid_uplift_height[0].err[0]<0.8f&&pid_uplift_height[1].err[0]<0.8f)
			{
				handle_cnt++;
				if(handle_cnt>5)
				{
					ready_step_flag = 1;
				}	
			}
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				if(func_mode == GET_BULLET1_MODE)
					bullet.ctrl_mode = BULLET_PUSH;
				else if(func_mode == GET_BULLET2_MODE)
					bullet.ctrl_mode = BULLET_WITHDRAW;
				bullet.step_flag = 0;
				ready_step_flag = 0;
				handle_cnt = 0;
			}
			break;
		case BULLET_WITHDRAW://（取第二排模式）收回悬架
			pump.bracket_ctrl_mode = OFF_MODE;			
			handle_cnt++;
			if(handle_cnt>20)
			{
				ready_step_flag = 1;
			}	
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				bullet.ctrl_mode = BULLET_PUSH;
				bullet.step_flag = 0;
				ready_step_flag = 0;
				handle_cnt = 0;

			}				
			break;
		case BULLET_PUSH://倒子弹
			pump.rotate_ctrl_mode = OFF_MODE;
			handle_cnt++;
			if(handle_cnt>20)
			{
				ready_step_flag = 1;
			}	

			ready_step_flag = 1;
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				bullet.ctrl_mode = BULLET_THROW;
				bullet.step_flag = 0;
				ready_step_flag = 0;
				handle_cnt = 0;
			}					
			break;
		case BULLET_THROW://丢弹药箱
			pump.rotate_ctrl_mode = ON_MODE;
			handle_cnt++;
			if(handle_cnt>bullet.loosetime)
			{
					ready_step_flag = 1;
			}
			
			if(ready_step_flag==1)		//自动适应到松爪子
			{
				bullet.step_flag = 0;
				ready_step_flag = 0;
				handle_cnt = 0;
				bullet.ctrl_mode = BULLET_LOOSE;

			}			
			break;
		case BULLET_LOOSE:
				pump.press_ctrl_mode = OFF_MODE;
				handle_cnt++;
				if(handle_cnt>50)
				{
					ready_step_flag = 1;
				}
			
			if(ready_step_flag==1&&bullet.step_flag==1)
			{
				//有问题
				if(rc.kb.bit.A&&rc.kb.bit.D==0)	bullet.ctrl_mode = BULLET_DONE_LEFT;
				else if(rc.kb.bit.D&&rc.kb.bit.A==0)	bullet.ctrl_mode = BULLET_DONE_RIGHT;
				else if(rc.kb.bit.D==0&&rc.kb.bit.A==0)	bullet.ctrl_mode = BULLET_DONE_STAY;				
				bullet.step_flag = 0;
				ready_step_flag = 0;
				handle_cnt = 0;

			}			
			break;
		case BULLET_DONE_STAY://取弹完成
			pump.rotate_ctrl_mode = OFF_MODE;
			uplift.height_ref[0] = uplift.height_aim; 
			uplift.height_ref[1] = uplift.height_aim;

			handle_cnt++;
			if(handle_cnt>10)
			{
				ready_step_flag = 1;bullet.reset_flag=0;
			}
			if(ready_step_flag==1)
			{
				handle_cnt = 0;
				bullet.ctrl_mode = BULLET_AIM;
				bullet.step_flag = 0;
				ready_step_flag = 0;
			}
			break;
			
		case BULLET_DONE_LEFT://取弹完成
			pump.rotate_ctrl_mode = OFF_MODE;
			uplift.height_ref[0] = uplift.height_aim; 
			uplift.height_ref[1] = uplift.height_aim;

			handle_cnt++;
			if(handle_cnt>10)
			{
				ready_step_flag = 1;bullet.reset_flag=0;
			}
			if(ready_step_flag==1)
			{
				handle_cnt = 0;
				bullet.ctrl_mode = BULLET_AIM;
				bullet.step_flag = 0;
				ready_step_flag = 0;
			}
			break;		
			case BULLET_DONE_RIGHT://取弹完成
			pump.rotate_ctrl_mode = OFF_MODE;
			handle_cnt++;
			if(handle_cnt>10)
			{
				ready_step_flag = 1;bullet.reset_flag=0;
			}
			if(ready_step_flag==1)
			{
				handle_cnt = 0;
				bullet.ctrl_mode = BULLET_AIM;
				bullet.step_flag = 0;
				ready_step_flag = 0;
			}
			break;			
			
			//以上为循环
		case BULLET_RESET://机构复位

			pump.bracket_ctrl_mode = OFF_MODE;
			pump.press_ctrl_mode = OFF_MODE;	
			pump.rotate_ctrl_mode = OFF_MODE;
		  pump.magazine_ctrl_mode = OFF_MODE;
			bullet.reset_flag = 1;
			uplift.height_ref[0] = uplift.height_normal;
			uplift.height_ref[1] = uplift.height_normal;

			handle_cnt++;
			if(handle_cnt>50)
			{
				ready_step_flag = 1;
				bullet.reset_flag=0;
			}
			bullet.ctrl_mode = BULLET_N;
			break;

	}
}

void climb_up_ctrl(void)
{
	
}

void climb_down_ctrl(void)
{

}
void climb_ctrl_mode(void)
{

}
