/**
  * @file not just bsp_pump.h �пոĵ��ļ��� =v=
  * @version 1.0
  * @date Mar,2 2019
  *
  * @brief  
  *
  * @author li zh
  *
  */

#ifndef __BSP_PUMP_H__
#define __BSP_PUMP_H__

#ifdef  __BSP_PUMP_GLOBALS
#define __BSP_PUMP_EXT
#else
#define __BSP_PUMP_EXT extern
#endif

#include "stm32f4xx_hal.h"
#include "relay_task.h"
#define help_on()				relay.gas_status |=  (0x01 <<0)
#define help_off()			relay.gas_status &= ~(0x01 <<0)
#define press_on()			relay.gas_status |=  (0x01 <<1)
#define press_off()			relay.gas_status &= ~(0x01 <<1)
#define bracket_on()		relay.gas_status |=  (0x01 <<2)
#define bracket_off()		relay.gas_status &= ~(0x01 <<2)
#define throw_on()			relay.gas_status |=  (0x01 <<3)
#define throw_off()			relay.gas_status &= ~(0x01 <<3)
#define bullet1_on()		relay.gas_status |=  (0x01 <<4)
#define bullet1_off()   relay.gas_status &= ~(0x01 <<4)
#define bullet2_on()		relay.gas_status |=  (0x01 <<5)
#define bullet2_off()		relay.gas_status &= ~(0x01 <<5)



#define camera_on()			relay.electrical_status |=  (0x01 <<0)
#define camera_off()		relay.electrical_status &= ~(0x01 <<0)
#define magazine_on() 	relay.electrical_status |=  (0x01 <<1)
#define magazine_off() 	relay.electrical_status &= ~(0x01 <<1)

#define relay_safe_on() 		relay.electrical_status |=  (0x01 <<2)
#define relay_safe_off() 		relay.electrical_status &= ~(0x01 <<2)



typedef enum
{
	OFF_MODE,
	ON_MODE,
} relay_mode_e;//ģʽ


typedef struct
{	
	relay_mode_e  help_ctrl_mode;			//��Ԯ 
  relay_mode_e  press_ctrl_mode;		//��ȡ��ҩ�� 
	relay_mode_e  bracket_ctrl_mode;	//���צ��
	relay_mode_e  throw_ctrl_mode;		//�ӳ���ҩ�� 
	relay_mode_e  bullet1_ctrl_mode;	//��������1
	relay_mode_e  bullet2_ctrl_mode;	//��������2
//	relay_mode_e  interact_ctrl_mode; //����ģ�鿨
	
} pump_t;

typedef struct
{
  relay_mode_e 	camera_ctrl_mode;	//�л���������ͷ�ӽ�
	relay_mode_e  magazine_ctrl_mode;	//�л�����ģʽ
  relay_mode_e  rotate_ctrl_mode;	//�л������������ת���ģʽ
	relay_mode_e	safe_mode;				//��ת�����ȫģʽ	
} electrical_t;

__BSP_PUMP_EXT void pump_init(void);
__BSP_PUMP_EXT void set_relay_all_off(void);

__BSP_PUMP_EXT void help_executed(void);
__BSP_PUMP_EXT void press_executed(void);
__BSP_PUMP_EXT void bracket_executed(void);
__BSP_PUMP_EXT void throw_executed(void);
__BSP_PUMP_EXT void bullet1_executed(void);
__BSP_PUMP_EXT void bullet2_executed(void);

__BSP_PUMP_EXT void camera_executed(void);
__BSP_PUMP_EXT void magazine_executed(void);

__BSP_PUMP_EXT void relay_safe_executed(void);

__BSP_PUMP_EXT pump_t pump;
__BSP_PUMP_EXT electrical_t electrical;

#endif

