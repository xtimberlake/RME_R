#define __ROTATE_TASK_GLOBALS
#include "rotate_task.h"
#include "cmsis_os.h"
#include "pid.h"
#include "comm_task.h"
#include "string.h"

extern TaskHandle_t can_msg_send_task_t;
void rotate_init()
{
		//ºáÒÆµÄPID 
//		PID_struct_init(&pid_slip_spd, POSITION_PID, 15000, 3000,
//									10.0f,	0.02f,	0.0f	);  
//		PID_struct_init(&pid_slip_pos, POSITION_PID, 15000, 3000,
//									10.0f,	0.02f,	0.0f	);  

}
void rotate_task(void const *argu)
{
  rotate.current[0]=0;
	rotate.current[1]=0;
	
	memcpy(motor_cur.rotate_cur, rotate.current, sizeof(rotate.current));
	osSignalSet(can_msg_send_task_t, ROTATE_MOTOR_MSG_SEND);
}
