#ifndef  __bsp_TOF_H
#define  __bsp_TOF_H

#include "stdint.h"


extern uint16_t TOF[2];

uint16_t TOF_GetData(uint8_t *receive_buff);

#endif
