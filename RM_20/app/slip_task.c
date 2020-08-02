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

uint16_t thres_hold=130;

extern TaskHandle_t can_msg_send_task_t;

void slip_init()
{
		//���Ƶ�PID 
		PID_struct_init(&pid_slip_spd, POSITION_PID, 6000, 3000,
									0.0f,	0.0f,	0.0f	);  
		
		PID_struct_init(&pid_slip_pos, POSITION_PID, 6000, 3000,
									0.0f,	0.0f,	0.0f	);  
	
//	  PID_struct_init(&pid_slip_single, POSITION_PID, 10000, 4000,
//	                100.0f, 2.0f, 0.0f);
 	
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
					static int handle_lt_times=0;
					if(fabs(slip.dist_offset)<0.01) 	//���У��δȷ��
					{
						
						slip.current = -1500;
						
						handle_lt_times++;
						
						
						if(handle_lt_times>30 && moto_slip.speed_rpm>-100) //����ת
						{
							handle_lt_times=0;
						 slip.dist_offset=moto_slip.total_ecd/slip_ratio; //������У׼��ֵ
						 slip.current = 0; //���ͣת
						}
						
					
					}
					else //У׼�ɹ�
					{
						handle_lt_times++;
						slip.current = 0;
						if(handle_lt_times>30)
						{
						slip.dist_fdb =  moto_slip.total_ecd/slip_ratio  -  slip.dist_offset;   //���У׼���λ����Ϣ
						slip.dist_ref =  49.35f;
						handle_lt_times=0;
						slip.state = SLIP_KNOWN; //������֪״̬
						}
						
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
						
						//λ�õ���
						//slip.current = pid_calc_integral_exp(&pid_slip_single,slip.dist_fdb,slip.dist_ref);
						
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
//	if(slip.state != SLIP_KNOWN) {slip.position_flag = NEED_CALIBRATED ;slip.state = SLIP_UNKNOWN;} //δУ׼����δȷ��״̬
//	else
//	{
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

//	}
}

float pid_calc_integral_exp(pid_t* pid, float get, float set)
	{
    pid->get[NOW] = get;
    pid->set[NOW] = set;
    pid->err[NOW] = set - get;	//set - measure
    if (pid->max_err != 0 && ABS(pid->err[NOW]) >  pid->max_err  )
		return 0;
	if (pid->deadband != 0 && ABS(pid->err[NOW]) < pid->deadband)
		return 0;
    
    if(pid->pid_mode == POSITION_PID) //λ��ʽp
    {
        pid->pout = pid->p * pid->err[NOW];
			
				if(ABS(pid->err[NOW])>10)
				pid->iout = 0;
				else
        pid->iout += pid->i * pid->err[NOW];
			
        pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST] );
        abs_limit(&(pid->iout), pid->IntegralLimit,0);
        pid->pos_out = pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->pos_out), pid->MaxOutput,0);
        pid->last_pos_out = pid->pos_out;	//update last time 
    }
    else if(pid->pid_mode == DELTA_PID)//����ʽP
    {
        pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
        pid->iout = pid->i * pid->err[NOW];
        pid->dout = pid->d * (pid->err[NOW] - 2*pid->err[LAST] + pid->err[LLAST]);
        
        abs_limit(&(pid->iout), pid->IntegralLimit,0);
        pid->delta_u = pid->pout + pid->iout + pid->dout;
        pid->delta_out = pid->last_delta_out + pid->delta_u;
        abs_limit(&(pid->delta_out), pid->MaxOutput,0);
        pid->last_delta_out = pid->delta_out;	//update last time
    }
    
    pid->err[LLAST] = pid->err[LAST];
    pid->err[LAST] = pid->err[NOW];
    pid->get[LLAST] = pid->get[LAST];
    pid->get[LAST] = pid->get[NOW];
    pid->set[LLAST] = pid->set[LAST];
    pid->set[LAST] = pid->set[NOW];
    return pid->pid_mode==POSITION_PID ? pid->pos_out : pid->delta_out;
//	
}
