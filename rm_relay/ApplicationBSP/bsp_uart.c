/** 
  * @file     bsp_uart.c
  * @version  v2.0
  * @date     2019.11.18
	*
  * @brief    串口驱动
	*
  *	@author   
  *
  */
#include "bsp_uart.h"
#include "gas_task.h"
#include "bsp_TOF.h"

dma_rec_t DMA;
extern DMA_HandleTypeDef hdma_usart2_rx;
uint8_t data_length;


/**
* @brief 使能串口空闲中断,开启串口DMA接收
* @param  无
* @retval 无
*/
void user_uart_init()
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	
	
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)DMA.receive_buff1, DMA_recive_buff_LEN);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)DMA.receive_buff2, DMA_recive_buff_LEN);
	HAL_UART_Receive_DMA(&huart3, (uint8_t*)DMA.receive_buff3, DMA_recive_buff_LEN);

}


 /**
  * @brief 串口空闲中断
  * @param UART_HandleTypeDef *huart
  * @retval 无
  */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if(RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))   //判断是否是空闲中断
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);                     //清除空闲中断标志（否则会一直不断进入中断）
		USAR_UART_IDLECallback(huart);                        //调用中断处理函数
	}
}


 /**
  * @brief 串口空闲中断回调函数
  * @param 串口句柄
  * @retval 无
  */
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	HAL_UART_DMAStop(huart);                                                    //停止本次DMA传输
	USER_UART_FUNCTION(huart);																									//各个串口功能函数
}


 /**
  * @brief 各个串口功能函数（有关串口函数处理于其书写）
  * @param 	UART_HandleTypeDef *huart
  * @retval 无
  */
void USER_UART_FUNCTION(UART_HandleTypeDef *huart)
{		
	if(huart->Instance== USART1 )
	{
		MAIN_get_data(DMA.receive_buff1);																									//MAIN数据解算	
		memset(DMA.receive_buff1,0,DMA_recive_buff_LEN);                                  //清零接收缓冲区
		HAL_UART_Receive_DMA(huart, (uint8_t*)DMA.receive_buff1, DMA_recive_buff_LEN);    //重启开始DMA传输
	}
	else if(huart->Instance== USART2)
	{
		TOF[0]=TOF_GetData(DMA.receive_buff2);
		data_length  = 50 - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		memset(DMA.receive_buff2,0,DMA_recive_buff_LEN);                                     
		HAL_UART_Receive_DMA(huart, (uint8_t*)DMA.receive_buff2, DMA_recive_buff_LEN);        
	}
	else if(huart->Instance== USART3)
	{
		TOF[1]=TOF_GetData(DMA.receive_buff3);																						 //左前小激光解算		
		memset(DMA.receive_buff3,0,DMA_recive_buff_LEN);                                     
		HAL_UART_Receive_DMA(huart, (uint8_t*)DMA.receive_buff3, DMA_recive_buff_LEN);     
	}
	
}

 /**
  * @brief 获取主控控制板的信息
  * @param 	uint8_t *receive_buff
  * @retval void
  */

void MAIN_get_data(uint8_t *receive_buff)
{
	main_data.gas_status=receive_buff[0];
	main_data.electrical_status=receive_buff[1];
}



		
		

