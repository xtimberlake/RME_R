#define __SLIP_TASK_GLOBALS
#include "slip_task.h"
#include "pid.h"
#include "bsp_can.h"
#include "bsp_motor.h"
#include "cmsis_os.h"
#include "string.h"
#include "remote_msg.h"


#define slip_ratio (8192.0f)

extern TaskHandle_t can_msg_send_task_t;

void slip_init()
{
		//���Ƶ�PID 
		PID_struct_init(&pid_slip_spd, POSITION_PID, 15000, 3000,
									15.0f,	0.00001f,	0.5f	);  
		
		PID_struct_init(&pid_slip_pos, POSITION_PID, 15000, 3000,
									950.0f,	0.0f,	0.0f	);  
 	
		slip.mode = SLIP_UNKNOWN;

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
				  slip.current = 800;
				  static int handle_times=0;
					handle_times++;
					if(handle_times>30 && moto_slip.speed_rpm<100) //����ת
					{
					 slip.dist_offset=moto_slip.total_ecd/slip_ratio;
					 slip.dist_fdb =  moto_slip.total_ecd/slip_ratio  -  slip.dist_offset;
					 slip.mode = SLIP_KNOWN; //������֪״̬
					// slip.dist_ref = slip.dist_fdb ; //����һ�ε�ǰλ��ΪĿ��ֵ
					 slip.dist_ref = -33.0 ; //�м�λ��
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
						slip.dist_ref += 0.002*rc.ch4;  //����λ�û�PID
						slip.spd_ref = pid_calc(&pid_slip_pos,slip.dist_fdb,slip.dist_ref); //λ�û�
						slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,slip.spd_ref); //�ٶȻ�
//						
//						slip.spd_ref = 5*rc.ch4;  //����λ�û�PID
//						slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,slip.spd_ref); //�ٶȻ�
					
					}break;
		case SLIP_STOP:
					{
					slip.current = 0;
					slip.dist_ref = slip.dist_fdb; //���õ�ǰ�ٶ�ΪĿ���ٶ�
					} break;
		case SLIP_AUTO:
					{
						
						slip.spd_ref = pid_calc(&pid_slip_pos,slip.dist_fdb,slip.dist_ref); //λ�û�
						slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,slip.spd_ref); //�ٶȻ�
						
					}break;
		case SLIP_KEYBOARD:
					{
					
					}break;			
	
	
	}
}


	motor_cur.slip_cur = slip.current;
	osSignalSet(can_msg_send_task_t, UPLIFT_MOTOR_MSG_SEND);

}
