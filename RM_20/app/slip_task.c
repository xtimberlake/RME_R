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
		//横移的PID 
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
	
	slip_get_position_flag(); //获取当前总体位置
	
	if(slip.mode != SLIP_KNOWN && slip.ctrl_mode == SLIP_AUTO) //自动模式且未知状态下进行校准
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
					if(fabs(slip.dist_offset_left)<0.01) 	//左边校正未确定
					{
						slip.current = -1000;
						static int handle_lt_times=0;
						handle_lt_times++;
						if(handle_lt_times>30 && moto_slip.speed_rpm>-100) //检测堵转
						{
						 slip.dist_offset_left=moto_slip.total_ecd/slip_ratio; //获得左边校准数值
						 slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,0); //电机停转
						}
					
					}
					else if(fabs(slip.dist_offset_right)<0.01) //右边校正未确定
					{
						slip.current = 1000;
						static int handle_rg_times=0;
						handle_rg_times++;
						if(handle_rg_times>30 && moto_slip.speed_rpm<100) //检测堵转
						{
						 slip.dist_offset_right=moto_slip.total_ecd/slip_ratio; //获得右边校准数值
						 slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,0); //电机停转
						}
					
					}
					else //校准成功
					{
						slip.dist_offset = (slip.dist_offset_left + slip.dist_offset_right)/2 ; //计算最终offset，零点为中间位置
						slip.dist_fdb =  moto_slip.total_ecd/slip_ratio  -  slip.dist_offset;   //获得校准后的位置信息
						slip.dist_ref =  slip.dist_fdb;
						slip.dist_ref = 0;			//设置目标位置为中间
						slip.mode = SLIP_KNOWN; //进入已知状态
					
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
						slip.spd_ref = pid_calc(&pid_slip_pos,slip.dist_fdb,slip.dist_ref); //位置环
						slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,slip.spd_ref); //速度环
					
					}break;
		case SLIP_STOP:
					{
						slip.current = 0;
						slip.dist_ref = slip.dist_fdb; //设置当前位置为目标位置
					} break;
		case SLIP_AUTO:
					{
						
						slip.spd_ref = pid_calc(&pid_slip_pos,slip.dist_fdb,slip.dist_ref); //位置环
						slip.current = pid_calc(&pid_slip_spd,moto_slip.speed_rpm,slip.spd_ref); //速度环
						
					}break;
		case SLIP_KEYBOARD:
					{
						keyboard_handle();
					
					}break;			
	
	
	}
}

	
	motor_cur.slip_cur = slip.current;
	osSignalSet(can_msg_send_task_t, ROTATE_MOTOR_MSG_SEND); //发送至can2

}

void slip_get_position_flag()
{
	if(slip.mode != SLIP_KNOWN) {slip.position_flag = NEED_CALIBRATED ;slip.mode = SLIP_UNKNOWN;} //未校准处于未确定状态
	else
	{
		//处于中间子弹位置
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
