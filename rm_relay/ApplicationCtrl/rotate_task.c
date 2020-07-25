#define __ROTATE_TASK_GLOBALS
#include "rotate_task.h"
#include "bsp_can.h"
#include "cmsis_os.h"
#include "pid.h"
#include "comm_task.h"
#include "string.h"
#include "math.h"


void rotate_init()
{
		//旋转电机的PID 速度pi/位置p(d)
		PID_struct_init(&pid_rotate_spd, POSITION_PID, 15000, 3000,
									20.0f,	0.0f,	0.0f	);  
		PID_struct_init(&pid_rotate_pos, POSITION_PID, 15000, 3000,
									0.0082f,	0.0f,	0.0f	);  
	
	rotate.loose_angle_ref 	= -20000;
	rotate.bullet_angle_ref	= -82500;//(81000~85000)

}

//电流0为正,往内
//void rotate_task(void const *argu)
//{

//	switch (rotate.ctrl_mode)
//	{
//		case ROTATE_ADJUST_MODE: //暂时调节PID模式
//		{
//			//注意使用CH5进行调试 注意不要和横移微调模式冲突
//			
//			rotate.spd_ref = 0.0005 * rc.ch5; //调节速度环
//			pid_calc(&pid_rotate_spd,moto_rotate[0].speed_rpm,rotate.spd_ref);
//			
//			rotate.current[0] = 0;
//			rotate.current[1] = -rotate.current[0];
//			
//		}break;
//			
//		case ROTATE_SAFE_MODE:
//		{
//			rotate.current[0] = 0;
//			rotate.current[1] = -rotate.current[0]; //电机共存亡
//	
//		}break;
//		
//		case ROTATE_AUTO_MODE:
//		{
//			
//		  //只用一边的电机作误差反馈计算
//				rotate.spd_ref 		= pid_calc(&pid_rotate_pos,moto_rotate[0].total_ecd,0); //位置环
//				rotate.current[0] = pid_calc(&pid_rotate_spd,moto_rotate[0].speed_rpm,rotate.spd_ref); //速度环
//				rotate.current[1] = -rotate.current[0] ; //赋值两电机相同的电流值
//		
//		}break;
//	
//	}
//	
//	
//	memcpy(motor_cur.rotate_cur, rotate.current, sizeof(rotate.current));
//	osSignalSet(can_msg_send_task_t, ROTATE_MOTOR_MSG_SEND);
//}

void rotate_task(void const *argu)
{
	rotate.ecd_fdb=moto_rotate[0].total_ecd - rotate.ecd_offset;
	
	if(rotate.state != ROTATE_KNOWN ) //自动模式且未知状态下进行校准
	{
		switch(rotate.state)
		{
			case ROTATE_UNKNOWN:
			{
				rotate.ecd_offset = 0;
				rotate.current[0] = 1200;
				rotate.current[1] = -1200;
				static int handle_lt_times_rotate=0;
				if(handle_lt_times_rotate>40)
				{rotate.state = ROTATE_CALIBRA;}
				handle_lt_times_rotate++;
			}break;
			case ROTATE_CALIBRA:
			{
		
				if(moto_rotate[0].speed_rpm<10)
				{
					{
					 rotate.current[0] = pid_calc(&pid_rotate_spd,moto_rotate[0].speed_rpm,0); //电机停转
					 rotate.current[1] = pid_calc(&pid_rotate_spd,moto_rotate[1].speed_rpm,0);
					 static int handle_lt_times_rotate=0;
				   if(handle_lt_times_rotate>40)
				   {
						rotate.state = ROTATE_KNOWN;
					  rotate.ecd_offset=moto_rotate[0].total_ecd; //获得补偿数值
					 }
				   handle_lt_times_rotate++;
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
			 case ROTATE_ON:
			 {
				rotate.spd_ref = pid_calc(&pid_rotate_pos,rotate.ecd_fdb,rotate.bullet_angle_ref); //位置环
				rotate.current[0] = pid_calc(&pid_rotate_spd,moto_rotate[0].speed_rpm,rotate.spd_ref); //速度环
				rotate.current[1] = -rotate.current[0];		 
			 }break;
			 case ROTATE_OFF:
			 {
				rotate.spd_ref = pid_calc(&pid_rotate_pos,rotate.ecd_fdb,rotate.loose_angle_ref); //位置环
				rotate.current[0] = pid_calc(&pid_rotate_spd,moto_rotate[0].speed_rpm,rotate.spd_ref); //速度环
				rotate.current[1] = -rotate.current[0];	
			 }break;	
			 case ROTATE_STOP:
			 {
				rotate.current[0] = pid_calc(&pid_rotate_spd,moto_rotate[0].speed_rpm,0); //电机停转
				rotate.current[1] = pid_calc(&pid_rotate_spd,moto_rotate[1].speed_rpm,0);
				rotate.ecd_ref = rotate.ecd_fdb;
			 }break;	
		}	 
	 }
	
	send_cur(rotate.current[0],rotate.current[1],0,0);
 }
			

void rotate_get_position_flag()
{

		if (fabs((double)(rotate.ecd_fdb-rotate.bullet_angle_ref))<4000) 
		{
			rotate.position_flag = ROTATE_BULLET_POS_FLAG ;
		}
		else if (fabs((double)(rotate.ecd_fdb-rotate.loose_angle_ref))<4000)
		{
			rotate.position_flag = ROTATE_LOOSE_POS_FLAG ;
		}
		else if (fabs((double)(rotate.ecd_fdb-rotate.init_angle_ref))<4000)
		{
			rotate.position_flag = ROTATE_INIT_POS_FLAG ;
		}
		else
		{
			rotate.position_flag = ROTATE_OTHERS_POS_FLAG ;
		}

}
