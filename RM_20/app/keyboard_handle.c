#include "keyboard_handle.h"
#include "remote_msg.h"
#include "chassis_task.h"
#include "uplift_task.h"
#include "slip_task.h"

/** 
  * @file 		keyboard_handle.c
  * @version 	0.9
  * @date 		Jan,18 2020
  * @brief  	键盘模式操作文件
  *	@author 	Taurus 2020
  */


void keyboard_handle()
{
	
	keyboard_chassis_ctrl(); //底盘部分键盘操作
	
}



/**
  * @brief      键盘控制底盘函数
  * @param[in]  none
  * @retval     none
	* @note				操作说明：
								1、取弹时，鼠标按住右键进行贴紧资源岛，按键WS调整前后慢速移动
								2、平常移动时，按住CTRL慢速移动、SHIFT快速移动
  */

void keyboard_chassis_ctrl()
{
	
	if(rc.mouse.r)  						{	chassis.ctrl_mode = CHASSIS_KB_NEAR; 		chassis_speed = SPEED_SLOW;		 	}
  else if(rc.kb.bit.CTRL) 		{	chassis_speed = SPEED_SLOW; 						chassis.ctrl_mode = CHASSIS_KB;	}
	else if (rc.kb.bit.SHIFT)		{	chassis_speed = SPEED_FAST; 						chassis.ctrl_mode = CHASSIS_KB;	}
	else												{	chassis_speed = SPEED_NORMAL; 					chassis.ctrl_mode = CHASSIS_KB;	}
	
}

void keyboard_uplift_ctrl()
{
	if(slip.position_flag == CENTER_BULLET_POS) //要在中心位置才可以抬升到取弹高度
	{
		slip.dist_ref = slip.center_bullet_REF; 	//插手调整横移部分
		uplift.height_ref[0] = uplift.height_get_bullet_REF ;
	}
	else
	{
		uplift.height_ref[0] = 0;
	}
	

}


