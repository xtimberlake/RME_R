#ifndef __ROTATE_TASK_H__
#define __ROTATE_TASK_H__
#ifdef  __ROTATE_TASK_GLOBALS
#define __ROTATE_TASK_EXT
#else
#define __ROTATE_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"

#define ROTATE_PERIOD 4

typedef enum
{
	ROTATE_UNKNOWN,		//δ֪̬
	ROTATE_CALIBRA,		//У׼	
	ROTATE_KNOWN			//У׼���
} rotate_init_e;	//���ƻ���

typedef enum
{
	ROTATE_STOP,
	ROTATE_AUTO,
	
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
	
  int16_t         current[2]; //�������������ͬ�ĵ���ֵ
	
	float						ecd_ref;	//�趨ֵ
	float						ecd_fdb;	//����ֵ
	float						ecd_offset;//����
	
	float           cnt_ref;
	float           cnt_fdb;
	
	int16_t					spd_ref; //ֻʹ��һ�ߵĵ������

	int32_t 				bullet_angle_ref;
	int32_t 				loose_angle_ref;
	int32_t					stay_angle_ref;
	int32_t  				init_angle_ref;
	rotate_position_flag_e position_flag;

}rotate_t;


__ROTATE_TASK_EXT rotate_t rotate;

void rotate_task(void const *argu);
void keyborad_bullet_handle(void);

void rotate_init(void);













#endif
