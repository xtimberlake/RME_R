#ifndef __GAS_TASK
#define __GAS_TASK

#include "stm32f1xx_hal.h"

#define BIT_HELP    0
#define BIT_PRESS   1
#define BIT_CLAW    2
#define BIT_BRACKET 3
#define BIT_ROTATE  4
#define BIT_EXA     5
#define BIT_EXB     6
#define BIT_CAMERA  7

typedef struct
{
	uint8_t	gas_status;
	uint8_t  electrical_status;
} main_data_t;


typedef struct
{
	_Bool HELP;
	_Bool PRESS;
	_Bool CLAW;
	_Bool BRACKET;
	_Bool ROTATE;
	_Bool CARD_HEP;
	_Bool MAGAZINE;
	_Bool CAMERA;
	
} air_cylinder_t;


void gas_task(void const * argument);
void gas_on(uint8_t bit);
void gas_off(uint8_t bit);

extern air_cylinder_t air_cylinder;
extern main_data_t main_data;



#endif
