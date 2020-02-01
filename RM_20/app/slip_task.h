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
	SLIP_UNKNOWN,		//δ֪̬
	SLIP_CALIBRA,		//У׼	
	SLIP_KNOWN			//У׼���
} slip_mode_e;	//���ƻ���


typedef enum
{
	SLIP_ADJUST,		//΢��
	SLIP_AUTO,			
	SLIP_STOP,
	SLIP_KEYBOARD,
} slip_ctrl_mode_e;//���ƻ���

typedef enum
{
	LEFT_BULLET_POS,
	CENTER_BULLET_POS,
	RIGHT_BULLET_POS,
	NEED_CALIBRATED,
	UNCERTAIN,
} slip_position_flag_e;

typedef struct
{
	slip_mode_e   				mode;	
  slip_ctrl_mode_e   		ctrl_mode;
  slip_ctrl_mode_e   		last_ctrl_mode;
	slip_position_flag_e	position_flag;
	
  int16_t         current;
	float						dist_ref;	//���ƻ��������趨ֵ
	float						dist_fdb;	//���ƻ������뷴��ֵ
	float						dist_offset;
	float						dist_offset_left;		//�����󲹳�
	float						dist_offset_right;	//�����Ҳ���
	int16_t					spd_ref;
	int16_t 				last_speed_rpm; 
	int16_t 				left_limit;
	int16_t					right_limit;
	int16_t					left_bullet_REF;
	int16_t 				right_bullet_REF;
	int16_t					center_bullet_REF;
	
} slip_t;

__SLIP_TASK_EXT slip_t slip;
void slip_task(void const *argu);
void slip_get_position_flag(void);
void slip_init(void);

#endif
