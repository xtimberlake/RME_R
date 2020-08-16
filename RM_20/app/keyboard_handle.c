/** 
  * @file 		keyboard_handle.c
  * @version 	0.9
  * @date 		Jan,18 2020
  * @brief  	����ģʽ�����ļ�
  *	@author 	Taurus 2020
  */
	
#define __KEYBOARD_HANDLE_GLOBALS
#include "keyboard_handle.h"
#include "modeswitch_task.h"
#include "remote_msg.h"
#include "chassis_task.h"
#include "uplift_task.h"
#include "slip_task.h"
#include "relay_task.h"
#include "cmsis_os.h"
#include "math.h"
#include "rotate_task.h"
#include "pid.h"
#include "bsp_motor.h"
#include "remote_msg.h"
#include "math_calcu.h"

uint8_t single_bullet_postion = 1;//����ȡ��λ�ñ�־λ

//������־λ
uint8_t one_click_G_flag = 1;
uint8_t one_click_mouseL_flag = 1;
uint8_t one_click_bullet_flag = 1;
uint8_t one_click_single_pos_flag = 1;
uint8_t one_click_help_flag = 1;
uint8_t one_click_give_bullet = 1;

//ȡ����ʱ��־λ
uint32_t handle_time=0;
uint32_t handle_time2=0;
uint32_t handle_times=0;

static uint8_t step = 0;

//ͼ����̨�ӽǶ���
#define PIT_ORIGIN	1480;
#define YAW_ORIGIN	1150;
#define PIT_LEFT		1000;
#define YAW_LEFT		840;
#define PIT_ORIGIN_UP		1350;
#define PIT_ORIGIN_DOWN	900;
//#define PIT_PWM_DOWN_LIMTI	800
//#define PIT_PWM_UP_LIMIT 		1650
//#define YAW_PWM_LEFT_LIMIT	2500
//#define YAW_PWM_RIGHT_LIMIT 500
//pwmֵ������ʱ��ת ����Ϊ500pwm������Ϊ2500

ramp_function_source_t chassis_auto_ramp;

	
/**
  * @brief      ���̿��ƺ���
  * @param[in]  none
  * @retval     none
	* @note				=v=
	*							
  */	
void keyboard_handle()
{	
//	if(uplift.state!=UPLIFT_KNOWN||slip.state!=SLIP_KNOWN||rotate.state!=ROTATE_KNOWN)
//	{return;}
	
	//ģʽѡ��
	if(rc.kb.bit.R && func_mode!=MOVE_MODE)		{func_mode = MOVE_MODE;}
	else if(rc.kb.bit.R)
	{
		set_relay_all_off();

		relay.view_tx.yaw		= PIT_ORIGIN;
		relay.view_tx.pitch = YAW_ORIGIN;
		slip.dist_ref = 493.6f;
		rotate.cnt_ref = 200;		
	}
	bullet_mode_switch();//ȡ��ģʽѡ��
	
	keyboard_chassis_ctrl();//�����ٶȿ���
	keyboard_sight_ctrl();//ͼ����̨����

	switch (func_mode)
	{
		
		case(MOVE_MODE):
		{
			if(rc.kb.bit.V)//��v�л���Ԯ����״̬
			{
				if(one_click_help_flag)
				{
					if(pump.help_ctrl_mode == ON_MODE)
					{
						pump.help_ctrl_mode = OFF_MODE;
						electrical.camera_ctrl_mode = OFF_MODE;
					}else
					{
						pump.help_ctrl_mode = ON_MODE; 
						electrical.camera_ctrl_mode = OFF_MODE;
					}
					one_click_help_flag = 0;
				}	
			}else {one_click_help_flag = 1;}
			
			if(rc.kb.bit.G)//��סg��ʼ����
			{
				pump.bullet1_ctrl_mode = ON_MODE;
				electrical.magazine_ctrl_mode = ON_MODE;
			}else 
			{
				pump.bullet1_ctrl_mode = OFF_MODE;
				electrical.magazine_ctrl_mode = OFF_MODE;
			}
		}break;
		
		case(GET_BULLET_SINGLE_MODE):
 		{
			bullet_single_handle();
			if(rc.mouse.l)
			{bullet_step_single = SINGLE_READY;}
			
			if(rc.kb.bit.C)//ȡ��ֹͣ
			{bullet_step_single = BULLET_RESET_STEP;}
			
			get_bullet_single();
		}break;
		
		case(GET_BULLET_FRONT_MODE):
		{
			
			if(rc.mouse.l)
			{			
				if(one_click_mouseL_flag)
				{
					if(bullet_step_front!=AIM_BULLET)
					{
						bullet_step_front = AIM_BULLET;
					} else
					{
						bullet_step_front = ROT_OUT_LEFT;
					}
					one_click_mouseL_flag = 0;
				}	
			}else {one_click_mouseL_flag = 1;}
			
			if(rc.kb.bit.C)//ȡ��ֹͣ
			{bullet_step_front = BULLET_RESET_STEP;}
			
			get_bullet_front();
		}break;
		
		
		
		
		
		
		default:
		break;
	}
	
	
}

