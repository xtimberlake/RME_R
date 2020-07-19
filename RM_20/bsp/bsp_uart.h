#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "usart.h"

#define 	DBUS_HUART    huart1
#define		IMU_HUART			huart2
#define   JUDGE_HUART   huart3
#define 	VISION_HUART	huart5		
#define   RELAY_HUARE		huart6

#define  DMA_DBUS_LEN			18		//串口数据缓存大小
#define  DMA_IMU_LEN			50
#define  DMA_JUDGE_LEN		200
#define  DMA_VISION_LEN		100
#define  DMA_RELAY_LEN		150

//union rec_float			//接收float信息
//{
//	float    msg_float;
//	uint8_t  msg_uint8_t[4];
//};

//union rec_uint16_t
//{
//	uint16_t msg_uint16_t;
//	uint8_t  msg_uint8_t[2];
//};

void uart_init(void);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);

extern uint8_t dma_dbus_buf[DMA_DBUS_LEN];
#endif

