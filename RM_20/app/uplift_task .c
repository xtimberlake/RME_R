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
#include "cmsis_os.h"
#include "usart.h"
#include "comm_task.h"
#include "string.h"	
#include "modeswitch_task.h"
#include "remote_msg.h"
#include "pid.h"
#include "stdlib.h"
#include "bsp_can.h"
#include "bsp_motor.h"
#include "data_processing.h"
#include "math_calcu.h"

#define uplift_ratio (8192.0f)

uint16_t uplift_test;
extern TaskHandle_t can_msg_send_task_t;

/**
  * @brief uplift_task
  * @param     
  * @attention  
	* @note  �����ʱ��
  */
void uplift_task(void const *argu)
{
	taskENTER_CRITICAL();
	uplift.height_fdb[0] =  moto_uplift[0].total_ecd/uplift_ratio  -  uplift.height_offset[0];//������Ҫ����
	uplift.height_fdb[1] = -moto_uplift[1].total_ecd/uplift_ratio  -  uplift.height_offset[1];
	uplift.down_limit1 = !HAL_GPIO_ReadPin(GPIOA,DOWN_LIMIT1_Pin);
	uplift.down_limit2 = !HAL_GPIO_ReadPin(GPIOB,DOWN_LIMIT2_Pin);
  uplift.slip_limit_front=HAL_GPIO_ReadPin(INDUCTIVE_LEFT_GPIO_Port,INDUCTIVE_LEFT_Pin);
	if(uplift.down_limit1==1)		//�Ӵ������޺������ֵ
	{
		uplift.height_offset[0] = moto_uplift[0].total_ecd/uplift_ratio ;	
//		uplift.spd_ref[0] =0;
		if(uplift.last_down_limit1==0){
			uplift.height_ref[0] = HEIGHT_DOWN_LIMIT;		//����һ��Ŀ��ֵ����
		}
	}
	if(uplift.down_limit2==1)		//�Ӵ������޺������ֵ
	{
		uplift.height_offset[1] = -moto_uplift[1].total_ecd/uplift_ratio ;
//		pid_uplift_height[1].iout = 0;
//		uplift.spd_ref[1] =0;
		if(uplift.last_down_limit2==0){
			uplift.height_ref[1] = HEIGHT_DOWN_LIMIT;
		}
	}
	uplift.last_down_limit1 = uplift.down_limit1;
	uplift.last_down_limit2 = uplift.down_limit2;
	
	if(uplift.down_limit1==1&&uplift.down_limit2==1)
		uplift.mode = UPLIFT_KNOWN;
 	
	if(uplift.ctrl_mode==UPLIFT_AUTO&&uplift.mode!=UPLIFT_KNOWN)	//�Զ�ģʽ����λ��״̬�ſ�ʼ�Զ�У׼
	{
		switch(uplift.mode)
		{
			case UPLIFT_UNKNOWN:
				uplift.mode = UPLIFT_CALIBRA;
				break;
			case UPLIFT_CALIBRA:
				
			{
				if(uplift.down_limit1==0)		//�Ӵ������޺������ֵ
					uplift.current[0] =  pid_calc(&pid_calibre_spd[0],moto_uplift[0].speed_rpm,-400);
				else
					uplift.current[0] =0;
				
				
				if(uplift.down_limit2==0)		//�Ӵ������޺������ֵ				
				uplift.current[1] =  pid_calc(&pid_calibre_spd[1],moto_uplift[1].speed_rpm,400);	
				else
					uplift.current[1] =0;		


			}	
				break;
		}
	}
	else	//
	{
		if(uplift.ctrl_mode==UPLIFT_ADJUST&&uplift.last_ctrl_mode!=UPLIFT_ADJUST)
		{
			uplift.height_ref[0] = uplift.height_fdb[0];
			uplift.height_ref[1] = uplift.height_fdb[1];
		}
		switch(uplift.ctrl_mode)
		{
			case UPLIFT_STOP:
			{
				uplift.current[0] = 0;	
				uplift.current[1] = 0;
				uplift.height_ref[0] = uplift.height_fdb[0];
				uplift.height_ref[1] = uplift.height_fdb[1];
			}
				break;
			
			case UPLIFT_ADJUST:
			{
			
//				uplift.height_ref[0] -= 0.00056*rc.ch5;
//				uplift.height_ref[1] -= 0.00056*rc.ch5;
				
				uplift.spd_ref[0] = -4*rc.ch5; //�����ٶȻ�
				

				uplift.current[0] = pid_calc(&pid_uplift_spd[0],moto_uplift[0].speed_rpm,uplift.spd_ref[0]);
				uplift.current[1] = -uplift.current[0]; //�����ͬ�������ֵ
		
		
			}
			break;
				
			case UPLIFT_AUTO:
			{
							//��λδȷ��
//				if(uplift.height_ref[0]>uplift.height_up_limit) 
//					uplift.height_ref[0] = uplift.height_up_limit;
//				if(uplift.height_ref[1]>uplift.height_up_limit)
//					uplift.height_ref[1] = uplift.height_up_limit;
			uplift.current[0] = 0;	
			uplift.current[1] = 0;
				
			
			}
			break;
		}
	}
	uplift.last_ctrl_mode = uplift.ctrl_mode;
	
	taskEXIT_CRITICAL();
	
	memcpy(motor_cur.uplift_cur, uplift.current, sizeof(uplift.current));
	osSignalSet(can_msg_send_task_t, UPLIFT_MOTOR_MSG_SEND);
}

