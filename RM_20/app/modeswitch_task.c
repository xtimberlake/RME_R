/** 
  * @file modeswitch_task.c
  * @version 1.0
  * @date Nar,1 2018
	*
  * @brief  ģʽѡ�������ļ�
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

float chassis_speed; //�����ٶ�ѡ�� �˶�����б�º����л��ٴ��õ�

int speed_normal=4000;
int speed_fast =8000;
int speed_slow =2000;

float servo_pit_pwm=1170;
float servo_yaw_pwm=2080;
//servo_yaw_pwm pwmֵ������ʱ��ת ����Ϊ500pwm������Ϊ2500

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
		//ȫ�ֿ���ģʽ�£����ң�����������зּ�
		//�������ϵĲ����ٶԸ���ִ�л����л�״̬
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
					//ң�ص�������ģʽ

					glb_ctrl_mode = RC_MOVE_MODE;
					rc_move_handle();
				}break;
				case RC_MI:
				{
					//ң��ȡ���o������ģʽ
					glb_ctrl_mode = RC_BULLET_MODE; //����ȫ��ȡ��ģʽ
					//rc_bullet_handle();  //ȡ��ң�ص��Ժ���
					//get_bullet_ctrl_mode(); //ȡ���ص�����
				}break;
				case RC_DN:
				{
				
				}break;
			}
		}
		break;
		case RC_MI:
		{
			//����
			glb_ctrl_mode = SAFETY_MODE;
			safety_mode_handle();			
		}
		break;
		case RC_DN:
		{
		//����ģʽ
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
//ң�ص����ƶ�ģʽ�¶�ң�����Ĳ��������Ը��������л�״̬
void rc_move_handle(void)
{	
	slip.ctrl_mode = SLIP_ADJUST;   //���Ƶ����΢��
	uplift.ctrl_mode = UPLIFT_ADJUST;					//̧��������΢����ch5
	chassis.ctrl_mode = CHASSIS_REMOTE_NORMAL;//���������ٶ�����ģʽch1.ch2,ch3
	//ch4�����¿��ۣ���Ԯ�����պϣ�����ģʽ����
	if(rc.ch4<-500)
		pump.help_ctrl_mode = ON_MODE;
	else
		pump.help_ctrl_mode = OFF_MODE;
	//ͼ���ӽ�����
	servo_pit_pwm = PIT_PWM_MID;
	servo_yaw_pwm = YAW_PWM_MID;	
	TIM1->CCR3 = servo_yaw_pwm;
	TIM1->CCR4 = servo_pit_pwm;
	//�رպ���
	climb.jdq = 0;
	climb.pwm = 800;
	climb.jdq?HAL_GPIO_WritePin(TURBINE_JDQ_GPIO_Port,TURBINE_JDQ_Pin,GPIO_PIN_SET):
	HAL_GPIO_WritePin(TURBINE_JDQ_GPIO_Port,TURBINE_JDQ_Pin,GPIO_PIN_RESET);
	TIM3->CCR3 = climb.pwm;

}

//ң�ز���ģʽ�¶�ң�����Ĳ��������Ը��������л�״̬
void rc_bullet_handle(void)
{	
//ȡ���o������ģʽ
//�ӽ��Զ�����ȡ����Ļ
//��ch4�����¿��ۣ�ȡ�ڶ���ģʽ���м�ȡ��һ��
//�����ϣ����ӵ�
//ch3������ת  ch2����ǰ��  ch1���ٺ���
//ch5���ϴ� һ�� ����ȡ������һ�β���
//���´�һ�� ��λ��ȡ�������ʼ״̬
	static uint8_t rc5_flag ;
	slip.ctrl_mode = SLIP_AUTO;
	uplift.ctrl_mode = UPLIFT_ADJUST; 
//	uplift.ctrl_mode = UPLIFT_AUTO;		//̧��������΢�����Զ�����ָ��λ��
	chassis.ctrl_mode = CHASSIS_REMOTE_SLOW;	//���������ƶ�
//	ch4�����¿��ۣ�ȡ�ڶ���/�����ӵ�ģʽ
//					�м䣬ȡ��һ��ģʽ
//					���ϣ����ӵ�ģʽ
	if(rc.ch4<-500)
		func_mode = GET_BULLET2_MODE;
	else if (rc.ch4>-150&&rc.ch4<150)
		func_mode = GET_BULLET1_MODE;
	else if (rc.ch4>500)
		func_mode = GIVE_BULLET_MODE;
	
	if(rc.ch5>500)//���²���һ�Ρ��ص���λ̬
	{
		rc5_flag = 1;
	}
	else if(rc.ch5<-500)//���ϲ�����������һ��״̬
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
			bullet.step_flag = 1;	//ȡ����־λ��һ
			rc5_flag = 0;
		}
	}
	
	if(func_mode == GIVE_BULLET_MODE)			//����   ç�������߶� shift c ������ ctrl c �ص���
	{
		//�����ӵ�ʱצ������
		uplift.height_ref[0] = uplift.height_give;
		uplift.height_ref[1] = uplift.height_give;
	}	

	//ͼ���ӽǶ���Ļ����Ļ�ӽ�Ϊȡ��
	servo_pit_pwm = PIT_PWM_LCD;
	servo_yaw_pwm = YAW_PWM_LCD;	
	TIM1->CCR3 = servo_yaw_pwm;
	TIM1->CCR4 = servo_pit_pwm;
	electrical.camera_ctrl_mode = OFF_MODE;

}



//����ģʽ�µĲ��������Ը��������л�״̬
void kb_handle(void)
{
	float mouse_speed;
	
	//uplift.ctrl_mode = UPLIFT_AUTO;
	uplift.ctrl_mode = UPLIFT_ADJUST; //̧����ʱδ����
	chassis.ctrl_mode = CHASSIS_KB;	

	if(rc.mouse.l)		//	�ӽ�	r���� shift rȡ�� ctrl r �ǵ�
	{
		//������̨����
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
	if(rc.mouse.r) //δ���Թ�
	{
			//����Ų������ƶ���̨����
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
		
		//ͼ����ʼ״̬
	}


	if(rc.kb.bit.CTRL)	{chassis_speed = 1800; mouse_speed = MOUSE_SPEED_SLOW;}
	else if(rc.kb.bit.SHIFT)	{chassis_speed = 8200; mouse_speed = MOUSE_SPEED_FAST;}
	else 								{chassis_speed = 3800; mouse_speed = MOUSE_SPEED_NORMAL;}

	
	if(rc.sw2==RC_UP)func_mode = GET_BULLET1_MODE;
	else if(rc.sw2==RC_MI)func_mode = GET_BULLET2_MODE;

	if(rc.kb.bit.Z)	//���ָ�λ		z
	{
		bullet.ctrl_mode = BULLET_RESET;
		climb.climb_up_mode = CLIMB_UP_DONE;
		climb.climb_down_mode = CLIMB_DOWN_DONE;
	}
	if(rc.kb.bit.X&&rc_kb_X_flag==0)	//ȡ����һ��   x
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
		case BULLET_AIM://��λ
//			pump.help_ctrl_mode = OFF_MODE;	
			uplift.height_ref[0] = uplift.height_aim; 
			uplift.height_ref[1] = uplift.height_aim;
			if(ABS(pid_uplift_height[0].err[0])<1.5f&&ABS(pid_uplift_height[1].err[0])<1.5f)//��λ�ɹ�
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
			
		case BULLET_STICK://��ȡ�ڶ���ģʽ���������
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
			
		case BULLET_ROTATE_OUT://ת��צ�ӣ����뵯ҩ��
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

		case BULLET_PRESS://��ȡ��ҩ��
			
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
		case BULLET_GET://̧����ҩ��ȡ��
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
		case BULLET_WITHDRAW://��ȡ�ڶ���ģʽ���ջ�����
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
		case BULLET_PUSH://���ӵ�
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
		case BULLET_THROW://����ҩ��
			pump.rotate_ctrl_mode = ON_MODE;
			handle_cnt++;
			if(handle_cnt>bullet.loosetime)
			{
					ready_step_flag = 1;
			}
			
			if(ready_step_flag==1)		//�Զ���Ӧ����צ��
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
				//������
				if(rc.kb.bit.A&&rc.kb.bit.D==0)	bullet.ctrl_mode = BULLET_DONE_LEFT;
				else if(rc.kb.bit.D&&rc.kb.bit.A==0)	bullet.ctrl_mode = BULLET_DONE_RIGHT;
				else if(rc.kb.bit.D==0&&rc.kb.bit.A==0)	bullet.ctrl_mode = BULLET_DONE_STAY;				
				bullet.step_flag = 0;
				ready_step_flag = 0;
				handle_cnt = 0;

			}			
			break;
		case BULLET_DONE_STAY://ȡ�����
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
			
		case BULLET_DONE_LEFT://ȡ�����
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
			case BULLET_DONE_RIGHT://ȡ�����
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
			
			//����Ϊѭ��
		case BULLET_RESET://������λ

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
