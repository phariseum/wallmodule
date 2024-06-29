/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : dimmer.c
  * @brief          : Software controlled light dimmer module
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dimmer.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
int zcRisingEdgeCounter = 0;
int acLineStateTim1 = POSITIVE_HALF_CYCLE;
int acLineStateTim2 = 0;
unsigned char avgCycleCount = 0;
unsigned char dimmerAngle = 0;
unsigned char mainsFreq = 0;
bool risingEdgeDetectionFlag = false;
bool shortCircuitDetectionFlag = false;
unsigned char shortCircuitRetryCount = 0;
bool setDefaultValueForTim2 = false;
bool dimmerIsRunning = false;
bool detectionPhaseInProgress = true;
unsigned char direction = 1;

unsigned char currentDimmingTableDescHi[22];
unsigned char currentDimmingTableDescLo[22];
unsigned char currentDimmingTableAscHi[22];
unsigned char currentDimmingTableAscLo[22];
int currentOnAutoreloadValue;
int currentOffAutoreloadValue;
int currentInitialAutoreloadValue;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

// Lookup table arrays for dimming curves and different AC frequencies
const unsigned char LUT_50_LIN_DESC_HI[] = {
    0x07, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x04, 0x04,
    0x04, 0x04, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x01,
    0x01, 0x01
};

const unsigned char LUT_50_LIN_DESC_LO[] = {
    0x21, 0xd9, 0x92, 0x4b, 0x04, 0xbc, 0x75, 0x2e, 0xe7, 0x9f,
    0x58, 0x11, 0xca, 0x82, 0x3b, 0xf4, 0xad, 0x65, 0x1e, 0xd7,
    0x90, 0x48
};

const unsigned char LUT_50_LIN_ASC_HI[] = {
    0x01, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04,
    0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07
};

const unsigned char LUT_50_LIN_ASC_LO[] = {
    0xa9, 0xf0, 0x37, 0x7e, 0xc6, 0x0d, 0x54, 0x9b, 0xe3, 0x2a,
    0x71, 0xb8, 0x00, 0x47, 0x8e, 0xd5, 0x1d, 0x64, 0xab, 0xf2,
    0x3a, 0x81
};

const unsigned char LUT_60_LIN_DESC_HI[] = {
    0x06, 0x05, 0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x03,
    0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
    0x01, 0x00
};

const unsigned char LUT_60_LIN_DESC_LO[] = {
    0x0e, 0xd0, 0x92, 0x54, 0x17, 0xd9, 0x9b, 0x5d, 0x20, 0xe2,
    0xa4, 0x66, 0x29, 0xeb, 0xad, 0x6f, 0x32, 0xf4, 0xb6, 0x78,
    0x3b, 0xfd
};

const unsigned char LUT_60_LIN_ASC_HI[] = {
    0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03,
	0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06
};

const unsigned char LUT_60_LIN_ASC_LO[] = {
    0x43, 0x80, 0xbe, 0xfc, 0x3a, 0x77, 0xb5, 0xf3, 0x31, 0x6e,
    0xac, 0xea, 0x28, 0x65, 0xa3, 0xe1, 0x1f, 0x5c, 0x9a, 0xd8,
    0x16, 0x53
};

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
void DIM_SCR_A_Activate()
{
	/* Sets SCR A to high */
	HAL_GPIO_WritePin(SCR_A_TRIG_GPIO_Port, SCR_A_TRIG_Pin, GPIO_PIN_SET);
}

void DIM_SCR_A_Deactivate()
{
	/* Sets SCR A to zero */
	HAL_GPIO_WritePin(SCR_A_TRIG_GPIO_Port, SCR_A_TRIG_Pin, GPIO_PIN_RESET);
}

void DIM_SCR_B_Activate()
{
	/* Sets SCR B to high */
	HAL_GPIO_WritePin(SCR_B_TRIG_GPIO_Port, SCR_B_TRIG_Pin, GPIO_PIN_SET);
}

void DIM_SCR_B_Deactivate()
{
	/* Sets SCR B to zero */
	HAL_GPIO_WritePin(SCR_B_TRIG_GPIO_Port, SCR_B_TRIG_Pin, GPIO_PIN_RESET);
}