void safety_first()
{
	chassis.ctrl_mode = CHASSIS_STOP;
	uplift.ctrl_mode = UPLIFT_STOP;
  slip.ctrl_mode = SLIP_STOP;
	rotate.ctrl_mode = ROTATE_STOP;
	pump.press_ctrl_mode=OFF_MODE;
	pump.throw_ctrl_mode=OFF_MODE;
}

/**
  * @brief      ���̿��Ƶ��̺���
  * @param[in]  none
  * @retval     none
	* @note				����˵����
	*							1��ȡ��ʱ����갴ס�Ҽ�����������Դ��������WS����ǰ�������ƶ�
	*							2��ƽ���ƶ�ʱ����סCTRL�����ƶ���SHIFT�����ƶ�
  */
void keyboard_chassis_ctrl()
{
	if(func_mode == MOVE_MODE)
	{
		if(rc.kb.bit.CTRL) 					{	chassis_speed = SPEED_SLOW; 						chassis.ctrl_mode = CHASSIS_KB;	}
		else if (rc.kb.bit.SHIFT)		{	chassis_speed = SPEED_FAST; 						chassis.ctrl_mode = CHASSIS_KB;	}
		else												{	chassis_speed = SPEED_NORMAL; 					chassis.ctrl_mode = CHASSIS_KB;	}
	}
}

/**
  * @brief      ��̨�ӽǿ��ƺ���
  * @param[in]  none
  * @retval     none
	* @note				����˵����
	*							1����ֻ̨��Ԥ���ӽ�,�޷����Ե���
	*							2��
  */
void keyboard_sight_ctrl()
{
	if(last_func_mode!=func_mode)
	{
		if(func_mode==GET_BULLET_SINGLE_MODE||func_mode==GET_BULLET_FRONT_MODE||func_mode==GET_BULLET_T_MODE||func_mode==GET_BULLET_last)
		{
			relay.view_tx.yaw		= PIT_LEFT;
			relay.view_tx.pitch = YAW_LEFT;			
		}else if(func_mode==MOVE_MODE)
		{
			relay.view_tx.yaw		= PIT_ORIGIN;
			relay.view_tx.pitch = YAW_ORIGIN;
		}
	}
}
/**
  * @brief      ����ģʽ��ȡ��ģʽ�л�����
  * @param[in]  none
  * @retval     none
	* @note				1��ȡ��ʱ����갴ס�Ҽ����̽���ȡ����׼ģʽ,Ĭ��Ϊȡ����ģʽ,��Z/X/C�л�Ϊǰ������/T��/ǰ������ģʽ
	*							2���ɿ����˳�ȡ��ģʽ����λ
  */
