#include "gas_task.h"
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "cmsis_os.h"

main_data_t main_data;
air_cylinder_t air_cylinder;

void gas_task(void const * argument)
{
	uint8_t i;
	uint8_t gas_status;
  for(;;)
  {
			
	for (i = 0; i < 8; i++)
	{
		if (gas_status & (1<<i))
		{
			gas_on(i);
		}
		else gas_off(i);
	}
		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    osDelay(500);
  }
  
}

/**
  * @brief  气阀开启
  * @param  bit 气阀对应BIT
  * @retval void
  * @note   void
  */
void gas_on(uint8_t bit)
{
	switch(bit)
	{
		case BIT_HELP:
			HAL_GPIO_WritePin(AIR_HELP_GPIO_Port,AIR_HELP_Pin,GPIO_PIN_SET);
			break;
		case BIT_PRESS:
			HAL_GPIO_WritePin(AIR_PRESS_GPIO_Port,AIR_PRESS_Pin,GPIO_PIN_SET);
			break;
		case BIT_CLAW:
			HAL_GPIO_WritePin(AIR_CLAW_GPIO_Port,AIR_CLAW_Pin,GPIO_PIN_SET);
			break;
		case BIT_BRACKET:
			HAL_GPIO_WritePin(AIR_BRACKET_GPIO_Port,AIR_BRACKET_Pin,GPIO_PIN_SET);
			break;
		case BIT_ROTATE:
			HAL_GPIO_WritePin(AIR_ROTATE_GPIO_Port,AIR_ROTATE_Pin,GPIO_PIN_SET);
			break;
		case BIT_EXA:
			HAL_GPIO_WritePin(AIR_MAGAZINE_GPIO_Port,AIR_MAGAZINE_Pin,GPIO_PIN_SET);
			break;
//		case BIT_EXB:
//			HAL_GPIO_WritePin(GAS_EXB_GPIO_Port,GAS_EXB_Pin,GPIO_PIN_SET);
		case BIT_CAMERA:
			HAL_GPIO_WritePin(CAMERA_GPIO_Port,CAMERA_Pin,GPIO_PIN_SET);
			break;		
	}
}

/**
  * @brief  气阀关闭
  * @param  bit 气阀对应BIT
  * @retval void
  * @note   void
  */
void gas_off(uint8_t bit)
{
	switch(bit)
	{
		case BIT_HELP:
			HAL_GPIO_WritePin(AIR_HELP_GPIO_Port,AIR_HELP_Pin,GPIO_PIN_RESET);
			break;
		case BIT_PRESS:
			HAL_GPIO_WritePin(AIR_PRESS_GPIO_Port,AIR_PRESS_Pin,GPIO_PIN_RESET);
			break;
		case BIT_CLAW:
			HAL_GPIO_WritePin(AIR_CLAW_GPIO_Port,AIR_CLAW_Pin,GPIO_PIN_RESET);
			break;
		case BIT_BRACKET:
			HAL_GPIO_WritePin(AIR_BRACKET_GPIO_Port,AIR_BRACKET_Pin,GPIO_PIN_RESET);
			break;
		case BIT_ROTATE:
			HAL_GPIO_WritePin(AIR_ROTATE_GPIO_Port,AIR_ROTATE_Pin,GPIO_PIN_RESET);
			break;
		case BIT_EXA:
			HAL_GPIO_WritePin(AIR_MAGAZINE_GPIO_Port,AIR_MAGAZINE_Pin,GPIO_PIN_RESET);
			break;
//		case BIT_EXB:
//			HAL_GPIO_WritePin(AIR_EXB_GPIO_Port,AIR_EXB_Pin,GPIO_PIN_RESET);
//			break;		
		case BIT_CAMERA:
			HAL_GPIO_WritePin(CAMERA_GPIO_Port,CAMERA_Pin,GPIO_PIN_RESET);
			break;		
	}
}