void DIM_IGBT_Trigger()
{
	/* Forces a pulse on IGBT to ensure activation */
	HAL_GPIO_WritePin(IGBT_TRIG_GPIO_Port, IGBT_TRIG_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IGBT_TRIG_GPIO_Port, IGBT_TRIG_Pin, GPIO_PIN_SET);
}

void DIM_IGBT_Release()
{
	/* Sets IGBT to zero */
	HAL_GPIO_WritePin(IGBT_TRIG_GPIO_Port, IGBT_TRIG_Pin, GPIO_PIN_RESET);
}

void DIM_SCR_TurnOff()
{
	/* Enables the activation of SCR A and SCR B */
	HAL_GPIO_WritePin(SCR_DISABLE_GPIO_Port, SCR_DISABLE_Pin, GPIO_PIN_SET);
}

void DIM_SCR_Enable()
{
	/* Turns off SCR A and SCR B */
	HAL_GPIO_WritePin(SCR_DISABLE_GPIO_Port, SCR_DISABLE_Pin, GPIO_PIN_RESET);
}

bool DIM_ShortCircuit_IsDetected()
{
	/* When ZC is in negative half cycle and there is short circuit condition (active LOW) */
	GPIO_PinState zcState = HAL_GPIO_ReadPin(ZC_DETECT_GPIO_Port, ZC_DETECT_Pin);
	GPIO_PinState shortCircuitState = HAL_GPIO_ReadPin(DIM_FAULT_GPIO_Port, DIM_FAULT_Pin);

	return (zcState == GPIO_PIN_RESET && shortCircuitState == GPIO_PIN_RESET);
}

void DIM_IndicateFault()
{
	HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
}

void DIM_EnterSafeMode()
{
	shortCircuitDetectionFlag = true;

	DIM_IGBT_Release();
	DIM_SCR_A_Deactivate();
	DIM_SCR_B_Deactivate();

	DIM_TIM2_Run(false);
	DIM_TIM1_Run(false);

	if (shortCircuitRetryCount == 4)
	{
		DIM_IndicateFault();

		/* Infinite loop or reset dimmer.*/
	}

	shortCircuitRetryCount++;
}

void DIM_EnableACDetection()
{
	/* Dimmer functionality is enabled by adding the interrupt on the AC line */
	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
}

void DIM_DisableACDetection()
{
	/* Dimmer functionality is disabled by removing the interrupt on the AC line */
	HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
}

void DIM_LoadLUTs(int mainsFreq)
{
	if (mainsFreq == 50)
	{
		memcpy(currentDimmingTableDescHi, LUT_50_LIN_DESC_HI, sizeof(LUT_50_LIN_DESC_HI));
		memcpy(currentDimmingTableDescLo, LUT_50_LIN_DESC_LO, sizeof(LUT_50_LIN_DESC_LO));
		memcpy(currentDimmingTableAscHi, LUT_50_LIN_ASC_HI, sizeof(LUT_50_LIN_ASC_HI));
		memcpy(currentDimmingTableAscLo, LUT_50_LIN_ASC_LO, sizeof(LUT_50_LIN_ASC_LO));
	}
	else if(mainsFreq == 60)
	{
		memcpy(currentDimmingTableDescHi, LUT_60_LIN_DESC_HI, sizeof(LUT_60_LIN_DESC_HI));
		memcpy(currentDimmingTableDescLo, LUT_60_LIN_DESC_LO, sizeof(LUT_60_LIN_DESC_LO));
		memcpy(currentDimmingTableAscHi, LUT_60_LIN_ASC_HI, sizeof(LUT_60_LIN_ASC_HI));
		memcpy(currentDimmingTableAscLo, LUT_60_LIN_ASC_LO, sizeof(LUT_60_LIN_ASC_LO));
	}
}

void DIM_LoadDefaultAutoReloads(int mainsFreq)
{
	if (mainsFreq == 50)
	{
		currentOnAutoreloadValue = AC_50HZ_ON_AUTORELOAD_VALUE;
		currentOffAutoreloadValue = AC_50HZ_OFF_AUTORELOAD_VALUE;
		currentInitialAutoreloadValue = AC_50HZ_INITIAL_AUTORELOAD_VALUE;
	}
	else if (mainsFreq == 60)
	{
		currentOnAutoreloadValue = AC_60HZ_ON_AUTORELOAD_VALUE;
		currentOffAutoreloadValue = AC_60HZ_OFF_AUTORELOAD_VALUE;
		currentInitialAutoreloadValue = AC_60HZ_INITIAL_AUTORELOAD_VALUE;
	}
}

