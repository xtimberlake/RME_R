#include "gas_task.h"
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "tim.h"
#include "view_task.h"

main_data_t main_data;
//air_cylinder_t air_cylinder;
uint8_t i;

void gas_task(void const * argument)
{
//	uint8_t i;
	uint8_t a;
	uint8_t gas_status;
	uint8_t	electrical_status;
  for(;;)
  {
			
		for (i = 0; i < 6; i++)
		{
			gas_status = main_data.gas_status;
			if (gas_status & (1<<i))
			{
				gas_on(i);
			}
			else gas_off(i);
		}
		
		for (a = 0; a < 2; a++)
		{
			electrical_status = main_data.electrical_status;
			if (electrical_status & (1<<a))
			{
				electrical_on(a);
			}
			else electrical_off(a);
		}
	
	  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,view.yaw_ref);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,view.pit_ref);
		osDelay(100);
  }
  
}

/**
  * @brief  ��������
  * @param  bit ������ӦBIT
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
		case BIT_THROW:
			HAL_GPIO_WritePin(AIR_THROW_GPIO_Port,AIR_THROW_Pin,GPIO_PIN_SET);
			break;
		case BIT_BRACKET:
			HAL_GPIO_WritePin(AIR_BRACKET_GPIO_Port,AIR_BRACKET_Pin,GPIO_PIN_SET);
			break;
		case BIT_BULLET1:
			HAL_GPIO_WritePin(AIR_BULLET1_GPIO_Port,AIR_BULLET1_Pin,GPIO_PIN_SET);
			break;
		case BIT_BULLET2:
			HAL_GPIO_WritePin(AIR_BULLET2_GPIO_Port,AIR_BULLET2_Pin,GPIO_PIN_SET);
			break;
//		case BIT_EXB:
//			HAL_GPIO_WritePin(GAS_EXB_GPIO_Port,GAS_EXB_Pin,GPIO_PIN_SET);
//		case BIT_CAMERA:
//			HAL_GPIO_WritePin(CAMERA_GPIO_Port,CAMERA_Pin,GPIO_PIN_SET);
//			break;		
	}
}

/**
  * @brief  �����ر�
  * @param  bit ������ӦBIT
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
		case BIT_THROW:
			HAL_GPIO_WritePin(AIR_THROW_GPIO_Port,AIR_THROW_Pin,GPIO_PIN_RESET);
			break;
		case BIT_BRACKET:
			HAL_GPIO_WritePin(AIR_BRACKET_GPIO_Port,AIR_BRACKET_Pin,GPIO_PIN_RESET);
			break;
		case BIT_BULLET1:
			HAL_GPIO_WritePin(AIR_BULLET1_GPIO_Port,AIR_BULLET1_Pin,GPIO_PIN_RESET);
			break;
		case BIT_BULLET2:
			HAL_GPIO_WritePin(AIR_BULLET2_GPIO_Port,AIR_BULLET2_Pin,GPIO_PIN_RESET);
			break;
//		case BIT_EXB:
//			HAL_GPIO_WritePin(AIR_EXB_GPIO_Port,AIR_EXB_Pin,GPIO_PIN_RESET);
//			break;		
//		case BIT_CAMERA:
//			HAL_GPIO_WritePin(CAMERA_GPIO_Port,CAMERA_Pin,GPIO_PIN_RESET);
//			break;		
	}
}

/**
  * @brief  ���Ƶ����״��ӽ��Լ���ȡ�����������
  * @param  bit 
  * @retval void
  * @note   void
  */
void electrical_on(uint8_t bit)
{
	switch(bit)
	{
		case BIT_CAMERA:
			HAL_GPIO_WritePin(AIR_HELP_GPIO_Port,AIR_HELP_Pin,GPIO_PIN_SET);
			break;
		case BIT_ROTATE:
			HAL_GPIO_WritePin(AIR_PRESS_GPIO_Port,AIR_PRESS_Pin,GPIO_PIN_SET);//ɾ��
			break;
	}
}

void electrical_off(uint8_t bit)
{
	switch(bit)
	{
		case BIT_CAMERA:
			HAL_GPIO_WritePin(AIR_HELP_GPIO_Port,AIR_HELP_Pin,GPIO_PIN_RESET);
			break;
		case BIT_ROTATE:
//			HAL_GPIO_WritePin(AIR_PRESS_GPIO_Port,AIR_PRESS_Pin,GPIO_PIN_RESET);
			break;
	}
}


