#include "bsp_uart.h"
#include "remote_msg.h"
#include "string.h"
#include "judge.h"
#include "bsp_JY901.h"
#include "relay_task.h"

uint8_t dma_dbus_buf[DMA_DBUS_LEN];	
uint8_t dma_imu_buf[DMA_IMU_LEN];	
uint8_t	dma_judge_buf[DMA_JUDGE_LEN];
uint8_t dma_vision_buf[DMA_VISION_LEN];	
uint8_t dma_relay_buf[DMA_RELAY_LEN];	

/**
  * @brief ���ڿ����жϻص�����
  * @param UART_HandleTypeDef *huart
  * @retval ��
	* @attention DBUS����������һ������  it.c����Ҫ��cube�Զ����ɵ��жϺ���ע�͵�������ֻ�ܽ��յ�һ���ֽڡ�ԭ���д��о���
  */
void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance== USART1)			//DBUS����  
	{
		rc_callback_handler(&rc,dma_dbus_buf);
		HAL_UART_Receive_DMA(huart, dma_dbus_buf, DMA_DBUS_LEN);
	}
	else if(huart->Instance== USART2)	//�����Ǵ���
	{
//		imu_decoding(dma_imu_buf,IMU_BUFLEN);
		HAL_UART_Receive_DMA(huart, dma_imu_buf, DMA_IMU_LEN);
	}	
	else if(huart->Instance== USART3)	//JUDGE����
	{
		Judge_Read_Data(dma_judge_buf);
		HAL_UART_Receive_DMA(huart, dma_judge_buf, DMA_JUDGE_LEN);
	}
	else if(huart->Instance== UART5)	//�Ӿ�����
	{
		HAL_UART_Receive_DMA(huart, dma_vision_buf, DMA_VISION_LEN);
	}
	else if(huart->Instance== USART6)	//�����崮��
	{
		HAL_UART_Receive_DMA(huart, dma_relay_buf, DMA_RELAY_LEN);
	}
}

/**
  * @brief ���ڿ����ж�   ע������it.c��ÿ�����ڵ��ж��е��øú���
  * @param UART_HandleTypeDef *huart
  * @retval ��
  */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if(RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) //�ж��Ƿ��ǿ����ж�
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
		HAL_UART_DMAStop(huart);															//ֹͣ����DMA����
		USER_UART_IDLECallback(huart);                        //���ô��ڹ��ܻص�����
	}
}


/**
* @brief ���ڳ�ʼ��:ʹ�ܴ��ڿ����ж�,��������DMA����
* @param  ��
* @retval ��
*/
void uart_init()
{
	__HAL_UART_CLEAR_IDLEFLAG(&DBUS_HUART);
	__HAL_UART_ENABLE_IT(&DBUS_HUART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&DBUS_HUART, dma_dbus_buf, DMA_DBUS_LEN);
	
	__HAL_UART_CLEAR_IDLEFLAG(&IMU_HUART);
	__HAL_UART_ENABLE_IT(&IMU_HUART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&IMU_HUART, dma_imu_buf, DMA_IMU_LEN);
	
	__HAL_UART_CLEAR_IDLEFLAG(&JUDGE_HUART);
	__HAL_UART_ENABLE_IT(&JUDGE_HUART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&JUDGE_HUART, dma_judge_buf, DMA_JUDGE_LEN);
	
	__HAL_UART_CLEAR_IDLEFLAG(&VISION_HUART);
	__HAL_UART_ENABLE_IT(&VISION_HUART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&VISION_HUART, dma_vision_buf, DMA_VISION_LEN);
	
	__HAL_UART_CLEAR_IDLEFLAG(&RELAY_HUARE);
	__HAL_UART_ENABLE_IT(&RELAY_HUARE, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&RELAY_HUARE, dma_relay_buf, DMA_RELAY_LEN);	
}

/**
* @brief ���������ݽ���
* @param  ��
* @retval ��
*/
void relay_decoding(void)
{
	relay.dis1 = dma_relay_buf[0]<<8|dma_relay_buf[1];
	relay.dis2 = dma_relay_buf[2]<<8|dma_relay_buf[3];
	
	if(dma_relay_buf[4]==0x00)	{relay.bullet_left = 0;relay.bullet_right = 0;}
	else if(dma_relay_buf[4]==0x40){relay.bullet_left = 0;relay.bullet_right = 1;}
	else if(dma_relay_buf[4]==0x80){relay.bullet_left = 1;relay.bullet_right = 0;}
	else if(dma_relay_buf[4]==0xC0){relay.bullet_left = 1;relay.bullet_right = 1;}
	
//	relay.bullet_left  = relay_buf[4]&0x80>>7;
//	relay.bullet_right = relay_buf[4]&0x40>>6;
}