void DIM_ProcessDimmingInput(void)
{
	/* TODO: Receive values from outside the module */
}

void DIM_FrequencyDetection()
{
	if (detectionPhaseInProgress == true)
	{
		if (avgCycleCount > 8)
		{
			__disable_irq();
			DIM_TIM1_Run(false);

			unsigned short avgTimerCounts = __HAL_TIM_GET_COUNTER(&htim1) / 8;

			if (avgTimerCounts < AC_PERIOD_THLD_ILLEGAL)	// 13.048 ms or 76.64 Hz	(invalid higher frequencies)
			{
				avgCycleCount = 0;
				__HAL_TIM_SET_COUNTER(&htim1, 0);
			}
			else if (avgTimerCounts < AC_PERIOD_THLD_60)		// 17.5 ms or 57.14 Hz	(50 Hz nominal)
			{
				mainsFreq = 60;

				DIM_LoadLUTs(mainsFreq);
				DIM_LoadDefaultAutoReloads(mainsFreq);

				DIM_TIM1_Run(false);

				detectionPhaseInProgress = false;
			}
			else if (avgTimerCounts < AC_PERIOD_THLD_50)		// 21.652 or 46.18 Hz	(60 Hz nominal)
			{
				mainsFreq = 50;

				DIM_LoadLUTs(mainsFreq);
				DIM_LoadDefaultAutoReloads(mainsFreq);

				DIM_TIM1_Run(false);

				detectionPhaseInProgress = false;
			}
			else								// (invalid lower frequencies or no activity)
			{
				avgCycleCount = 0;
				__HAL_TIM_SET_COUNTER(&htim1, 0);
			}

			__enable_irq();
		}
	}
}

void DIM_TIM1_Run(bool preloadAndRun)
{
	if (preloadAndRun == true)
	{
		if (dimmerIsRunning == true)
		{
			__HAL_TIM_SET_COUNTER(&htim1, __HAL_TIM_GET_AUTORELOAD(&htim1));
		}

		__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
		__HAL_TIM_ENABLE(&htim1);
	}

	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
}

void DIM_TIM2_Run(bool hasToRun)
{
	if (hasToRun == false)
	{
		__HAL_TIM_DISABLE(&htim2);
	}
	else
	{
		__HAL_TIM_SET_COUNTER(&htim2, 0);

		__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
		__HAL_TIM_ENABLE(&htim2);
	}

	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
}

void DIM_Init()
{

	DIM_SCR_TurnOff();

	if (dimmerIsRunning == true)
	{
		DIM_IGBT_Release();
		DIM_SCR_A_Deactivate();
		DIM_SCR_B_Deactivate();
		DIM_SCR_TurnOff();

		(&htim1)->Instance->CR1 |= TIM_COUNTERMODE_DOWN;	// Set TIM1 direction to count down.

		__HAL_TIM_SET_COUNTER(&htim1, currentDimmingTableDescHi[dimmerAngle] << 8 | currentDimmingTableDescLo[dimmerAngle]);
		__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
		__HAL_TIM_SET_AUTORELOAD(&htim2, currentDimmingTableAscHi[dimmerAngle] << 8 | currentDimmingTableAscLo[dimmerAngle]);

		acLineStateTim1 = POSITIVE_HALF_CYCLE;
		acLineStateTim2 = POSITIVE_HALF_CYCLE;

		__enable_irq();

		/* Force interrupt */
		__HAL_TIM_SET_AUTORELOAD(&htim1, __HAL_TIM_GET_COUNTER(&htim1));

		// GOTO infinite empty loop

	}
	else
	{
		detectionPhaseInProgress = true;
		DIM_TIM2_Run(false);
		DIM_TIM1_Run(false);

		DIM_IGBT_Release();
		DIM_SCR_A_Deactivate();
		DIM_SCR_B_Deactivate();

		acLineStateTim1 = POSITIVE_HALF_CYCLE;
		acLineStateTim2 = POSITIVE_HALF_CYCLE;
		avgCycleCount = 0;
		dimmerAngle = 0;
		shortCircuitRetryCount = 0;
		shortCircuitDetectionFlag = false;
		dimmerIsRunning = false;
		setDefaultValueForTim2 = false;

		dimmerAngle = 8;
		__enable_irq();
		// Launch DIM_FrequencyDetection (blocking), finishes with detectionPhaseInprogress = false
		// Halt (disables exti0 and exti1)

	}
}

