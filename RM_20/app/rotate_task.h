#ifndef __ROTATE_TASK_H__
#define __ROTATE_TASK_H__
#ifdef  __ROTATE_TASK_GLOBALS
#define __ROTATE_TASK_EXT
#else
#define __ROTATE_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define ROTATE_PERIOD 10

typedef struct
{	
  int16_t         current[2];
	int16_t					spd_ref; //只使用一边的电机反馈
	uint32_t 				up_limit;
	uint32_t 				down_limit;
	float						angle_ref;	
	float						angle_fdb;	

	float bullet_angle_ref;
	float back_angle_ref;

}rotate_t;

__ROTATE_TASK_EXT rotate_t rotate;

void rotate_task(void const *argu);















#endif
