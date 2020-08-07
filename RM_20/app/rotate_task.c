#define __ROTATE_TASK_GLOBALS
#include "rotate_task.h"
#include "bsp_can.h"
#include "bsp_motor.h"
#include "cmsis_os.h"
#include "pid.h"
#include "comm_task.h"
#include "string.h"
#include "remote_msg.h"
#include "math.h"


#define rotate_ratio (82.0f)


extern TaskHandle_t can_msg_send_task_t;
void rotate_init()
{
		//旋转电机的PID 速度pi/位置p(d)
		PID_struct_init(&pid_rotate_spd, POSITION_PID, 10000, 1000,
									32.0f,	0.46f,	0.0f	);  
		PID_struct_init(&pid_rotate_pos, POSITION_PID, 180, 500,
									0.5f,	0.0f,	0.0f	);  
}


void rotate_task(void const *argu)
{
	static int handle_lt_times_rotate=0;
	static int handle_lt_times_rotate2=0;
	rotate.ecd_fdb = moto_rotate[0].total_ecd - rotate.ecd_offset;
	rotate.cnt_fdb = -rotate.ecd_fdb/rotate_ratio;
	
	if(rotate.state != ROTATE_KNOWN && rotate.ctrl_mode == ROTATE_AUTO) //自动模式且未知状态下进行校准
	{
		switch(rotate.state)
		{
			case ROTATE_UNKNOWN:
			{
				rotate.ecd_offset = 0;
				rotate.current[0] = 500;
				rotate.current[1] = -500;
				
				if(handle_lt_times_rotate>10)
				{
					rotate.state = ROTATE_CALIBRA;
					handle_lt_times_rotate=0;
				}
				handle_lt_times_rotate++;
			}break;
			case ROTATE_CALIBRA:
			{
		
				if(fabs((double)(moto_rotate[0].speed_rpm<10)))
				{
					{
					 rotate.current[0] = 0; //电机停转
					 rotate.current[1] = 0;
					 
					handle_lt_times_rotate2++;
				   if(handle_lt_times_rotate2>30)
				   {
						 handle_lt_times_rotate2=0;
						rotate.state = ROTATE_KNOWN;
					  rotate.ecd_offset=moto_rotate[0].total_ecd; //获得补偿数值
					 }
				   
					}
				}
			 }break;
			case ROTATE_KNOWN :
			{
				
			}break;	
		 }				 
	 }
	 else
	 {
		 switch(rotate.ctrl_mode) 
		{
			 
			 case ROTATE_STOP:
			 {
				rotate.current[0] = 0; //电机停转
				rotate.current[1] = 0;

			 }break;	
			 
			 case ROTATE_AUTO:
			 {
 
				rotate.spd_ref= pid_calc(&pid_rotate_pos,rotate.cnt_fdb,rotate.cnt_ref); 
				 
				pid_calc(&pid_rotate_spd,-moto_rotate[0].speed_rpm,rotate.spd_ref);
				rotate.current[0] =  -pid_rotate_spd.pos_out;
				rotate.current[1] =  -rotate.current[0];
				
				
			 }break;
			 		 
		}	 
	 }
	
		memcpy(motor_cur.rotate_cur, rotate.current, sizeof(rotate.current));
		osSignalSet(can_msg_send_task_t, ROTATE_MOTOR_MSG_SEND);
 }

