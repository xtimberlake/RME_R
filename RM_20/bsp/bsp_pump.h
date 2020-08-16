/**
  * @file not just bsp_pump.h 有空改掉文件名 =v=
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
} relay_mode_e;//模式


typedef struct
{	
	relay_mode_e  help_ctrl_mode;			//救援 
  relay_mode_e  press_ctrl_mode;		//夹取弹药箱 
	relay_mode_e  bracket_ctrl_mode;	//伸出爪子
	relay_mode_e  throw_ctrl_mode;		//扔出弹药箱 
	relay_mode_e  bullet1_ctrl_mode;	//给弹气泵1
	relay_mode_e  bullet2_ctrl_mode;	//给弹气泵2
//	relay_mode_e  interact_ctrl_mode; //交互模块卡
	
} pump_t;

typedef struct
{
  relay_mode_e 	camera_ctrl_mode;	//切换倒车摄像头视角
	relay_mode_e  magazine_ctrl_mode;	//切换弹仓模式
  relay_mode_e  rotate_ctrl_mode;	//切换气阀板控制旋转电机模式
	relay_mode_e	safe_mode;				//旋转电机安全模式	
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

