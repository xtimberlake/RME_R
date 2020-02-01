#ifndef __KEYBOARD_HANDLE
#define __KEYBOARD_HANDLE

#include "stm32f4xx_hal.h"

#define SPEED_NORMAL	2800
#define SPEED_SLOW		800
#define	SPEED_FAST		6000


extern float chassis_speed;  //ȷ�����������ٶȣ���modeswitch�ж���


void keyboard_handle(void);
void keyboard_chassis_ctrl(void);
void keyboard_uplift_ctrl(void);


#endif
