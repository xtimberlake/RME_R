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
#include "keyboard_handle.h"
#include "math.h"

#define Cotroller_Owner 1


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
uint8_t ready_flag=0;


#define MOUSE_SPEED_SLOW		50
#define MOUSE_SPEED_NORMAL	100
#define MOUSE_SPEED_FAST		150

#define slope_scale 100
uint32_t handle_cnt=0,ready_step_flag=0;
uint32_t climb_handle_cnt=0,climb_ready_step_flag=0;
uint32_t handle_fetch_time=0,handle_fetch_time2=0;

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
	#if(Cotroller_Owner)
	switch (rc.sw1)
  {
		case RC_UP:
		{
			switch (rc.sw2){
				case RC_UP:
				{			
					//ң�ص�������ģʽ
					//glb_ctrl_mode = RC_MOVE_MODE;
					rc_move_handle();
				}break;
				case RC_MI:
				{
					//ң��ȡ���o������ģʽ
						//����
					
					chassis.ctrl_mode=CHASSIS_REMOTE_SLOW;
					if(rc.ch4>500)
					pump.bullet1_ctrl_mode = ON_MODE;
					else
					pump.bullet1_ctrl_mode = OFF_MODE;
	
					//glb_ctrl_mode = RC_BULLET_MODE; //����ȫ��ȡ��ģʽ
					rc_bullet_handle();  //ȡ��ң�ص��Ժ���
					//get_bullet_ctrl_mode(); //ȡ���ص�����
					keyborad_bullet_handle_controller();
				}break;
				case RC_DN:
				{
					slip.state = SLIP_KNOWN;
					slip.ctrl_mode = SLIP_AUTO;
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
	#else
	
	keyboard_handle();	
		
	#endif
}
	
void safety_mode_handle(void)
{
	chassis.ctrl_mode = CHASSIS_STOP;
	uplift.ctrl_mode = UPLIFT_STOP;
	slip.ctrl_mode = SLIP_STOP;
	rotate.ctrl_mode = ROTATE_STOP;
	
	electrical.safe_mode = OFF_MODE;
}
//ң�ص����ƶ�ģʽ�¶�ң�����Ĳ��������Ը��������л�״̬
void rc_move_handle(void)
{	
//	slip.ctrl_mode = SLIP_ADJUST;   					//���Ƶ����΢��
//	uplift.ctrl_mode = UPLIFT_ADJUST;					//̧��������΢����ch5
	
	uplift.ctrl_mode = UPLIFT_STOP;
	chassis.ctrl_mode = CHASSIS_REMOTE_NORMAL;//���������ٶ�����ģʽch1.ch2,ch3
	
	//����
	if(rc.ch4>500)
		pump.bullet1_ctrl_mode = ON_MODE;
	else
		pump.bullet1_ctrl_mode = OFF_MODE;
	
	
	
	
	
	
	
	
//	//ch4�����¿��ۣ���Ԯ�����պϣ�����ģʽ����
//	if(rc.ch4<-500)
//		pump.help_ctrl_mode = ON_MODE;
//	else
//		pump.help_ctrl_mode = OFF_MODE;
	//ͼ���ӽ�����
	
//	relay.view_tx.pitch = PIT_PWM_MID;
//	relay.view_tx.yaw = YAW_PWM_MID;	

	//�رպ���
//	climb.jdq = 0;
//	climb.pwm = 800;
//	climb.jdq?HAL_GPIO_WritePin(TURBINE_JDQ_GPIO_Port,TURBINE_JDQ_Pin,GPIO_PIN_SET):
//	HAL_GPIO_WritePin(TURBINE_JDQ_GPIO_Port,TURBINE_JDQ_Pin,GPIO_PIN_RESET);
//	TIM3->CCR3 = climb.pwm;

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
			ready_flag = 0;
			bullet_setp = BULLET_SAFETY_MODE;
			rc5_flag = 0;		
		
	}
	else if(rc5_flag==2&&rc.ch5>-250&&rc.ch5<250)
	{

			ready_flag = 1;	//ȡ����־λ��һ
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



void climb_up_ctrl(void)
{
	
}

void climb_down_ctrl(void)
{

}
void climb_ctrl_mode(void)
{

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
			slip.dist_ref = 0;
			rotate.cnt_ref = 600;
			if(fabs((double)(slip.dist_fdb))<1.6)
			{
				ready_flag=0;
			  bullet_setp = ROT_OUT;
				pump.press_ctrl_mode = ON_MODE;
				
				
			/*�Զ�������һ��*/
			//ready_flag=1;
			/*�Զ�������һ��*/
				
				
			}
			else if(fabs((double)(slip.dist_fdb))<25)
			{
				pump.press_ctrl_mode = ON_MODE;
			}
		}

	}break;
	
	case ROT_OUT:
	{
		if(ready_flag==1)
		{
			
			
			rotate.cnt_ref = 990;
			
			if(fabs((double)(rotate.cnt_fdb-990))<20)
			{
				ready_flag=0;
				bullet_setp = ROT_OFF;
				handle_fetch_time=0;
			}
		}

	}break;
	case ROT_OFF:
	{
				if(ready_flag==1)
		{
			handle_fetch_time++;
			pump.press_ctrl_mode =OFF_MODE;
			
			if(handle_fetch_time>70)
			{
			rotate.cnt_ref = 170;
			if(fabs((double)(rotate.cnt_fdb-170))<15)
			{
				ready_flag=0;
				bullet_setp = TAKE_OFF_AND_OUT;
				handle_fetch_time=0;
			}
			else if(fabs((double)(rotate.cnt_fdb-170))<70)
			{
				slip.dist_ref = 49.35f;
			}	
		}
			
		}

	}break;
	
	case TAKE_OFF_AND_OUT:
	{
		
		slip.dist_ref = 49.35f;
			if(ready_flag==1 )
		{
			handle_fetch_time++;
			pump.press_ctrl_mode = ON_MODE;
			if(handle_fetch_time>55)
			{
			pump.throw_ctrl_mode = ON_MODE;
			rotate.cnt_ref = 990;
			
				
			if(fabs((float)(rotate.cnt_fdb-990))<20)
			{
				
					pump.throw_ctrl_mode = OFF_MODE;
					handle_fetch_time2=0;
					handle_fetch_time=0;
					ready_flag=0;
					bullet_setp = ROT_OFF_MID;

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
			
			if(handle_fetch_time>70)
			{
			rotate.cnt_ref = 170;
			if(fabs((double)(rotate.cnt_fdb-170))<15)
			{
				ready_flag=0;
				bullet_setp = TAKE_OFF_AND_OUT_MID;
				handle_fetch_time=0;
				handle_fetch_time2=0;
			}
			else if(fabs((double)(rotate.cnt_fdb-170))<70)
			{
				slip.dist_ref = 98.72f;
			}	
		}
			
		}
	
	}break;
	
	case TAKE_OFF_AND_OUT_MID:
	{
		slip.dist_ref = 98.72f;
					if(ready_flag==1 )
		{
			handle_fetch_time++;
			pump.press_ctrl_mode = ON_MODE;
			if(handle_fetch_time>55)
			{
				pump.throw_ctrl_mode = ON_MODE;
			rotate.cnt_ref = 990;
			
				
			if(fabs((double)(rotate.cnt_fdb-990))<20)
			{
				
					pump.throw_ctrl_mode = OFF_MODE;
					handle_fetch_time2=0;
					handle_fetch_time=0;
					ready_flag=0;
					bullet_setp = ROT_OFF_FINAL;	
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
			
			if(handle_fetch_time>70)
			{
			rotate.cnt_ref = 170;
			if(fabs((double)(rotate.cnt_fdb-170))<15)
			{
				ready_flag=0;
				bullet_setp = TAKE_OFF_FINAL;
				handle_fetch_time=0;
				handle_fetch_time2=0;
			}
			else if(fabs((double)(rotate.cnt_fdb-170))<75)
			{
				slip.dist_ref = 49.35f;
			}	
		}
			
		}
	
	}break;
	
	case TAKE_OFF_FINAL:
	{
	
			slip.dist_ref = 49.35f;
					if(ready_flag==1 )
		{
			handle_fetch_time++;
			pump.press_ctrl_mode = ON_MODE;
			if(handle_fetch_time>55)
			{
				pump.throw_ctrl_mode = ON_MODE;
			rotate.cnt_ref = 500;
			
			
			if(fabs((double)(rotate.cnt_fdb-500))<50)
			{
					handle_fetch_time2++;
				  pump.throw_ctrl_mode = OFF_MODE;
				
					if(handle_fetch_time2>70)
					{
					rotate.cnt_ref = 200;
					ready_flag=0;
					handle_fetch_time=0;
					handle_fetch_time2=0;
					bullet_setp = BULLET_HOLD_ON;
						
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
