/** 
  * @file modeswitch_task.h
  * @version 1.0
  * @date Mar,05th 2018
	*
  * @brief  模式选择
	*
  *	@author lin zh
  *
  */
	
#ifndef __MODESWITCH_TASK_H__
#define __MODESWITCH_TASK_H__

#ifdef  __MODESW_TASK_GLOBALS
#define __MODESW_TASK_EXT
#else
#define __MODESW_TASK_EXT extern
#endif
#include "stm32f4xx_hal.h"

#define INFO_GET_PERIOD 5

//工程整机模式
typedef enum
{
  SAFETY_MODE,//保护模式
	RC_MOVE_MODE,//遥控器底盘移动模式（抬升机构可微调）
	RC_BULLET_MODE,//遥控器各组件测试模式
	KB_MODE,//键盘模式
	DEBUG_MODE,
} engineer_mode_e;

//功能模式
typedef enum
{
  GET_BULLET1_MODE,
	GET_BULLET2_MODE,
	GIVE_BULLET_MODE,
	MOVE_MODE,
} func_mode_e;

typedef enum 
{
	CAMERA_NORMAL_MODE,
	CAMERA_CLIMB_MODE,
	CAMERA_BULLET_MODE,	
	CAMERA_HELP_MODE,
}camera_mode_e;


typedef enum
{
	BULLET_AIM,					//贴墙后左右对位,夹弹爪子打开,高度提高到取弹高度
	
	BULLET_ROTATE_INIT,	//转动爪子到中值
	BULLET_ROTATE_OUT,	//转动爪子出去
	BULLET_PRESS,				//夹取弹药箱
	BULLET_PUSH,				//倒子弹
	BULLET_THROW,				//丢弹药箱
	
	BULLET_DONE,					//取弹完成
	BULLET_DONE_MIDDLE,		//取弹完成停止位置
	BULLET_DONE_LEFT,
	BULLET_DONE_RIGHT,
	
	BULLET_STRETCH,			//伸出悬架（取第二排模式）
	BULLET_WITHDRAW,		//收回悬架
	BULLET_GET,					//抬升弹药箱
	
	BULLET_RESET,				//校准
}		get_bullet_mode_e;//用状态描述整个取弹流程

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
	
}bullet_step_e;//用状态描述整个取弹流程

typedef struct
{
	get_bullet_mode_e 	ctrl_mode;
	uint8_t	line_flag;
	uint8_t step_flag;		//单步执行的flag
	uint8_t reset_flag;
	uint8_t loosetime;
	float dis_ref1;
	float dis_ref2;
	int handle_ro_times;
} get_bullet_t;

__MODESW_TASK_EXT bullet_step_e bullet_setp;

__MODESW_TASK_EXT get_bullet_t bullet;

__MODESW_TASK_EXT engineer_mode_e glb_ctrl_mode;
__MODESW_TASK_EXT engineer_mode_e last_glb_ctrl_mode;

__MODESW_TASK_EXT func_mode_e func_mode;
__MODESW_TASK_EXT func_mode_e last_func_mode;
__MODESW_TASK_EXT camera_mode_e camera_mode;

__MODESW_TASK_EXT int camera_flag;	//决定车前后方向
void mode_switch_task(void const *argu);

void get_main_ctrl_mode(void);
void keyborad_bullet_handle_controller(void);
void get_global_last_mode(void);
void get_bullet_task(void);//
void rc_move_handle(void);
void rc_bullet_handle(void);
void rc_climb_handle(void);
void kb_handle(void);
void safety_mode_handle(void);
void climb_ctrl_mode(void);
void get_bullet_ctrl_mode(void);

#endif
