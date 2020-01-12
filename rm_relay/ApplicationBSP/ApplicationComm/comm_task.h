///** 
//  * @file     comm_task.c
//  * @version  v2.0
//  * @date     July,6th 2019
//	*
//  * @brief    交互任务结构体定义
//	*
//  *	@author   Fatmouse
//  *
//  */
//#ifndef __COMM_TASK_H__
//#define __COMM_TASK_H__

//#include "stm32f1xx_hal.h"
//#define SHOOT_CONTROL_MSG_SEND    ( 1 << 2 )
//#define SUPERCAP_CONTROL_MSG_SEND ( 1 << 3 )
//#define RISE_FINISH_SIGNAL        ( 1 << 4 )
//#define DETELE_RISE_TASK			    ( 1 << 5 )

//#define GIMBAL_MOTOR_MSG_SEND     ( 1 << 6 )
//#define CHASSIS_MOTOR_MSG_SEND    ( 1 << 7 )

//#define SHOT_TASK_EXE_SIGNAL      ( 1 << 8 )
//#define INFO_GET_EXE_SIGNAL       ( 1 << 9 )
///* motor current parameter structure */
//typedef struct
//{
//  /* 4 chassis motor current */
//  int16_t chassis_cur[4];
//  /* yaw/pitch/trigger motor current */
//  int16_t gimbal_cur[3];
//} motor_current_t;



//typedef struct
//{
//  uint8_t supercap_mode;
//  uint8_t supercap_switch;
//  uint8_t data_2;
//  uint8_t data_3;
//  uint8_t data_4;
//	uint8_t data_5;
//	uint8_t data_6;
//  uint8_t data_7;
//} supercap_message_t;

//void can_msg_send_task(void const *argu);
//void send_chassis_motor_ctrl_message(int16_t chassis_cur[]);
//void send_gimbal_motor_ctrl_message(int16_t gimbal_cur[]);
//extern motor_current_t motor_cur;
//extern supercap_message_t supercap_control;
//#endif
