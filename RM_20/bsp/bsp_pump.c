/** 
  * @file bsp_pump.c
  * @version 1.0
  * @date Mar,2 2018
	*
  * @brief  气动元件。救援机构。抱柱爪子，悬架，夹取
	*
  *	@author li zh
  *
  */
#define  __BSP_PUMP_GLOBALS                                                                                             
#include "string.h"
#include "stdlib.h"
#include "bsp_uart.h"
#include "usart.h"
#include "main.h"
#include "bsp_pump.h"
#include "chassis_task.h"
#include "gimbal_task.h"
#include "relay_task.h"
#include "bsp_uart.h"

void throw_executed()
{
	if(pump.throw_ctrl_mode ==ON_MODE)	throw_on();
	else	throw_off();
}

void bracket_executed()
{
	if(pump.bracket_ctrl_mode ==ON_MODE)	bracket_on();
	else	bracket_off();
}

void press_executed()
{
	if(pump.press_ctrl_mode ==ON_MODE)	press_on();
	else	press_off();
}

void help_executed()
{
	if(pump.help_ctrl_mode ==ON_MODE)	help_on();
	else	help_off();
}

void rotate_executed()
{
	if(pump.rotate_ctrl_mode ==ON_MODE)	rotate_on();
	else	rotate_off();
}

void magazine_executed()
{
	if(pump.magazine_ctrl_mode ==ON_MODE)	magazine_on();
	else	magazine_off();
}

void interact_executed()
{
	if(pump.interact_ctrl_mode ==ON_MODE)	interact_on();
	else	interact_off();
}

/** 
  
  * @brief  以下是电气串口信息
  * @date Jan,12 2020
  *	@author 
  *
  */

void camera_executed()
{
	if(electrical.camera_ctrl_mode ==ON_MODE)	camera_on();
	else	camera_off();
}

void laser_executed()
{
	if(electrical.laser_ctrl_mode ==ON_MODE)	laser_on();
	else	laser_off();
}



void pump_init()
{
	//程序初始化完成后发送接受气阀信息标志位
	
	pump.help_ctrl_mode = OFF_MODE;
	pump.bracket_ctrl_mode = OFF_MODE;
	pump.magazine_ctrl_mode = OFF_MODE;
	pump.throw_ctrl_mode = OFF_MODE;
	pump.rotate_ctrl_mode = OFF_MODE;
	pump.press_ctrl_mode = OFF_MODE;
	pump.interact_ctrl_mode = OFF_MODE;
	
	electrical.camera_ctrl_mode = OFF_MODE;
	electrical.camera_ctrl_mode = OFF_MODE;
	
	help_executed();
	bracket_executed();
	magazine_executed();
	throw_executed();
	press_executed();
	rotate_executed();
	interact_executed();
	
	camera_executed();
	laser_executed();
	
	relay.status[0]=0XAA;
	relay.status[1]=0XBB;
	relay.status[2]=relay.gas_status;
	relay.status[3]=relay.electrical_status;
	
	HAL_UART_Transmit(&huart6, (uint8_t *)&relay.status, 4, 0x1f);
	
}