void bullet_mode_switch()
{
	if(rc.mouse.r)
	{
		if(one_click_bullet_flag)
		{
			func_mode = GET_BULLET_SINGLE_MODE;
			one_click_bullet_flag = 0;
		}
		if(rc.kb.bit.Z)					{	func_mode = GET_BULLET_FRONT_MODE;	}
		else if(rc.kb.bit.X) 		{	func_mode = GET_BULLET_T_MODE;			}
		else if (rc.kb.bit.C)		{	func_mode = GET_BULLET_last;				}											
	} else	{ one_click_bullet_flag = 1; }
}

/**
  * @brief      ȡ����ģʽ�µ�ȡ������λ�ÿ��ƺ���
  * @param[in]  none
  * @retval     none
	* @note				V/B����ȡ����������/��,G����ȡ���������/�ջ�
	*							
  */
void bullet_single_handle(void)
{
	if(func_mode == GET_BULLET_SINGLE_MODE)
	{
		if(rc.kb.bit.G)
		{
			if(one_click_G_flag)
			{
				if(pump.bracket_ctrl_mode == OFF_MODE)
				{
					pump.bracket_ctrl_mode = ON_MODE;
				}else
				{ pump.bracket_ctrl_mode = OFF_MODE; }
				one_click_G_flag = 0;
			}	
		}else {one_click_G_flag = 1;}
		
		if(rc.kb.bit.V && single_bullet_postion>0)
		{
			if(one_click_single_pos_flag)
			{
				single_bullet_postion-=1;
				one_click_single_pos_flag = 0;
			}
		}else if(rc.kb.bit.B && single_bullet_postion<2) 
		{
			if(one_click_single_pos_flag)
			{
				single_bullet_postion+=1;
				one_click_single_pos_flag = 0;
			}
		}else {one_click_single_pos_flag = 1;}
		
		switch (single_bullet_postion)
		{
			case 0: {slip.dist_ref = 5.2f;	}break;
			case 1: {slip.dist_ref = 493.6f;}break;
			case 2: {slip.dist_ref = 992.6f;}break;
		}
		
	}
}




/**
  * @brief      ����ģʽ��̧���ͺ�����Ϻ���
  * @param[in]  none
  * @retval     none
	* @note				����˵����
								1��ȡ��ʱ����갴ס�Ҽ�����������Դ�������ƻ������м�λ�ã�̧������
								2���ɿ����˳�ȡ��ģʽ����λ
  */
void keyboard_uplift_slip_ctrl()
{
	if(glb_ctrl_mode == RC_BULLET_MODE && rc.mouse.r)
	{
		if(slip.position_flag == CENTER_BULLET_POS) //Ҫ������λ�òſ���̧����ȡ���߶�
		{	
			uplift.height_ref[0] = uplift.height_get_bullet_REF ; //̧����ȡ���߶�
		}
		else
		{
			slip.dist_ref = slip.center_bullet_REF; 	//���ֵ������Ʋ���
			uplift.height_ref[0] = uplift.height_fdb[0]; //̧�����ֲ���
		}
	}
	else
	{
		reset_bullet(); 
	}

}
/**
  * @brief      ȡ����ɺ�λ
  * @param[in]  none
  * @retval     none
	* @note				˵����
								1��צ���ɿ���������ջأ�צ���ջ�
								2���ɿ����˳�ȡ��ģʽ����λ

case ��̧��׼��ȡ��״̬�� �жϺ������м�λ�ã��ջ�̧��


  */
