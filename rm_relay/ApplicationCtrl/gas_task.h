#ifndef __GAS_TASK
#define __GAS_TASK

#include "stm32f1xx_hal.h"

#define BIT_HELP    	0
#define BIT_PRESS   	1
#define BIT_BRACKET   2
#define BIT_THROW 		3
#define BIT_BULLET1 	4
#define BIT_BULLET2 	5
#define BIT_RESERVE 	0

#define BIT_CAMERA		0
#define BIT_MAGAZINE 	1
#define BIT_SAFE			2

typedef struct
{
	uint8_t	gas_status;
	uint8_t  electrical_status;
} main_data_t;



//typedef struct
//{
//	_Bool HELP;
//	_Bool PRESS;
//	_Bool CLAW;
//	_Bool BRACKET;
//	_Bool ROTATE;
//	_Bool CARD_HEP;
//	_Bool MAGAZINE;
//	_Bool CAMERA;
//	
//} air_cylinder_t;


void gas_task(void const * argument);
void gas_on(uint8_t bit);
void gas_off(uint8_t bit);
void electrical_on(uint8_t bit);
void electrical_off(uint8_t bit);

//extern air_cylinder_t air_cylinder;
extern main_data_t main_data;



#endif
