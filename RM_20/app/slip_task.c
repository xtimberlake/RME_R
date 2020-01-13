#define __SLIP_TASK_GLOBALS
#include "slip_task.h"
#include "pid.h"
#include "bsp_can.h"
#include "bsp_motor.h"

#define slip_ratio (8192.0f)

void slip_init()
{
	//对位校准的PID
		PID_struct_init(&pid_slip_spd, POSITION_PID, 15000, 4000,
									10.0f,	0.03f,	0.0f	);  
		PID_struct_init(&pid_slip_pos, POSITION_PID, 15000, 4000,
									10.0f,	0.03f,	0.0f	); 
	
		//横移的PID 
		PID_struct_init(&pid_uplift_spd[0], POSITION_PID, 15000, 3000,
									10.0f,	0.02f,	0.0f	);  
		PID_struct_init(&pid_uplift_spd[1], POSITION_PID, 15000, 3000,
									10.0f,	0.02f,	0.0f	);  
 	
		slip.mode = UPLIFT_UNKNOWN;

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
