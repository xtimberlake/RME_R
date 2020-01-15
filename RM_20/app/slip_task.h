#ifndef __SLIP_TASK_H__
#define __SLIP_TASK_H__
#ifdef  __SLIP_TASK_GLOBALS
#define __SLIP_TASK_EXT
#else
#define __SLIP_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define SLIP_PERIOD 10

typedef enum
{
	SLIP_UNKNOWN,		//未知态
	SLIP_CALIBRA,		//校准	
	SLIP_KNOWN			//校准完成
} slip_mode_e;	//横移机构


typedef enum
{
	SLIP_ADJUST,		//微调
	SLIP_AUTO,			
	SLIP_STOP,
	SLIP_KEYBOARD,
} slip_ctrl_mode_e;//横移机构

typedef struct
{
	slip_mode_e   mode;	
  slip_ctrl_mode_e   ctrl_mode;
  slip_ctrl_mode_e   last_ctrl_mode;
	
  int16_t         current;
	float						dist_ref;	//横移机构距离设定值
	float						dist_fdb;	//横移机构距离反馈值
	float						dist_offset;//动态补偿
	int16_t					spd_ref;
	int16_t 				left_limit;
	int16_t					right_limit;
	
} slip_t;

__SLIP_TASK_EXT slip_t slip;
void slip_task(void const *argu);
void slip_init(void);

#endif
