/** 
  * @file not just bsp_pump.c �пոĵ��ļ��� =v=
  * @version 1.0
  * @date Mar,2 2018
	*
  * @brief  ����Ԫ��;��ת��ȡ���;��������ͷ
	*
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
#include "relay_task.h"
#include "bsp_uart.h"

void throw_executed()
{
	if(pump.throw_ctrl_mode ==ON_MODE)	throw_on();
	else	throw_off();
}//�����ӵ�ҩ��

void bracket_executed()
{
	if(pump.bracket_ctrl_mode ==ON_MODE)	bracket_on();
	else	bracket_off();
}//�������֧��

void press_executed()
{
	if(pump.press_ctrl_mode ==ON_MODE)	press_on();
	else	press_off();
}//����ȡ������

void help_executed()
{
	if(pump.help_ctrl_mode ==ON_MODE)	help_on();
	else	help_off();
}//���ؾ�Ԯ����

void bullet1_executed()
{
	if(pump.bullet1_ctrl_mode ==ON_MODE)	bullet1_on();
	else	bullet1_off();
}//��ҩ����1

void bullet2_executed()
{
	if(pump.bullet2_ctrl_mode ==ON_MODE)	bullet2_on();
	else	bullet2_off();
}//��ҩ����2


/** 
  
  * @brief  �����ǵ���������Ϣ
  * @date Jan,12 2020
  *	@author 
  *
  */

void camera_executed()
{
	if(electrical.camera_ctrl_mode ==ON_MODE)	camera_on();
	else	camera_off();
}//�л���������ͷ�ӽ�

void magazine_executed()
{
	if(electrical.magazine_ctrl_mode ==ON_MODE)	magazine_on();
	else	magazine_off();
}//�л�����ģʽ

void relay_safe_executed()
{
	if(electrical.safe_mode ==ON_MODE)	relay_safe_on();
	else	relay_safe_off();
}//��ȫģʽ

void pump_init()
{
	//�����ʼ����ɺ��ͽ���������Ϣ��־λ
	
	pump.help_ctrl_mode = OFF_MODE;
	pump.press_ctrl_mode = OFF_MODE;
	pump.throw_ctrl_mode = OFF_MODE;
	pump.bracket_ctrl_mode = OFF_MODE;
	pump.bullet1_ctrl_mode = OFF_MODE;
	pump.bullet2_ctrl_mode = OFF_MODE;
//	pump.interact_ctrl_mode = OFF_MODE;
	
	electrical.camera_ctrl_mode = OFF_MODE;
	electrical.magazine_ctrl_mode = OFF_MODE;
	
	electrical.safe_mode = OFF_MODE;
	
	help_executed();
	press_executed();
	bracket_executed();
	throw_executed();
	bullet1_executed();
	bullet2_executed();

	camera_executed();
	magazine_executed();
	
	relay_safe_executed();
	
	relay.status[0]=0XAA;
	relay.status[1]=0XBB;
	relay.status[2]=relay.gas_status;
	relay.status[3]=relay.electrical_status;
	
	HAL_UART_Transmit(&huart6, (uint8_t *)&relay.status, 4, 0x1f);
	
}


