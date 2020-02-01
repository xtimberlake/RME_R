#include "keyboard_handle.h"
#include "remote_msg.h"
#include "chassis_task.h"
#include "uplift_task.h"
#include "slip_task.h"

/** 
  * @file 		keyboard_handle.c
  * @version 	0.9
  * @date 		Jan,18 2020
  * @brief  	����ģʽ�����ļ�
  *	@author 	Taurus 2020
  */


void keyboard_handle()
{
	
	keyboard_chassis_ctrl(); //���̲��ּ��̲���
	
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
	
	if(rc.mouse.r)  						{	chassis.ctrl_mode = CHASSIS_KB_NEAR; 		chassis_speed = SPEED_SLOW;		 	}
  else if(rc.kb.bit.CTRL) 		{	chassis_speed = SPEED_SLOW; 						chassis.ctrl_mode = CHASSIS_KB;	}
	else if (rc.kb.bit.SHIFT)		{	chassis_speed = SPEED_FAST; 						chassis.ctrl_mode = CHASSIS_KB;	}
	else												{	chassis_speed = SPEED_NORMAL; 					chassis.ctrl_mode = CHASSIS_KB;	}
	
}

void keyboard_uplift_ctrl()
{
	if(slip.position_flag == CENTER_BULLET_POS) //Ҫ������λ�òſ���̧����ȡ���߶�
	{
		slip.dist_ref = slip.center_bullet_REF; 	//���ֵ������Ʋ���
		uplift.height_ref[0] = uplift.height_get_bullet_REF ;
	}
	else
	{
		uplift.height_ref[0] = 0;
	}
	

}


