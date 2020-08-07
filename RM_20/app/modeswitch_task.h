/** 
  * @file modeswitch_task.h
  * @version 1.0
  * @date Mar,05th 2018
	*
  * @brief  ģʽѡ��
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

//��������ģʽ
typedef enum
{
  SAFETY_MODE,//����ģʽ
	RC_MOVE_MODE,//ң���������ƶ�ģʽ��̧��������΢����
	RC_BULLET_MODE,//ң�������������ģʽ
	KB_MODE,//����ģʽ
	DEBUG_MODE,
} engineer_mode_e;

//����ģʽ
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
	BULLET_AIM,					//��ǽ�����Ҷ�λ,�е�צ�Ӵ�,�߶���ߵ�ȡ���߶�
	
	BULLET_ROTATE_INIT,	//ת��צ�ӵ���ֵ
	BULLET_ROTATE_OUT,	//ת��צ�ӳ�ȥ
	BULLET_PRESS,				//��ȡ��ҩ��
	BULLET_PUSH,				//���ӵ�
	BULLET_THROW,				//����ҩ��
	
	BULLET_DONE,					//ȡ�����
	BULLET_DONE_MIDDLE,		//ȡ�����ֹͣλ��
	BULLET_DONE_LEFT,
	BULLET_DONE_RIGHT,
	
	BULLET_STRETCH,			//������ܣ�ȡ�ڶ���ģʽ��
	BULLET_WITHDRAW,		//�ջ�����
	BULLET_GET,					//̧����ҩ��
	
	BULLET_RESET,				//У׼
}		get_bullet_mode_e;//��״̬��������ȡ������

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
	
}bullet_step_e;//��״̬��������ȡ������

typedef struct
{
	get_bullet_mode_e 	ctrl_mode;
	uint8_t	line_flag;
	uint8_t step_flag;		//����ִ�е�flag
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

__MODESW_TASK_EXT int camera_flag;	//������ǰ����
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
