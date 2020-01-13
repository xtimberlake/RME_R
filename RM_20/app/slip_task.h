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
	UPLIFT_UNKNOWN,		//δ֪̬
	UPLIFT_CALIBRA,		//У׼	
	UPLIFT_KNOWN			//У׼���
} slip_mode_e;	//���ƻ���


typedef enum
{
	UPLIFT_ADJUST,		//΢��
	UPLIFT_AUTO,			
	UPLIFT_STOP,
} slip_ctrl_mode_e;//���ƻ���

typedef struct
{
	slip_mode_e   mode;	
  slip_ctrl_mode_e   ctrl_mode;
  slip_ctrl_mode_e   last_ctrl_mode;
	
  int16_t         current;
	float						dist_ref;	//���ƻ��������趨ֵ
	float						dist_fdb;	//���ƻ������뷴��ֵ
	float						dist_offset;//�Ӵ�����翪�غ�̬����
	int16_t					spd_ref;
	uint8_t 				limit1;
	
	uint8_t					slip_limit_front;	//������λ���أ�ǰ��
	uint8_t 				slip_limit_back;	//������λ���أ���
	
	uint8_t 				last_down_limit1;
	uint8_t 				last_down_limit2;
	
	uint8_t  				offset_flag;//�Զ�ģʽ��У׼̧������������ģʽ�ɲ���

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
