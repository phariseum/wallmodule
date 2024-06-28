/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f0xx_hal.h"

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
#define PLC_RESETN_Pin GPIO_PIN_15
#define PLC_RESETN_GPIO_Port GPIOC
#define PLC_T_REQ_Pin GPIO_PIN_1
#define PLC_T_REQ_GPIO_Port GPIOA
#define PLC_RXD_Pin GPIO_PIN_2
#define PLC_RXD_GPIO_Port GPIOA
#define PLC_TXD_Pin GPIO_PIN_3
#define PLC_TXD_GPIO_Port GPIOA
#define METER_NCS_Pin GPIO_PIN_4
#define METER_NCS_GPIO_Port GPIOA
#define METER_SCK_Pin GPIO_PIN_5
#define METER_SCK_GPIO_Port GPIOA
#define METER_MISO_Pin GPIO_PIN_6
#define METER_MISO_GPIO_Port GPIOA
#define METER_MOSI_Pin GPIO_PIN_7
#define METER_MOSI_GPIO_Port GPIOA
#define DIM_FAULT_Pin GPIO_PIN_1
#define DIM_FAULT_GPIO_Port GPIOB
#define SCR_DISABLE_Pin GPIO_PIN_2
#define SCR_DISABLE_GPIO_Port GPIOB
#define RELAY_COIL_ON_Pin GPIO_PIN_14
#define RELAY_COIL_ON_GPIO_Port GPIOB
#define RELAY_COIL_OFF_Pin GPIO_PIN_15
#define RELAY_COIL_OFF_GPIO_Port GPIOB
#define CLOCK_OUT_Pin GPIO_PIN_8
#define CLOCK_OUT_GPIO_Port GPIOA
#define METER_RST_Pin GPIO_PIN_10
#define METER_RST_GPIO_Port GPIOA
#define METER_INT_Pin GPIO_PIN_11
#define METER_INT_GPIO_Port GPIOA
#define LED_STATUS_Pin GPIO_PIN_15
#define LED_STATUS_GPIO_Port GPIOA
#define ZC_DETECT_Pin GPIO_PIN_3
#define ZC_DETECT_GPIO_Port GPIOB
#define ZC_DETECT_EXTI_IRQn EXTI2_3_IRQn
#define SCR_A_TRIG_Pin GPIO_PIN_4
#define SCR_A_TRIG_GPIO_Port GPIOB
#define SCR_B_TRIG_Pin GPIO_PIN_5
#define SCR_B_TRIG_GPIO_Port GPIOB
#define IGBT_TRIG_Pin GPIO_PIN_6
#define IGBT_TRIG_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
