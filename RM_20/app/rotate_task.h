#ifndef __ROTATE_TASK_H__
#define __ROTATE_TASK_H__
#ifdef  __ROTATE_TASK_GLOBALS
#define __ROTATE_TASK_EXT
#else
#define __ROTATE_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define ROTATE_PERIOD 10

typedef enum
{
	ROTATE_SAFE_MODE,
	ROTATE_AUTO_MODE,
	ROTATE_ADJUST_MODE,
	
}rotate_mode_t;

typedef enum
{
	ROTATE_BULLET_POS_FLAG,
	ROTATE_LOOSE_POS_FLAG,
	ROTATE_INIT_POS_FLAG,
	ROTATE_OTHERS_POS_FLAG,
	
}rotate_position_flag_e;

typedef struct
{	
	rotate_mode_t   ctrl_mode;
  int16_t         current[2]; //赋予两个电机相同的电流值
	int16_t					spd_ref; //只使用一边的电机反馈
	int32_t 				up_limit;
	int32_t 				down_limit;
	int32_t 				bullet_angle_ref;
	int32_t 				loose_angle_ref;
	int32_t  				init_angle_ref;
	int32_t					angle_ref;	
	int32_t					angle_fdb;	
	rotate_position_flag_e position_flag;

}rotate_t;

__ROTATE_TASK_EXT rotate_t rotate;

void rotate_task(void const *argu);















#endif
