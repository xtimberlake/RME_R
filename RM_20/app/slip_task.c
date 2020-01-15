#define __SLIP_TASK_GLOBALS
#include "slip_task.h"
#include "pid.h"
#include "bsp_can.h"
#include "bsp_motor.h"
#include "cmsis_os.h"
#include "string.h"


#define slip_ratio (8192.0f)

extern TaskHandle_t can_msg_send_task_t;

void slip_init()
{
		//ºáÒÆµÄPID 
		PID_struct_init(&pid_slip_spd, POSITION_PID, 15000, 3000,
									10.0f,	0.02f,	0.0f	);  
		PID_struct_init(&pid_slip_pos, POSITION_PID, 15000, 3000,
									10.0f,	0.02f,	0.0f	);  
 	
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
	
	
	if(slip.mode == SLIP_UNKNOWN && slip.ctrl_mode == SLIP_AUTO)
	{
		switch(slip.mode)
		{
			case SLIP_UNKNOWN:
			{
				
				
			}break;
			case SLIP_CALIBRA:
			{
				
				
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
					
						
					}break;
		case SLIP_STOP:
					{
					slip.current = 0;
						
					} break;
		case SLIP_AUTO:
					{
					
						
					}break;
		case SLIP_KEYBOARD:
					{
					
					}break;			
	
	
	}
}


	motor_cur.slip_cur = slip.current;
	osSignalSet(can_msg_send_task_t, UPLIFT_MOTOR_MSG_SEND);

}
