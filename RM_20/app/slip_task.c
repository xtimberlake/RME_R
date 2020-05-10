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


#define slip_ratio (8192.0f)

extern TaskHandle_t can_msg_send_task_t;

void slip_init()
{
		//���Ƶ�PID 
		PID_struct_init(&pid_slip_spd, POSITION_PID, 7000, 3000,
									15.0f,	0.001f,	0.5f	);  
		
		PID_struct_init(&pid_slip_pos, POSITION_PID, 15000, 3000,
									400.0f,	0.001f,	0.0f	);  
 	
		slip.mode = SLIP_UNKNOWN;
		slip.dist_offset_left  = 0;
		slip.dist_offset_right = 0;
		slip.dist_offset			 = 0;

//	uplift.height_up_limit = HEIGHT_UP_LIMIT;
//	uplift.height_give = HEIGHT_GIVE;
//	uplift.height_get = HEIGHT_GET;
//	uplift.height_aim = HEIGHT_AIM;
//	uplift.height_normal = HEIGHT_NORMAL;
//	
//	uplift.height_climb_up_hang = 	HEIGHT_CLIMB_UP_HANG;
//	uplift.height_climb_up_zhuzi = HEIGHT_CLIMB_UP_ZHUZI;
//	uplift.height_climb_on_top = HEIGHT_CLIMB_ON_TOP;
//	uplift.height_climb_down_hang = HEIGHT_CLIMB_DOWN_HANG;
////	uplift.height_climb_down_zhuzi = HEIGHT_CLIMB_ON_TOP;
//	uplift.height_climb_retract = HEIGHT_CLIMB_RETRACT;
}

void slip_task(void const *argu)
{
	
	slip.dist_fdb =  moto_slip.total_ecd/slip_ratio  -  slip.dist_offset;
	
	slip_get_position_flag(); //��ȡ��ǰ����λ��
	
	if(slip.mode != SLIP_KNOWN && slip.ctrl_mode == SLIP_AUTO) //�Զ�ģʽ��δ֪״̬�½���У׼
	{
		switch(slip.mode)
		{
			case SLIP_UNKNOWN:
			{
				slip.dist_offset = 0;
				slip. mode = SLIP_CALIBRA;
		
			}break;
			case SLIP_CALIBRA:
			{
					if(fabs(slip.dist_offset_left)<0.01) 	//���У��δȷ��
					{
						slip.current = -1000;
						static int handle_lt_times=0;
						handle_lt_times++;
						if(handle_lt_times>30 && moto_slip.speed_rpm>-100) //����ת
						{
						 slip.dist_offset_left=moto_slip.total_ecd/slip_ratio; //������У׼��ֵ
						 slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,0); //���ͣת
						}
					
					}
					else if(fabs(slip.dist_offset_right)<0.01) //�ұ�У��δȷ��
					{
						slip.current = 1000;
						static int handle_rg_times=0;
						handle_rg_times++;
						if(handle_rg_times>30 && moto_slip.speed_rpm<100) //����ת
						{
						 slip.dist_offset_right=moto_slip.total_ecd/slip_ratio; //����ұ�У׼��ֵ
						 slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,0); //���ͣת
						}
					
					}
					else //У׼�ɹ�
					{
						slip.dist_offset = (slip.dist_offset_left + slip.dist_offset_right)/2 ; //��������offset�����Ϊ�м�λ��
						slip.dist_fdb =  moto_slip.total_ecd/slip_ratio  -  slip.dist_offset;   //���У׼���λ����Ϣ
						slip.dist_ref =  slip.dist_fdb;
						slip.dist_ref = 0;			//����Ŀ��λ��Ϊ�м�
						slip.mode = SLIP_KNOWN; //������֪״̬
					
					}
									
					
			}break;
			case SLIP_KNOWN:
			{
				
			
			}break;	
			
		}
	}
	else
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
						keyboard_handle();
					
					}break;			
	
	
	}
}

	
	motor_cur.slip_cur = slip.current;
	osSignalSet(can_msg_send_task_t, ROTATE_MOTOR_MSG_SEND); //������can2

}

void slip_get_position_flag()
{
	if(slip.mode != SLIP_KNOWN) {slip.position_flag = NEED_CALIBRATED ;slip.mode = SLIP_UNKNOWN;} //δУ׼����δȷ��״̬
	else
	{
		//�����м��ӵ�λ��
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
}