void reset_bullet()
{

	static int fun_reset = 0;
		switch (fetch_step)
	{
		
		case UPLIFT_READY:
		case SLIP_LEFT:
		case SLIP_MIDD:
		case SLIP_RIGHT:	
											 {
												 pump.press_ctrl_mode = OFF_MODE;
												 pump.bracket_ctrl_mode = OFF_MODE; 
												 electrical.rotate_ctrl_mode = OFF_MODE; 										 
												 slip.dist_ref = slip.center_bullet_REF;
												 if(slip.position_flag == CENTER_BULLET_POS) uplift.height_ref[0] = 0;
												 else uplift.height_ref[0] = uplift.height_fdb[0]; //����ԭ��״̬
												 
												 if(fabs( uplift.height_fdb[0]) < 0.1) {fetch_step = RESET_FLAG; fun_reset=0;}
											 } break;

		case ROTATE_GOING:
											 {
												 electrical.rotate_ctrl_mode = OFF_MODE; 	
												 pump.bracket_ctrl_mode = OFF_MODE; 
												 fun_reset++;
												 if(fun_reset>50) {fetch_step = SLIP_LEFT; fun_reset=0;}//�ص���һλ��	
											 } break;

		case PRESSING:
											 {
												 pump.press_ctrl_mode = OFF_MODE;
												 fun_reset++;
												 if(fun_reset>20) {fetch_step = ROTATE_GOING; fun_reset=0;}//��ȷ��ԭ��ȡ���ǵ�һ�Ż��ǵڶ���
											 } break;

		case ROTATE_BACK:
											 { //צ���ջز��裬��ֱ�ӽ��е��ף���ɺ����ȡ������
												 electrical.rotate_ctrl_mode = OFF_MODE;
												 fun_reset++;
												 if(fun_reset>20) {fetch_step = THROWING; fun_reset=0;}//��һ��
											 } break;
		case THROWING:
											 {
												pump.throw_ctrl_mode = ON_MODE;
												fun_reset++;
												if(fun_reset > 100 && fun_reset<200){pump.throw_ctrl_mode = OFF_MODE;}
												else if(fun_reset > 250) {fetch_step = SLIP_LEFT; fun_reset=0;}
											 } break;
		case BRACKET_GOING:
											 {
												 electrical.rotate_ctrl_mode = OFF_MODE; 	
												 pump.bracket_ctrl_mode = OFF_MODE; 
												 fun_reset++;
												 if(fun_reset>50) {fetch_step = SLIP_LEFT; fun_reset=0;}//�ص���һλ��	
											 } break;
		case BRACKET_BACK:
											 {
												 pump.press_ctrl_mode = OFF_MODE;
												 fun_reset++;
												 if(fun_reset>20) {fetch_step = ROTATE_GOING; fun_reset=0;}//��ȷ��ԭ��ȡ���ǵ�һ�Ż��ǵڶ���
											 } break;
		case RESET_FLAG:
											 {
												fun_reset = 0;
												uplift.height_ref[0] = 0;
												slip.dist_ref = slip.center_bullet_REF; 
											 } break;
		case STAND_BY: break;
	
	}
}

