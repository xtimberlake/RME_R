#include "gas_task.h"
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "tim.h"
#include "view_task.h"
#include "rotate_task.h"

main_data_t main_data;
main_data_t last_main_data;
//air_cylinder_t air_cylinder;

	uint8_t i;
	uint8_t a;
	
void gas_task(void const * argument)
{
//	uint8_t i;
//	uint8_t a;
	uint8_t gas_status;
	uint8_t	electrical_status;
  for(;;)
  {
			for (i = 0; i < 7; i++)
			{
				
				gas_status = main_data.gas_status;
				if (gas_status & (1<<i))
				{
					gas_on(i);
				}
				else gas_off(i);
				osDelay(20);
			}
			
			for (a = 0; a < 3; a++)
			{
				electrical_status = main_data.electrical_status;
				if (electrical_status & (1<<a))
				{
					electrical_on(a);
				}
				else electrical_off(a);
				osDelay(20);
			}
			
	
	  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,view.yaw_ref);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,view.pit_ref);
		osDelay(100);
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
		case BIT_BRACKET:
			HAL_GPIO_WritePin(AIR_BRACKET_GPIO_Port,AIR_BRACKET_Pin,GPIO_PIN_SET);
			break;
		case BIT_THROW:
			HAL_GPIO_WritePin(AIR_THROW_GPIO_Port,AIR_THROW_Pin,GPIO_PIN_SET);
			break;
		case BIT_BULLET1:
			HAL_GPIO_WritePin(AIR_BULLET1_GPIO_Port,AIR_BULLET1_Pin,GPIO_PIN_SET);
			break;
		case BIT_BULLET2:
			HAL_GPIO_WritePin(AIR_BULLET2_GPIO_Port,AIR_BULLET2_Pin,GPIO_PIN_SET);
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
		case BIT_BRACKET:
			HAL_GPIO_WritePin(AIR_BRACKET_GPIO_Port,AIR_BRACKET_Pin,GPIO_PIN_RESET);
			break;
		case BIT_THROW:
			HAL_GPIO_WritePin(AIR_THROW_GPIO_Port,AIR_THROW_Pin,GPIO_PIN_RESET);
			break;
		case BIT_BULLET1:
			HAL_GPIO_WritePin(AIR_BULLET1_GPIO_Port,AIR_BULLET1_Pin,GPIO_PIN_RESET);
			break;
		case BIT_BULLET2:
			HAL_GPIO_WritePin(AIR_BULLET2_GPIO_Port,AIR_BULLET2_Pin,GPIO_PIN_RESET);
			break;
	}
}

/**
  * @brief  控制倒车雷达视角以及夹取电机旋出旋入
  * @param  bit 
  * @retval void
  * @note   void
  */
void electrical_on(uint8_t bit)
{
	switch(bit)
	{
		case BIT_CAMERA:
//			HAL_GPIO_WritePin(AIR_HELP_GPIO_Port,AIR_HELP_Pin,GPIO_PIN_SET);
			break;
		case BIT_MAGAZINE:
		{
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,2500);	
		}break;
		case BIT_SAFE:
		{
			rotate.ctrl_mode =ROTATE_STOP;
		}break;
	}
}

void electrical_off(uint8_t bit)
{
	switch(bit)
	{
		case BIT_CAMERA:
//			HAL_GPIO_WritePin(AIR_HELP_GPIO_Port,AIR_HELP_Pin,GPIO_PIN_RESET);
			break;
		case BIT_MAGAZINE:
		{
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,1200);			
		}break;
		case BIT_SAFE:
		{

		}break;		
	}
}


