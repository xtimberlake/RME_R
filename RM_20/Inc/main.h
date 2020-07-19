/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DOWN_LIMIT1_Pin GPIO_PIN_1
#define DOWN_LIMIT1_GPIO_Port GPIOA
#define BLUE_TX_Pin GPIO_PIN_2
#define BLUE_TX_GPIO_Port GPIOA
#define BLUE_RX_Pin GPIO_PIN_3
#define BLUE_RX_GPIO_Port GPIOA
#define BEEP_PWN_Pin GPIO_PIN_6
#define BEEP_PWN_GPIO_Port GPIOA
#define TURBINE_PWM1_Pin GPIO_PIN_0
#define TURBINE_PWM1_GPIO_Port GPIOB
#define TURBINE_JDQ_Pin GPIO_PIN_1
#define TURBINE_JDQ_GPIO_Port GPIOB
#define INDUCTIVE_LEFT_Pin GPIO_PIN_13
#define INDUCTIVE_LEFT_GPIO_Port GPIOE
#define INDUCTIVE_RIGHT_Pin GPIO_PIN_14
#define INDUCTIVE_RIGHT_GPIO_Port GPIOE
#define LED_D_Pin GPIO_PIN_8
#define LED_D_GPIO_Port GPIOD
#define LED_C_Pin GPIO_PIN_9
#define LED_C_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_10
#define LED_B_GPIO_Port GPIOD
#define LED_A_Pin GPIO_PIN_11
#define LED_A_GPIO_Port GPIOD
#define DBUS_TX_Pin GPIO_PIN_9
#define DBUS_TX_GPIO_Port GPIOA
#define DBUS_RX_Pin GPIO_PIN_10
#define DBUS_RX_GPIO_Port GPIOA
#define DOWN_LIMIT2_Pin GPIO_PIN_6
#define DOWN_LIMIT2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
