/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "stm32f4xx_hal.h"
#include "comm_task.h"
#include "chassis_task.h"
#include "modeswitch_task.h"
#include "debug_task.h"
#include "status_task.h"
#include "uplift_task.h"
#include "slip_task.h"
#include "rotate_task.h"
#include "relay_task.h"
#include "judge.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId can_msg_send_task_t;
osThreadId mode_sw_task_t;
osThreadId debug_task_t;
osThreadId status_task_t;
osThreadId relay_task_t;

osTimerId chassis_timer_id;
osTimerId uplift_timer_id;
osTimerId	slip_timer_id;
osTimerId	rotate_timer_id;

osTimerId judge_sendTimer_id;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];
  
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )  
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}                   
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
		taskENTER_CRITICAL();
		
   osTimerDef(chassisTimer, chassis_task);
  chassis_timer_id = osTimerCreate(osTimer(chassisTimer), osTimerPeriodic, NULL);

   osTimerDef(upliftTimer, uplift_task);
  uplift_timer_id = osTimerCreate(osTimer(upliftTimer), osTimerPeriodic, NULL);
	
	 osTimerDef(sliptTimer, slip_task);
  slip_timer_id = osTimerCreate(osTimer(sliptTimer), osTimerPeriodic, NULL);
	
	 osTimerDef(rotateTimer, rotate_task);
  rotate_timer_id = osTimerCreate(osTimer(rotateTimer), osTimerPeriodic, NULL);
	
	 osTimerDef(judge_sendTimer, judge_send_task);
  judge_sendTimer_id = osTimerCreate(osTimer(judge_sendTimer), osTimerPeriodic, NULL);
	
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	/* add threads, ... */
			 /* high priority task */
		
	 osThreadDef(canTask, can_msg_send_task, osPriorityAboveNormal, 0, 512);
  can_msg_send_task_t = osThreadCreate(osThread(canTask), NULL);
	
	 osThreadDef(modeTask, mode_switch_task, osPriorityAboveNormal, 0, 512);
	mode_sw_task_t = osThreadCreate(osThread(modeTask), NULL);

	 	 /* low priority task */
	 osThreadDef(relayTask, relay_task, osPriorityNormal, 0, 128);
  relay_task_t = osThreadCreate(osThread(relayTask), NULL);	 

	 osThreadDef(debugTask, debug_task, osPriorityLow, 0, 256);
	debug_task_t = osThreadCreate(osThread(debugTask), NULL);

	 osThreadDef(statusTask, status_task, osPriorityLow, 0, 512);
	status_task_t = osThreadCreate(osThread(statusTask), NULL);

	 	taskEXIT_CRITICAL();
	
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
    
    

  /* USER CODE BEGIN StartDefaultTask */
	
	//开启底盘和抬升机构的软件定时器任务

	osTimerStart(chassis_timer_id, CHASSIS_PERIOD);
	osTimerStart(uplift_timer_id, UPLIFT_PERIOD);
	osTimerStart(slip_timer_id, SLIP_PERIOD);
	osTimerStart(rotate_timer_id,ROTATE_PERIOD);
	osTimerStart(judge_sendTimer_id, JUDGE_SEND_PERIOD);
	
  /* Infinite loop */
	 while(1)
  {
		HAL_GPIO_TogglePin(LED_A_GPIO_Port,LED_A_Pin);
		HAL_GPIO_TogglePin(LED_C_GPIO_Port,LED_C_Pin);
    osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
