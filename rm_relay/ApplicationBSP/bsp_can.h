#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include "can.h"



void my_can_filter_init_recv_all(void);
void can_device_init(void);
void send_chassis_cur(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);


#endif

