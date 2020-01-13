#ifndef __SLIP_TASK_H__
#define __SLIP_TASK_H__
#ifdef  __SLIP_TASK_GLOBALS
#define __SLIP_TASK_EXT
#else
#define __SLIP_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

typedef enum
{
	UPLIFT_UNKNOWN,		//未知态
	UPLIFT_CALIBRA,		//校准	
	UPLIFT_KNOWN			//校准完成
} slip_mode_e;	//横移机构


typedef enum
{
	UPLIFT_ADJUST,		//微调
	UPLIFT_AUTO,			
	UPLIFT_STOP,
} slip_ctrl_mode_e;//横移机构

typedef struct
{
	slip_mode_e   mode;	
  slip_ctrl_mode_e   ctrl_mode;
  slip_ctrl_mode_e   last_ctrl_mode;
	
  int16_t         current;
	float						dist_ref;	//横移机构距离设定值
	float						dist_fdb;	//横移机构距离反馈值
	float						dist_offset;//接触到光电开关后动态补偿
	int16_t					spd_ref;
	uint8_t 				limit1;
	
	uint8_t					slip_limit_front;	//横移限位开关（前）
	uint8_t 				slip_limit_back;	//横移限位开关（后）
	
	uint8_t 				last_down_limit1;
	uint8_t 				last_down_limit2;
	
	uint8_t  				offset_flag;//自动模式下校准抬升机构，其他模式可不理

	float max_limit;
	float height_give;
	float height_get;
	float height_aim;
	float height_normal;
	float height_climb_up_hang;
	float height_climb_up_zhuzi;
	float height_climb_down_hang;
	float height_climb_on_top;
	float height_climb_retract;

} slip_t;

__SLIP_TASK_EXT slip_t slip;

#endif
