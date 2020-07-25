#include "relay_commun_task.h"
#include "usart.h"
#include "bsp_TOF.h"
#include "cmsis_os.h"

void relay_commun_task(void const * argument)
{
	for(;;)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)TOF,sizeof(TOF),0xff);
		osDelay(100);	
	}
}
