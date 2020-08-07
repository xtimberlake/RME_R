/** 
  * @file uplift_task.c
  * @version 1.1
  * @date Mar,2 2019
	*
  * @brief  ̧����������
	*
  *	@author li zh
  *
  */
#define __UPLIFT_TASK_GLOBALS
#include "uplift_task.h"
#include "pid.h"
#include "bsp_can.h"
#include "bsp_motor.h"
#include "cmsis_os.h"
#include "string.h"
#include "remote_msg.h"
#include "math.h"
#include "keyboard_handle.h"

#define uplift_ratio (8192.0f)

uint16_t uplift_test;
extern TaskHandle_t can_msg_send_task_t;

void uplift_init()
{
		//̧����PID  ��е������,�ϰ�˵����е���,������ͨ�������ܵ���
		PID_struct_init(&pid_uplift_spd[0], POSITION_PID, 15000, 3000,
									10.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_spd[1], POSITION_PID, 15000, 3000,
									10.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_height[0], POSITION_PID, 2800, 500,
									800.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_height[1], POSITION_PID, 2800, 500,
									800.0f,	0.0f,	0.0f	);  
	
	
	uplift.state = UPLIFT_UNKNOWN;

	//��ʼ�������߶ȷ���
	
	uplift.height_get_bullet_REF = HEIGHT_GET;//ȡ���߶�
	uplift.height_give_hero_REF  = 0;					//�����߶�
	uplift.calibra_flag1=1;				//��ʼ����־λ
	uplift.calibra_flag2=1;
	uplift.height_offset[0] = 0;	//�߶Ȳ���ֵ
	uplift.height_offset[1] = 0;
}
/**
  * @brief uplift_task
  * @param     
  * @attention  δ����
	* @note  �����ʱ��
  */
void uplift_task(void const *argu)
{
	taskENTER_CRITICAL();
	
	uplift.height_fdb[0] =  moto_uplift[0].total_ecd/uplift_ratio  -  uplift.height_offset[0];
	uplift.height_fdb[1] =  moto_uplift[1].total_ecd/uplift_ratio  -  uplift.height_offset[1];
	
	if(uplift.state != UPLIFT_KNOWN && uplift.ctrl_mode == UPLIFT_AUTO) //�Զ�ģʽ��δ֪״̬�½���У׼ 
	{
		switch(uplift.state)
		{
			case UPLIFT_UNKNOWN:
			{
				uplift.state = UPLIFT_CALIBRA;
			}break;
			case UPLIFT_CALIBRA:
			{
				uplift.height_offset[0]=moto_uplift[0].total_ecd/uplift_ratio;
				uplift.height_offset[1]=moto_uplift[1].total_ecd/uplift_ratio;
				uplift.state = UPLIFT_KNOWN;
			}break;
			case UPLIFT_KNOWN:
			{
						
			}break;
		}	
	}
	else
	{
		switch(uplift.ctrl_mode)
		{
			case UPLIFT_ADJUST:
			{

				uplift.height_ref[0] -= 0.001*rc.ch5; 
				uplift.height_ref[1] =  -uplift.height_ref[0];
				
//				//����������
				if(uplift.height_ref[0]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[0] = HEIGHT_DOWN_LIMIT;
				if(uplift.height_ref[1]>=HEIGHT_DOWN_LIMIT)	uplift.height_ref[1] = HEIGHT_DOWN_LIMIT;
//				if(uplift.height_ref[0]>=HEIGHT_UP_LIMIT)	uplift.height_ref[0] = HEIGHT_UP_LIMIT;
//				if(uplift.height_ref[1]>=HEIGHT_UP_LIMIT)	uplift.height_ref[1] = HEIGHT_UP_LIMIT;
				
				uplift.spd_ref[0] = pid_calc(&pid_uplift_height[0],uplift.height_fdb[0],uplift.height_ref[0]); //�߶Ȼ�
				uplift.current[0] = pid_calc(&pid_uplift_spd[0],moto_uplift[0].speed_rpm,uplift.spd_ref[0]); //�ٶȻ�

				uplift.spd_ref[1] = pid_calc(&pid_uplift_height[1],uplift.height_fdb[1],uplift.height_ref[1]); //�߶Ȼ�
				uplift.current[1] = pid_calc(&pid_uplift_spd[1],moto_uplift[1].speed_rpm,uplift.spd_ref[1]); //�ٶȻ�		

			}break;
			case UPLIFT_STOP:
			{
				uplift.current[0] = 0;
				uplift.current[1] = 0;
				uplift.height_ref[0] =uplift.height_fdb[0] ; //���õ�ǰλ��ΪĿ��λ��
				uplift.height_ref[1] =uplift.height_fdb[1] ; 
			}break;
			case UPLIFT_AUTO:
			{
				//����������
				if(uplift.height_ref[0]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[0] = HEIGHT_DOWN_LIMIT;
				if(uplift.height_ref[1]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[1] = HEIGHT_DOWN_LIMIT;
				if(uplift.height_ref[0]>=HEIGHT_UP_LIMIT)	uplift.height_ref[0] = HEIGHT_UP_LIMIT;
				if(uplift.height_ref[1]>=HEIGHT_UP_LIMIT)	uplift.height_ref[1] = HEIGHT_UP_LIMIT;
								
				uplift.spd_ref[0] = pid_calc(&pid_uplift_height[0],uplift.height_fdb[0],uplift.height_ref[0]); //�߶Ȼ�
				uplift.current[0] = pid_calc(&pid_uplift_spd[0],moto_uplift[0].speed_rpm,uplift.spd_ref[0]); //�ٶȻ�
				
				uplift.height_ref[1] =  -uplift.height_ref[0];

				uplift.spd_ref[1] = pid_calc(&pid_uplift_height[1],uplift.height_fdb[1],uplift.height_ref[1]); //�߶Ȼ�
				uplift.current[1] = pid_calc(&pid_uplift_spd[1],moto_uplift[1].speed_rpm,uplift.spd_ref[1]); //�ٶȻ�						
			}break;
			case UPLIFT_KEYBORAD:
			{
				//keyboard_handle();
			}break;
		}
	
	}
	
	uplift.last_ctrl_mode = uplift.ctrl_mode;
	
	taskEXIT_CRITICAL();
	
	memcpy(motor_cur.uplift_cur, uplift.current, sizeof(uplift.current));
	osSignalSet(can_msg_send_task_t, UPLIFT_MOTOR_MSG_SEND);
}



