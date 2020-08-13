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
	MOVE_MODE,
	GET_BULLET_SINGLE_MODE,	//ȡ����
  GET_BULLET_FRONT_MODE,	//ȡǰ������
	GET_BULLET_T_MODE,			//ȡT������
	GET_BULLET_TWO_MODE,		//ȡǰ������
	GIVE_BULLET_MODE,
} func_mode_e;

typedef enum 
{
	CAMERA_NORMAL_MODE,
	CAMERA_BULLET_MODE,	
	CAMERA_HELP_MODE,
}camera_mode_e;


typedef enum
{
	VOID_HANDLE,//ռλ,����ʱ�����κβ���
	AIM_BULLET,
	
	//����
	SINGLE_READY,
	ROT_OUT,
	ROT_OFF,
	TAKE_OFF,
	
	
	//����
	LEFT_POS,
	ROT_OUT_LEFT,
	ROT_OFF_LEFT,
	
	TAKE_OFF_AND_OUT,		
	ROT_OFF_MID,	
	
	TAKE_OFF_AND_OUT_MID,	
	ROT_OFF_FINAL,
	TAKE_OFF_FINAL,
	
	
	
	BULLET_RESET_STEP,
//	BULLET_SAFETY_MODE,
	
}bullet_step_e;//��״̬����ȡ������


__MODESW_TASK_EXT bullet_step_e		bullet_step_front;
__MODESW_TASK_EXT bullet_step_e		bullet_step_single;

__MODESW_TASK_EXT engineer_mode_e glb_ctrl_mode;
__MODESW_TASK_EXT engineer_mode_e last_glb_ctrl_mode;

__MODESW_TASK_EXT func_mode_e func_mode;
__MODESW_TASK_EXT func_mode_e last_func_mode;
__MODESW_TASK_EXT camera_mode_e camera_mode;

__MODESW_TASK_EXT int camera_flag;	//����lcd��ʾ�ӽ�

void mode_switch_task(void const *argu);

void get_main_ctrl_mode(void);
void get_global_last_mode(void);
void rc_move_handle(void);
void rc_bullet_handle(void);
void safety_mode_handle(void);
void kb_handle(void);

void get_bullet_front(void);	//ȡǰ������
void get_bullet_single(void);	//�Զ�ȡһ��

#endif