void uplift_init()
{
	//��λУ׼��PID
		PID_struct_init(&pid_calibre_spd[0], POSITION_PID, 15000, 4000,
									10.0f,	0.03f,	0.0f	);  
		PID_struct_init(&pid_calibre_spd[1], POSITION_PID, 15000, 4000,
									10.0f,	0.03f,	0.0f	); 
//		PID_struct_init(&pid_calibre_height[0], POSITION_PID, 3000, 500,
//									2500.0f,	5.0f,	0.0f	);  
//		PID_struct_init(&pid_calibre_height[1], POSITION_PID, 3000, 500,
//									2500.0f,	5.0f,	0.0f	);  

		//̧����PID 
		PID_struct_init(&pid_uplift_spd[0], POSITION_PID, 15000, 3000,
									15.0f,	0.001f,	0.0f	);  
		PID_struct_init(&pid_uplift_spd[1], POSITION_PID, 15000, 3000,
									10.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_height[0], POSITION_PID, 2800, 500,
									800.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_uplift_height[1], POSITION_PID, 2800, 500,
									800.0f,	0.0f,	0.0f	);  
	
		//���ȵ�PID 
		PID_struct_init(&pid_retract_spd[0], POSITION_PID, 15000, 15000,
									10.0f,	0.02f,	0.0f	);  
		PID_struct_init(&pid_retract_spd[1], POSITION_PID, 15000, 15000,
									10.0f,	0.02f,	0.0f	);  
		PID_struct_init(&pid_retract_height[0], POSITION_PID, 3000, 500,
									800.0f,	5.0f,	0.0f	);  
		PID_struct_init(&pid_retract_height[1], POSITION_PID, 3000, 500,
									800.0f,	5.0f,	0.0f	);  
	
	uplift.mode = UPLIFT_UNKNOWN;

	//��ʼ�������߶ȷ���
	
	uplift.height_get_bullet_REF = 0;
	uplift.height_give_hero_REF  = 0;
	
}


			//����������
//			if(uplift.height_ref[0]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[0] = HEIGHT_DOWN_LIMIT;
//			if(uplift.height_ref[1]<=HEIGHT_DOWN_LIMIT)	uplift.height_ref[1] = HEIGHT_DOWN_LIMIT;
//			if(uplift.height_ref[0]>=HEIGHT_UP_LIMIT)	uplift.height_ref[0] = uplift.height_up_limit;
//			if(uplift.height_ref[1]>=HEIGHT_UP_LIMIT)	uplift.height_ref[1] = uplift.height_up_limit;

