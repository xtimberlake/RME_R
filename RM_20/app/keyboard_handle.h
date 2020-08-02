#ifndef __KEYBOARD_HANDLE
#define __KEYBOARD_HANDLE

#ifdef __KEYBOARD_HANDLE_GLOBALS
#define __KEYBOARD_HANDLE_EXT 
#else
#define __KEYBOARD_HANDLE_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define SPEED_NORMAL	2800
#define SPEED_SLOW		800
#define	SPEED_FAST		6000


extern float chassis_speed;  //确定底盘总体速度，在modeswitch中定义

typedef enum
{
	UPLIFT_READY,
	SLIP_LEFT,
	SLIP_MIDD,
	SLIP_RIGHT,
	ROTATE_GOING,
	PRESSING,
	ROTATE_BACK,
	THROWING,
	BRACKET_GOING,
	BRACKET_BACK,
	RESET_FLAG,
	STAND_BY,
}fetch_step_e;

typedef enum
{
	VOID_HANDLE,
	LEFT_POS,
	ROT_OUT,
	ROT_OFF,
	TAKE_OFF_AND_OUT,
	
	ROT_OFF_MID,
	
	TAKE_OFF_AND_OUT_MID,
	
	ROT_OFF_FINAL,
	TAKE_OFF_FINAL,
	
	BULLET_HOLD_ON,
	BULLET_RESET_STEP,
	BULLET_SAFETY_MODE,
	
}bullet_step_e;

__KEYBOARD_HANDLE_EXT fetch_step_e fetch_step;

__KEYBOARD_HANDLE_EXT bullet_step_e bullet_setp;




void debug_slip(void);
void keyborad_bullet_handle(void);
void fetch_bullet_ctrl_fun(void);
void keyboard_handle(void);
void keyboard_chassis_ctrl(void);
void keyboard_uplift_ctrl(void);
void reset_bullet(void);
void single_fetch_bullet(void);
void single_second_fetch_bullet(void);

#endif
