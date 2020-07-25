#include "view_task.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "bsp_can.h"
#include "bsp_TOF.h"


view_t view;

void view_task(void const * argument)
{
	for(;;)
	{

		osDelay(100);	
	}
}
