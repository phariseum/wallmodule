/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : dimmer.h
  * @brief          : Header for dimmer.c file.
  *                   This file contains all defines for dimmer.h.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DIMMER_H
#define __DIMMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include <stdbool.h>

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void DIM_SCR_A_Activate(void);
void DIM_SCR_A_Deactivate(void);
void DIM_SCR_B_Activate(void);
void DIM_SCR_B_Deactivate(void);
void DIM_IGBT_Trigger(void);
void DIM_IGBT_Release(void);
void DIM_SCR_Enable(void);
void DIM_SCR_TurnOff(void);
void DIM_TIM1_Run(bool preloadAndRun);
void DIM_TIM2_Run(bool hasToRun);
bool DIM_ShortCircuit_IsDetected(void);
void DIM_EnterSafeMode(void);
void DIM_IndicateFault(void);
void DIM_EnableACDetection(void);
void DIM_DisableACDetection(void);
void DIM_LoadLUTs(int mainsFreq);
void DIM_LoadDefaultAutoReloads(int mainsFreq);
void DIM_ProcessDimmingInput(void);
void DIM_Init(void);
void DIM_Loop(void);
void DIM_Process(void);

/* Private defines -----------------------------------------------------------*/
#define POSITIVE_HALF_CYCLE	0
#define NEGATIVE_HALF_CYCLE	1

#define AC_PERIOD_THLD_ILLEGAL	3262
#define AC_PERIOD_THLD_50	5413
#define AC_PERIOD_THLD_60	4375

#define AC_50HZ_ON_AUTORELOAD_VALUE	2344		// 1_alt
#define AC_60HZ_ON_AUTORELOAD_VALUE	1927
#define AC_50HZ_OFF_AUTORELOAD_VALUE	125		// 2
#define AC_60HZ_OFF_AUTORELOAD_VALUE	5
#define AC_50HZ_INITIAL_AUTORELOAD_VALUE	2000	// 1
#define AC_60HZ_INITIAL_AUTORELOAD_VALUE	1675

#define DIMMER_INITIAL_ANGLE	8

#ifdef __cplusplus
}
#endif

#endif /* __DIMMER_H */
