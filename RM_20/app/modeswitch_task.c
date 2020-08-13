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

uint32_t handle_cnt=0,ready_step_flag=0;

uint32_t handle_fetch_time=0,handle_fetch_time2=0;

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
	#if(I_have_a_remote)
	switch (rc.sw1)
  {
		case RC_UP:
		{
			
			switch (rc.sw2)
			{
				case RC_UP:
				{			
					//ң�ص�������ģʽ
//					glb_ctrl_mode = RC_MOVE_MODE;
					rc_move_handle();
				}break;
				
				
				case RC_MI:
				{
					//ң��ȡ������ģʽ					
					glb_ctrl_mode = DEBUG_MODE;
					chassis.ctrl_mode=CHASSIS_STAY;
					
//					glb_ctrl_mode = RC_BULLET_MODE; //����ȫ��ȡ��ģʽ
//					rc_bullet_handle();  //ȡ��ң�ص��Ժ���
//					get_bullet_front();
					get_bullet_single();
				}break;
				
				
				case RC_DN:
				{
//					//����ģʽ/��ʼ��ȫ��
					
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
			//����
			glb_ctrl_mode = SAFETY_MODE;
			safety_mode_handle();			
		}break;
		
		
		case RC_DN:
		{
		//����ģʽ
			glb_ctrl_mode = KB_MODE;
			
			keyboard_handle();
		}break;
		
		
		default:
		break;
  }
	#else
	
	keyboard_handle();	
		
	#endif
}


//ң�ص����ƶ�ģʽ�¶�ң�����Ĳ��������Ը��������л�״̬
void rc_move_handle(void)
{	
	slip.ctrl_mode = SLIP_ADJUST;   					//���Ƶ����΢��
	uplift.ctrl_mode = UPLIFT_ADJUST;					//̧��������΢����ch5
	
	uplift.ctrl_mode = UPLIFT_ADJUST;
	chassis.ctrl_mode = CHASSIS_REMOTE_NORMAL;//���������ٶ�����ģʽch1.ch2,ch3
	
//	//����
//	if(rc.ch4>500)
//		pump.bullet1_ctrl_mode = ON_MODE;
//	else
//		pump.bullet1_ctrl_mode = OFF_MODE;
	
	
//	//��Ԯ
//	if(rc.ch4<-500)
//		pump.help_ctrl_mode = ON_MODE;
//	else
//		pump.help_ctrl_mode = OFF_MODE;


//	//ͼ���ӽ�����
//	relay.view_tx.pitch = PIT_PWM_MID;
//	relay.view_tx.yaw = YAW_PWM_MID;	
}


//��ȫģʽ
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
//			camera_mode = CAMERA_CLIMB_MODE;
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

	
//	if(rc.sw2==RC_UP)func_mode = GET_BULLET1_MODE;
//	else if(rc.sw2==RC_MI)func_mode = GET_BULLET2_MODE;

	if(rc.kb.bit.Z)	//���ָ�λ		z
	{
//		bullet.ctrl_mode = BULLET_RESET;
	}
	if(rc.kb.bit.X&&rc_kb_X_flag==0)	//ȡ����һ��   x
	{
		rc_kb_X_flag = 1;
//		if(rc.sw2==RC_UP)func_mode = GET_BULLET1_MODE;
//		else if(rc.sw2==RC_MI)func_mode = GET_BULLET2_MODE;
//		if(rc.kb.bit.SHIFT)	func_mode = GET_BULLET2_MODE;
//		else func_mode = GET_BULLET1_MODE;
	}
	if(rc.kb.bit.X==0&&rc_kb_X_flag)
	{
		rc_kb_X_flag = 0;
	}
}

void get_global_last_mode(void)
{
	last_glb_ctrl_mode = glb_ctrl_mode;
	last_func_mode = func_mode;
}

/**
  * @brief  �Զ�ȡһ���ӵ�
  * @param  void
  * @retval void
  * @attention  �������������,bullet_step_singleΪSINGLE_READY��ʼȡ���䵯
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
			rotate.cnt_ref = 400;	
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
  * @brief  �Զ�ȡǰ�������ӵ�
  * @param  void
  * @retval void
  * @attention  �������������,��bullet_step_front��ΪAIM_BULLET�����ȡ����׼,��bullet_flag1��1��ʼȡ��
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
//			chassis.ctrl_mode = CHASSIS_RC_NEAR;//������ǽ��λ
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
//					bullet_flag1=0;
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
//						bullet_flag1=0;
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
//						bullet_flag1=0;
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
//						bullet_flag1=0;
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
//						bullet_flag1=0;
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
//					bullet_flag1=0;
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
//								bullet_flag1=0;
							handle_fetch_time=0;
							handle_fetch_time2=0;
							bullet_step_front = BULLET_RESET_STEP;												
						}			
				}
			}
			
		}break;
		
		
		case BULLET_RESET_STEP:
		{
			uplift.ctrl_mode = UPLIFT_STOP;
			slip.ctrl_mode = SLIP_STOP;
			rotate.ctrl_mode = ROTATE_STOP;
			
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


