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
#define E_rotate 1 //����1��ʾʹ��3508��ת���,����ʹ����ת����


/** 
  * @file 		keyboard_handle.c
  * @version 	0.9
  * @date 		Jan,18 2020
  * @brief  	����ģʽ�����ļ�
  *	@author 	Taurus 2020
  */


void keyboard_handle()
{
	//δ����
	//keyboard_chassis_ctrl(); //���̲��ּ��̲���
	
}



/**
  * @brief      ���̿��Ƶ��̺���
  * @param[in]  none
  * @retval     none
	* @note				����˵����
								1��ȡ��ʱ����갴ס�Ҽ�����������Դ��������WS����ǰ�������ƶ�
								2��ƽ���ƶ�ʱ����סCTRL�����ƶ���SHIFT�����ƶ�
  */

void keyboard_chassis_ctrl()
{
	
	if(rc.mouse.r && glb_ctrl_mode == RC_BULLET_MODE)  						{	chassis.ctrl_mode = CHASSIS_KB_NEAR; 		chassis_speed = SPEED_SLOW;		 	}
  else if(rc.kb.bit.CTRL) 		{	chassis_speed = SPEED_SLOW; 						chassis.ctrl_mode = CHASSIS_KB;	}
	else if (rc.kb.bit.SHIFT)		{	chassis_speed = SPEED_FAST; 						chassis.ctrl_mode = CHASSIS_KB;	}
	else												{	chassis_speed = SPEED_NORMAL; 					chassis.ctrl_mode = CHASSIS_KB;	}
	
}
/**
  * @brief      ����ģʽ��ȡ���ܿ��ƺ���
  * @param[in]  none
  * @retval     none
	* @note				����˵����
								1��ȡ��ʱ����갴ס�Ҽ�����������Դ�������ƻ������м�λ�ã�̧������
								2���ɿ����˳�ȡ��ģʽ����λ
  */
void fetch_bullet_ctrl_fun()
{



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
				#if(E_rotate)
				rotate.ecd_fdb = rotate.bullet_angle_ref;
				if(rotate.position_flag == ROTATE_BULLET_POS_FLAG) 
				{
					fetch_step = PRESSING;
					fun_sec_cnt = 0;
				}
				#else
				pump.rotate_ctrl_mode = ON_MODE;
				if(fun_sec_cnt > 20)
				{
					fetch_step = PRESSING;
					fun_sec_cnt = 0;
				}
				#endif

				
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
				#if(E_rotate)
				rotate.ecd_fdb = rotate.loose_angle_ref;
				if(rotate.position_flag == ROTATE_LOOSE_POS_FLAG)
				{
				  fetch_step = THROWING;
					fun_sec_cnt = 0;
				}
				#else
				pump.rotate_ctrl_mode = OFF_MODE;
				if(fun_sec_cnt > 20)
				{
					fetch_step = THROWING;
					fun_sec_cnt = 0;
				}
				#endif
				
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


