#ifndef __ROTATE_TASK_H__
#define __ROTATE_TASK_H__
#ifdef  __ROTATE_TASK_GLOBALS
#define __ROTATE_TASK_EXT
#else
#define __ROTATE_TASK_EXT extern
#endif

#include "stm32f1xx_hal.h"

#define ROTATE_PERIOD 10

typedef enum
{
	ROTATE_UNKNOWN,		//未知态
	ROTATE_CALIBRA,		//校准	
	ROTATE_KNOWN			//校准完成
} rotate_init_e;	//横移机构

typedef enum
{
	ROTATE_STOP,
	ROTATE_ON,
	ROTATE_OFF	
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
	rotate_init_e		state;
	rotate_mode_t   ctrl_mode;
	
  int16_t         current[2]; //赋予两个电机相同的电流值
	
	float						ecd_ref;	//设定值
	float						ecd_fdb;	//反馈值
	float						ecd_offset;//补偿
	
	int16_t					spd_ref; //只使用一边的电机反馈
	int32_t 				bullet_angle_ref;
	int32_t 				loose_angle_ref;
	int32_t  				init_angle_ref;
	rotate_position_flag_e position_flag;

}rotate_t;

__ROTATE_TASK_EXT rotate_t rotate;

void rotate_task(void const *argu);

void rotate_init(void);













#endif
