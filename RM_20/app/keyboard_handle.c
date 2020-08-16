/** 
  * @file 		keyboard_handle.c
  * @version 	0.9
  * @date 		Jan,18 2020
  * @brief  	键盘模式操作文件
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

uint8_t single_bullet_postion = 1;//单箱取弹位置标志位

//单击标志位
uint8_t one_click_G_flag = 1;
uint8_t one_click_mouseL_flag = 1;
uint8_t one_click_bullet_flag = 1;
uint8_t one_click_single_pos_flag = 1;
uint8_t one_click_help_flag = 1;
uint8_t one_click_give_bullet = 1;

//取弹计时标志位
uint32_t handle_time=0;
uint32_t handle_time2=0;
uint32_t handle_times=0;

static uint8_t step = 0;

//图传云台视角定义
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
//pwm值增大逆时针转 最左为500pwm，最右为2500

ramp_function_source_t chassis_auto_ramp;

	
/**
  * @brief      键盘控制函数
  * @param[in]  none
  * @retval     none
	* @note				=v=
	*							
  */	
void keyboard_handle()
{	
//	if(uplift.state!=UPLIFT_KNOWN||slip.state!=SLIP_KNOWN||rotate.state!=ROTATE_KNOWN)
//	{return;}
	
	//模式选择
	if(rc.kb.bit.R && func_mode!=MOVE_MODE)		{func_mode = MOVE_MODE;}
	else if(rc.kb.bit.R)
	{
		set_relay_all_off();

		relay.view_tx.yaw		= PIT_ORIGIN;
		relay.view_tx.pitch = YAW_ORIGIN;
		slip.dist_ref = 493.6f;
		rotate.cnt_ref = 200;		
	}
	bullet_mode_switch();//取弹模式选择
	
	keyboard_chassis_ctrl();//底盘速度控制
	keyboard_sight_ctrl();//图传云台控制

	switch (func_mode)
	{
		
		case(MOVE_MODE):
		{
			if(rc.kb.bit.V)//按v切换救援夹子状态
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
			
			if(rc.kb.bit.G)//按住g开始给弹
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
			
			if(rc.kb.bit.C)//取弹停止
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
			
			if(rc.kb.bit.C)//取弹停止
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
  * @brief      键盘控制底盘函数
  * @param[in]  none
  * @retval     none
	* @note				操作说明：
	*							1、取弹时，鼠标按住右键进行贴紧资源岛，按键WS调整前后慢速移动
	*							2、平常移动时，按住CTRL慢速移动、SHIFT快速移动
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
  * @brief      云台视角控制函数
  * @param[in]  none
  * @retval     none
	* @note				操作说明：
	*							1、云台只有预设视角,无法线性调整
	*							2、
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
  * @brief      比赛模式下取弹模式切换函数
  * @param[in]  none
  * @retval     none
	* @note				1、取弹时，鼠标按住右键底盘进入取弹瞄准模式,默认为取单箱模式,按Z/X/C切换为前面三箱/T形/前后六箱模式
	*							2、松开后，退出取弹模式，复位
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
  * @brief      取单箱模式下的取弹机构位置控制函数
  * @param[in]  none
  * @retval     none
	* @note				V/B控制取弹机构向左/右,G控制取弹机构伸出/收回
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
  * @brief      比赛模式下抬升和横移配合函数
  * @param[in]  none
  * @retval     none
	* @note				操作说明：
								1、取弹时，鼠标按住右键进行贴紧资源岛，横移机构在中间位置，抬升上升
								2、松开后，退出取弹模式，复位
  */
void keyboard_uplift_slip_ctrl()
{
	if(glb_ctrl_mode == RC_BULLET_MODE && rc.mouse.r)
	{
		if(slip.position_flag == CENTER_BULLET_POS) //要在中心位置才可以抬升到取弹高度
		{	
			uplift.height_ref[0] = uplift.height_get_bullet_REF ; //抬升到取弹高度
		}
		else
		{
			slip.dist_ref = slip.center_bullet_REF; 	//插手调整横移部分
			uplift.height_ref[0] = uplift.height_fdb[0]; //抬升保持不动
		}
	}
	else
	{
		reset_bullet(); 
	}

}
/**
  * @brief      取弹完成后复位
  * @param[in]  none
  * @retval     none
	* @note				说明：
								1、爪子松开，伸出架收回，爪子收回
								2、松开后，退出取弹模式，复位

case 在抬升准备取弹状态： 判断横移在中间位置，收回抬升


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
												 else uplift.height_ref[0] = uplift.height_fdb[0]; //保持原来状态
												 
												 if(fabs( uplift.height_fdb[0]) < 0.1) {fetch_step = RESET_FLAG; fun_reset=0;}
											 } break;

		case ROTATE_GOING:
											 {
												 electrical.rotate_ctrl_mode = OFF_MODE; 	
												 pump.bracket_ctrl_mode = OFF_MODE; 
												 fun_reset++;
												 if(fun_reset>50) {fetch_step = SLIP_LEFT; fun_reset=0;}//回到任一位置	
											 } break;

		case PRESSING:
											 {
												 pump.press_ctrl_mode = OFF_MODE;
												 fun_reset++;
												 if(fun_reset>20) {fetch_step = ROTATE_GOING; fun_reset=0;}//因不确定原来取的是第一排还是第二排
											 } break;

		case ROTATE_BACK:
											 { //爪子收回步骤，就直接进行到底，完成后面的取弹步骤
												 electrical.rotate_ctrl_mode = OFF_MODE;
												 fun_reset++;
												 if(fun_reset>20) {fetch_step = THROWING; fun_reset=0;}//下一步
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
												 if(fun_reset>50) {fetch_step = SLIP_LEFT; fun_reset=0;}//回到任一位置	
											 } break;
		case BRACKET_BACK:
											 {
												 pump.press_ctrl_mode = OFF_MODE;
												 fun_reset++;
												 if(fun_reset>20) {fetch_step = ROTATE_GOING; fun_reset=0;}//因不确定原来取的是第一排还是第二排
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
				if(handle_times>=20) handle_times=20; //避免数值过大
				pump.press_ctrl_mode = ON_MODE;
				if(handle_time2>10)
				{								
					if(fabs(rotate.cnt_fdb-990)<40)
					{
						
						pump.press_ctrl_mode = OFF_MODE;
						handle_times++;
						if(handle_times>=71) handle_times=71; //避免数值过大
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
				if(handle_times>=106) handle_times=106; //避免数值过大
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
