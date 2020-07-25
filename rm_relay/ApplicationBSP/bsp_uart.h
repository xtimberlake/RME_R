#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "string.h"
#include "stdlib.h"
#include "usart.h"



#define DMA_recive_buff_LEN 50    //串口DMA接收缓冲区大小

typedef struct
{
	uint8_t	 receive_buff1[DMA_recive_buff_LEN];
	uint8_t  receive_buff2[DMA_recive_buff_LEN];
	uint8_t  receive_buff3[DMA_recive_buff_LEN];
	
} dma_rec_t;

extern dma_rec_t DMA;

void user_uart_init(void);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart);
void USER_UART_FUNCTION(UART_HandleTypeDef *huart); 
void MAIN_get_data(uint8_t *receive_buff);

#endif