void DIM_Loop()
{
	/* EMPTY */
}

void DIM_Process()
{
	DIM_FrequencyDetection();
	/* Set here the desired dimmer angle */

	/* Implement some logic so that the relay shorts the dimmer for intensities > 80% */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM1)
	{
		DIM_IGBT_Trigger();

		DIM_TIM2_Run(true);

		DIM_SCR_Enable();

		if (DIM_ShortCircuit_IsDetected() == true)
		{
			DIM_EnterSafeMode();
		}

		if (acLineStateTim1 == POSITIVE_HALF_CYCLE)
		{
			if (shortCircuitDetectionFlag == false)
			{
				DIM_SCR_A_Activate();
			}

			DIM_DisableACDetection();

			__HAL_TIM_SET_AUTORELOAD(&htim1, currentOnAutoreloadValue);
			DIM_TIM1_Run(true);
			acLineStateTim1 = NEGATIVE_HALF_CYCLE;
		}
		else if (acLineStateTim1 == NEGATIVE_HALF_CYCLE)
		{
			if (shortCircuitDetectionFlag == false)
			{
				DIM_SCR_B_Activate();
			}
			__HAL_TIM_SET_AUTORELOAD(&htim1, currentDimmingTableDescHi[dimmerAngle] << 8 | currentDimmingTableDescLo[dimmerAngle]);
			acLineStateTim1 = POSITIVE_HALF_CYCLE;
		}
	}
	else if (htim->Instance == TIM2)
	{
		DIM_TIM2_Run(false);

		if (acLineStateTim2 == NEGATIVE_HALF_CYCLE)
		{
			DIM_SCR_B_Deactivate();
			DIM_IGBT_Release();
			acLineStateTim2 = POSITIVE_HALF_CYCLE;

			if (setDefaultValueForTim2 == true)
			{
				__HAL_TIM_SET_AUTORELOAD(&htim2, currentOffAutoreloadValue);
				setDefaultValueForTim2 = false;
			}
			else
			{
				__HAL_TIM_SET_AUTORELOAD(&htim2, currentDimmingTableAscHi[dimmerAngle] << 8 | currentDimmingTableAscLo[dimmerAngle]);
			}

			DIM_EnableACDetection();

		}
		else if (acLineStateTim2 == POSITIVE_HALF_CYCLE)
		{
			DIM_SCR_A_Deactivate();
			DIM_IGBT_Release();
			acLineStateTim2 = NEGATIVE_HALF_CYCLE;
			risingEdgeDetectionFlag = false;

			if (setDefaultValueForTim2 == true)
			{
				__HAL_TIM_SET_AUTORELOAD(&htim2, currentOffAutoreloadValue);
			}
		}

		DIM_SCR_TurnOff();
	}


}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* Callback code for RISING zero-crossing on AC line */
	if(GPIO_Pin == ZC_DETECT_Pin)
	{
		DIM_TIM1_Run(true);
		avgCycleCount++;
		risingEdgeDetectionFlag = true;
		// checkInputs();

		(direction == 1)? dimmerAngle++ : dimmerAngle--;

		if (dimmerAngle == 0) direction = 1;
		if (dimmerAngle > 19) direction = 0;

		/* Temporary replacement for check inputs */
		if (dimmerIsRunning == false && detectionPhaseInProgress == false)
		{
		    dimmerIsRunning = true;
		    (&htim1)->Instance->CR1 |= TIM_COUNTERMODE_DOWN;	// Set TIM1 direction to count down.

		    __HAL_TIM_SET_AUTORELOAD(&htim1, currentInitialAutoreloadValue);
		    setDefaultValueForTim2 = true;
		    __HAL_TIM_SET_COUNTER(&htim1, currentInitialAutoreloadValue);
		    __HAL_TIM_SET_AUTORELOAD(&htim2, currentOffAutoreloadValue);
		    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
		    HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

		}
	}
}
