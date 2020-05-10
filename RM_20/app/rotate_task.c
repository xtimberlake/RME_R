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


extern TaskHandle_t can_msg_send_task_t;
void rotate_init()
{
		//旋转电机的PID 
		PID_struct_init(&pid_rotate_spd, POSITION_PID, 15000, 3000,
									10.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_rotate_pos, POSITION_PID, 15000, 3000,
									10.0f,	0.0f,	0.0f	);  
	
	rotate.loose_angle_ref 	= 0;
	rotate.bullet_angle_ref	= 0;

}
void rotate_task(void const *argu)
{

	switch (rotate.ctrl_mode)
	{
		case ROTATE_ADJUST_MODE: //暂时调节PID模式
		{
			//注意使用CH5进行调试 注意不要和横移微调模式冲突
			
			rotate.spd_ref = 0.0005 * rc.ch5; //调节速度环
			pid_calc(&pid_rotate_spd,moto_rotate[0].speed_rpm,rotate.spd_ref);
			
			rotate.current[0] = 0;
			rotate.current[1] = -rotate.current[0];
			
		}break;
			
		case ROTATE_SAFE_MODE:
		{
			rotate.current[0] = 0;
			rotate.current[1] = -rotate.current[0]; //电机共存亡
	
		}break;
		
		case ROTATE_AUTO_MODE:
		{
		  //只用一边的电机作误差反馈计算
				rotate.spd_ref 		= pid_calc(&pid_slip_pos,rotate.angle_fdb,rotate.angle_ref); //位置环
				rotate.current[0] = pid_calc(&pid_slip_spd,moto_rotate[0].speed_rpm,rotate.spd_ref); //速度环
				rotate.current[1] = -rotate.current[0] ; //赋值两电机相同的电流值
		
		}break;
	
	}
	
	
	memcpy(motor_cur.rotate_cur, rotate.current, sizeof(rotate.current));
	osSignalSet(can_msg_send_task_t, ROTATE_MOTOR_MSG_SEND);
}

void rotate_get_position_flag()
{

		if (fabs((double)(rotate.angle_fdb-rotate.bullet_angle_ref))<0.5) 
		{
			rotate.position_flag = ROTATE_BULLET_POS_FLAG ;
		}
		else if (fabs((double)(rotate.angle_fdb-rotate.loose_angle_ref))<0.5)
		{
			rotate.position_flag = ROTATE_LOOSE_POS_FLAG ;
		}
		else if (fabs((double)(rotate.angle_fdb-rotate.init_angle_ref))<0.5)
		{
			rotate.position_flag = ROTATE_INIT_POS_FLAG ;
		}
		else
		{
			rotate.position_flag = ROTATE_OTHERS_POS_FLAG ;
		}

}