void single_fetch_bullet()
{
	static int fun_cnt;
	fun_cnt++;
	switch (fetch_step)
	{
		
		case UPLIFT_READY:
		case SLIP_LEFT:
		case SLIP_MIDD:
		case SLIP_RIGHT:
			{
				fun_cnt = 0;
				fetch_step = ROTATE_GOING;
			}break;
		case ROTATE_GOING:
			{
				electrical.rotate_ctrl_mode = ON_MODE;
				if(fun_cnt > 20)
				{
					fetch_step = PRESSING;
					fun_cnt = 0;
				}
			}break;
		case PRESSING:
			{
				pump.press_ctrl_mode = ON_MODE;
				if(fun_cnt > 20)
				{
					fetch_step = ROTATE_BACK;
					fun_cnt = 0;
				}
			}break;
		case ROTATE_BACK:
			{

				electrical.rotate_ctrl_mode = OFF_MODE;
				if(fun_cnt > 20)
				{
					fetch_step = THROWING;
					fun_cnt = 0;
				}
			
			}break;
		case THROWING:
			{
				pump.throw_ctrl_mode = ON_MODE;
				fun_cnt++;
				if(fun_cnt > 100 && fun_cnt<200){pump.throw_ctrl_mode = OFF_MODE;}
			  else if(fun_cnt > 250) {fetch_step = UPLIFT_READY; fun_cnt=0;}
			}break;
		case BRACKET_GOING:
			{}break;
		case BRACKET_BACK:
			{}break;
		case RESET_FLAG:
			{}break;
		case STAND_BY: break;
		 
	
	}

}
void single_second_fetch_bullet(void)
{
	static int fun_sec_cnt;
	fun_sec_cnt++;
	switch (fetch_step)
	{
		
		case UPLIFT_READY: 
		case SLIP_LEFT:
		case SLIP_MIDD:
		case SLIP_RIGHT:
			{
				fun_sec_cnt = 0;
				fetch_step = ROTATE_GOING;
			}break;
			
		case BRACKET_GOING:
			{
				pump.bracket_ctrl_mode = ON_MODE;
				if(fun_sec_cnt > 20)
				{
					fetch_step = ROTATE_GOING;
					fun_sec_cnt = 0;
				}
			
			}break;	
		case ROTATE_GOING:
			{
				rotate.ecd_fdb = rotate.bullet_angle_ref;
				if(rotate.position_flag == ROTATE_BULLET_POS_FLAG) 
				{
					fetch_step = PRESSING;
					fun_sec_cnt = 0;
				}				
			}break;
		case PRESSING:
			{
				pump.press_ctrl_mode = ON_MODE;
				if(fun_sec_cnt > 20)
				{
					fetch_step = ROTATE_BACK;
					fun_sec_cnt = 0;
				}
			}break;
		case BRACKET_BACK:
			{
				pump.bracket_ctrl_mode = OFF_MODE;
			  if(fun_sec_cnt > 20)
				{
					fetch_step = ROTATE_BACK;
					fun_sec_cnt = 0;
				}
			}break;	
		case ROTATE_BACK:
			{
				rotate.ecd_fdb = rotate.loose_angle_ref;
				if(rotate.position_flag == ROTATE_LOOSE_POS_FLAG)
				{
				  fetch_step = THROWING;
					fun_sec_cnt = 0;
				}
			}break;
		case THROWING:
			{
				pump.throw_ctrl_mode = ON_MODE;
				fun_sec_cnt++;
				if(fun_sec_cnt > 100 && fun_sec_cnt<200){pump.throw_ctrl_mode = OFF_MODE;}
			  else if(fun_sec_cnt > 250)
				{
					fetch_step = STAND_BY; 
					fun_sec_cnt=0;
				}
	
			}break;
	
		case STAND_BY:{}break;
		case RESET_FLAG:
			{}break;

	
	}

}

void one_shot()
{
	
	switch(step)
	{
		case 0:
		{
			if(rc.kb.bit.F || fabs(rotate.cnt_ref-990)<2)
			{	
				handle_time2++;
				if(handle_times>=20) handle_times=20; //������ֵ����
				pump.press_ctrl_mode = ON_MODE;
				if(handle_time2>10)
				{								
					if(fabs(rotate.cnt_fdb-990)<40)
					{
						
						pump.press_ctrl_mode = OFF_MODE;
						handle_times++;
						if(handle_times>=71) handle_times=71; //������ֵ����
						if(handle_times>70 && rc.kb.bit.F)
						{
							handle_times = 0;
							handle_time2 = 0;
							rotate.cnt_ref = 180;
							step=1;
						}
						
					}
					else
					{
						rotate.cnt_ref = 990;					
					}			
				}
			}
		}break;
		
		
		case 1:
		{
			rotate.cnt_ref = 180;
			
			if(fabs((double)(rotate.cnt_fdb-180))<30 || handle_times>1)
			{
				pump.press_ctrl_mode = ON_MODE;
				handle_times++;
				if(handle_times>=106) handle_times=106; //������ֵ����
				if(fabs((double)(rotate.cnt_fdb-500))<30)
				{
					rotate.cnt_ref = 320;
					pump.press_ctrl_mode = OFF_MODE;
					pump.throw_ctrl_mode = OFF_MODE;
					handle_times = 0;
					handle_time2 = 0;
					step =0;
				}
				else if(handle_times>55)
				{
					pump.throw_ctrl_mode = ON_MODE;
					rotate.cnt_ref = 500;
				}
			}
		
		}break;
  }	

}
