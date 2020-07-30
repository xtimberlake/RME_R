/**
  * @file uplift_task.h
  * @version 1.0
  * @date March,1 2019
  *
  * @brief  
  *
  *	@author lin zh
  *
  */
#ifndef __UPLIFT_TASK_H__
#define __UPLIFT_TASK_H__
#ifdef  __UPLIFT_TASK_GLOBALS
#define __UPLIFT_TASK_EXT
#else
#define __UPLIFT_TASK_EXT extern
#endif

#include "stm32f4xx_hal.h"
#include "bsp_pump.h"

#define UPLIFT_PERIOD 10

#define HEIGHT_DOWN_LIMIT		0				//��ʼ���߶�
#define HEIGHT_UP_LIMIT			1.0f		//���޵ĸ߶�
#define HEIGHT_GET					1.0f		//ȡ���߶�
#define HEIGHT_GET_2				1.0f		//ȡ�ڶ��ŵ��߶�
#define HEIGHT_GET_PULL			1.0f		//ȡ��������߶�
#define HEIGHT_GIVE					1.1f		//�����ӵ��ĸ߶�.

typedef enum
{
	UPLIFT_UNKNOWN,		//δ֪̬
	UPLIFT_CALIBRA,		//У׼	
	UPLIFT_KNOWN			//У׼���
} uplift_init_e;//��ʼ������


typedef enum
{
	UPLIFT_ADJUST,		//΢��
	UPLIFT_AUTO,			
	UPLIFT_STOP,
	UPLIFT_KEYBORAD,
} uplift_ctrl_mode_e;//̧������

typedef enum
{
	READY_HEIGHT,
	BELOW_HEIGHT,
} uplift_position_flag_e;//̧������λ�ñ�־
typedef struct
{
	uplift_init_e				 state;
  uplift_ctrl_mode_e   ctrl_mode;
  uplift_ctrl_mode_e   last_ctrl_mode;
	uplift_position_flag_e position_flag;
		
  int16_t         current[2];
	float						height_ref[2];	//̧�������߶��趨ֵ
	float						height_fdb[2];	//̧�������߶ȷ���ֵ
	float						height_offset[2];//�߶Ȳ���ֵ
	float						height_calibra_offset[2];//��ʼ�����̸߶Ȳ���ֵ
	
	int16_t					spd_ref[2];
	float						down_limit1;
	float						down_limit2;
	
	uint8_t					calibra_flag1;
	uint8_t					calibra_flag2;
	
	uint8_t 				last_down_limit1;
	uint8_t 				last_down_limit2;
									

	float height_get_bullet_REF;
	float height_give_hero_REF;
		
	float height_up_limit;
	float height_down_limit;
	
	
	float height_give;
	float height_get;
	float height_aim;
	float height_normal;
	float height_climb_on_top;
	float height_climb_retract;

} uplift_t;

__UPLIFT_TASK_EXT uplift_t uplift;
void uplift_task(void const *argu);
void uplift_init(void);

#endif
