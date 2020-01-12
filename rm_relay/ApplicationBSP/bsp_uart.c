/** 
  * @file     bsp_uart.c
  * @version  v2.0
  * @date     2019.11.18
	*
  * @brief    ��������
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
* @brief ʹ�ܴ��ڿ����ж�,��������DMA����
* @param  ��
* @retval ��
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
  * @brief ���ڿ����ж�
  * @param UART_HandleTypeDef *huart
  * @retval ��
  */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if(RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
		USAR_UART_IDLECallback(huart);                        //�����жϴ�����
	}
}


 /**
  * @brief ���ڿ����жϻص�����
  * @param ���ھ��
  * @retval ��
  */
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	HAL_UART_DMAStop(huart);                                                    //ֹͣ����DMA����
	USER_UART_FUNCTION(huart);																									//�������ڹ��ܺ���
}


 /**
  * @brief �������ڹ��ܺ������йش��ں�������������д��
  * @param 	UART_HandleTypeDef *huart
  * @retval ��
  */
void USER_UART_FUNCTION(UART_HandleTypeDef *huart)
{		
	if(huart->Instance== USART1 )
	{
		MAIN_get_data(DMA.receive_buff1);																									//MAIN���ݽ���	
		memset(DMA.receive_buff1,0,DMA_recive_buff_LEN);                                  //������ջ�����
		HAL_UART_Receive_DMA(huart, (uint8_t*)DMA.receive_buff1, DMA_recive_buff_LEN);    //������ʼDMA����
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
		TOF[1]=TOF_GetData(DMA.receive_buff3);																						 //��ǰС�������		
		memset(DMA.receive_buff3,0,DMA_recive_buff_LEN);                                     
		HAL_UART_Receive_DMA(huart, (uint8_t*)DMA.receive_buff3, DMA_recive_buff_LEN);     
	}
	
}

 /**
  * @brief ��ȡ���ؿ��ư����Ϣ
  * @param 	uint8_t *receive_buff
  * @retval void
  */

void MAIN_get_data(uint8_t *receive_buff)
{
	main_data.gas_status=receive_buff[0];
	main_data.electrical_status=receive_buff[1];
}



		
		

