#define __SLIP_TASK_GLOBALS
#include "slip_task.h"
#include "pid.h"
#include "bsp_can.h"
#include "bsp_motor.h"
#include "cmsis_os.h"
#include "string.h"
#include "remote_msg.h"
#include "math.h"
#include "keyboard_handle.h"
#include "data_processing.h"



#define slip_ratio (820.0f)


extern TaskHandle_t can_msg_send_task_t;

void slip_init()
{
		//���Ƶ�PID 
		PID_struct_init(&pid_slip_spd, POSITION_PID, 7000, 3000,
									15.0f,	1.0f,	0.0f	);  
		
		PID_struct_init(&pid_slip_pos, POSITION_PID, 6500, 3000,
									15.0f,	0.0f,	0.0f	);  
	
 	
	  slip.ctrl_mode = SLIP_STOP;
		slip.state = SLIP_UNKNOWN;
		slip.dist_offset_left  = 0;
		slip.dist_offset_right = 0;
		slip.dist_offset			 = 0;

}

void slip_task(void const *argu)
{
	
	slip.dist_fdb =  moto_slip.total_ecd/slip_ratio  -  slip.dist_offset;
	
	slip_get_position_flag(); //��ȡ��ǰ����λ��
	
	if(slip.state != SLIP_KNOWN && slip.ctrl_mode == SLIP_AUTO) //�Զ�ģʽ��δ֪״̬�½���У׼
	{
		switch(slip.state)
		{
			case SLIP_UNKNOWN:
			{
				slip.dist_offset = 0;
				slip. state = SLIP_CALIBRA;		
			}break;
			case SLIP_CALIBRA:
			{
				
					static int handle_lt_times_slip=0;
					if(fabs(slip.dist_offset)<0.01) 	//���У��δȷ��
					{						
						slip.current = -1500;
						
						handle_lt_times_slip++;
						
						if(handle_lt_times_slip>30 && moto_slip.speed_rpm>-100) //����ת
						{
							handle_lt_times_slip=0;
							slip.dist_offset=moto_slip.total_ecd/slip_ratio; //������У׼��ֵ
							slip.current = 0; //���ͣת
						}					
					}
					else //У׼�ɹ�
					{
						handle_lt_times_slip++;
						slip.current = 0;
						if(handle_lt_times_slip>30)
						{
							slip.dist_fdb =  moto_slip.total_ecd/slip_ratio  -  slip.dist_offset;   //���У׼���λ����Ϣ
							slip.dist_ref =  493.6f;
							handle_lt_times_slip=0;
							slip.state = SLIP_KNOWN; //������֪״̬
						}						
					}

			}break;
			case SLIP_KNOWN:
			{
				
			}break;			
		}
	}else
	{
		switch(slip.ctrl_mode)
		{
			case SLIP_ADJUST: 
						{
							slip.dist_ref -= 0.002*rc.ch4;  
							slip.spd_ref = pid_calc(&pid_slip_pos,slip.dist_fdb,slip.dist_ref); //λ�û�
							slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,slip.spd_ref); //�ٶȻ�
						}break;
			case SLIP_STOP:
						{
							slip.current = 0;
							slip.dist_ref = slip.dist_fdb; //���õ�ǰλ��ΪĿ��λ��
						} break;
			case SLIP_AUTO:
						{
							slip.spd_ref = pid_calc(&pid_slip_pos,slip.dist_fdb,slip.dist_ref); //λ�û�
							slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,slip.spd_ref); //�ٶȻ�
							
						}break;
			case SLIP_KEYBOARD:
						{
							//keyboard_handle();
						
						}break;			
		
		
		}
}

	motor_cur.slip_cur = slip.current;
	osSignalSet(can_msg_send_task_t, SLIP_MOTOR_MSG_SEND); //������can2

}

void slip_get_position_flag()
{
		if (fabs((double)(slip.dist_fdb-slip.center_bullet_REF))<0.5) 
		{
			slip.position_flag = CENTER_BULLET_POS ;
		}
		else if (fabs((double)(slip.dist_fdb-slip.left_bullet_REF))<0.5)
		{
			slip.position_flag = LEFT_BULLET_POS ;
		}
		else if (fabs((double)(slip.dist_fdb-slip.right_bullet_REF))<0.5)
		{
			slip.position_flag = RIGHT_BULLET_POS ;
		}
		else
		{
			slip.position_flag = UNCERTAIN ;		
		}

}

