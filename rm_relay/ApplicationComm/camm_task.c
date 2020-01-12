///** 
//  * @file     comm_task.c
//  * @version  v2.0
//  * @date     July,6th 2019
//	*
//  * @brief    交互任务文件
//	*
//  *	@author   Fatmouse
//  *
//  */
//#include "comm_task.h"
//#include "bsp_can.h"
//#include "cmsis_os.h"


//motor_current_t motor_cur;
//supercap_message_t supercap_control;


///**
//  * @brief can_msg_send_task
//  * @param 
//  * @attention  
//  * @note  
//  */
//void can_msg_send_task(void const *argu)
//{
//	osEvent event;
//	for(;;)
//  {
//		event = osSignalWait(GIMBAL_MOTOR_MSG_SEND  | \
//		                     CHASSIS_MOTOR_MSG_SEND | \
//		                     SHOOT_CONTROL_MSG_SEND | \
//		                     SUPERCAP_CONTROL_MSG_SEND, osWaitForever);
//		if (event.status == osEventSignal)
//    {

//      if (event.value.signals & GIMBAL_MOTOR_MSG_SEND)
//      {
//				send_gimbal_motor_ctrl_message(motor_cur.gimbal_cur);
//      }
//			if (event.value.signals & CHASSIS_MOTOR_MSG_SEND)
//      {
//				send_chassis_motor_ctrl_message(motor_cur.chassis_cur);
//			}
//			if (event.value.signals & SUPERCAP_CONTROL_MSG_SEND)
//      {
////        send_supercap_message(supercap_control);
//			}
//		 
//		}
//	}
//}

///**
//  * @brief send_chassis_motor_ctrl_message
//  * @param 
//  * @attention  
//  * @note  
//  */
//void send_chassis_motor_ctrl_message(int16_t chassis_cur[])
//{
//  send_chassis_cur(chassis_cur[0], chassis_cur[1], 
//                   chassis_cur[2], chassis_cur[3]);
//}

//void send_gimbal_motor_ctrl_message(int16_t gimbal_cur[])
//{
//  /* 0: yaw motor current
//     1: pitch motor current
//     2: trigger motor current*/
//	//
//	
//	//gimbal_cur[1] = -1.0f*gimbal_cur[1];
//	send_gimbal_cur(gimbal_cur[0], gimbal_cur[1], gimbal_cur[2]);

//}

