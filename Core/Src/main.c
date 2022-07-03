/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>

#include"Controller/mode.h"
#include"Controller/state.h"
#include"Controller/callback.h"
#include"Controller/search.h"
#include"Controller/test.h"

#include"System/music.h"
#include"System/sensing.h"
#include"System/motion.h"
#include"System/log.h"
#include"System/flags.h"
#include"System/map.h"
#include"System/route.h"

#include"Hardware/ICM20689.h"
#include"Hardware/basic_timer.h"
#include"Hardware/battery.h"
#include"Hardware/buzzer.h"
#include"Hardware/encoder.h"
#include"Hardware/interface_LED.h"
#include "Hardware/motor.h"
#include"Hardware/wall_sensor.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int c)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

int __io_putchar(int c)
{
	if( c == '\n'){
		uint8_t _c = '\r';
		  LL_USART_TransmitData8(USART1, _c);
		  while(LL_USART_IsActiveFlag_TXE(USART1) == 0);
	}
	LL_USART_TransmitData8(USART1, (uint8_t)c);
	while(LL_USART_IsActiveFlag_TXE(USART1) == 0);
	return 0;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */
  setbuf(stdout,NULL);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_SPI3_Init();
  MX_TIM11_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  enableMelody();
  waitMs(1);

  initSensors();
  initMouseStatus();
  if(readBatteryCheck()==0)		errorFunctions(1);
  MF.FLAG.FIRST = 1;

  uint8_t mode;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  mode = modeSelect();
	  waitMs(500);
	  switch(mode){
	  	  case 0:		//		ログの吐き出し
	  		 MF.FLAG.SAFETY= 0;
	  		waitStarting();
	  		  for(uint16_t i = 0;i<MAX_LOG;i++){
	  			  printf("%.3lf, %.3lf, %.3lf, %.3lf,%.3lf, %.3lf \n",
	  					log[i].target_velocity, log[i].target_omega,log[i].real_velocity, log[i].real_omega,
						log[i].mileage, log[i].angle);
	  			  waitMs(1);
	  		  }
	  		  break;

	  	  case 1:		//		探索走行
	  		  MelodyNatsumatsuri();

	  		  waitStarting();
	  		  getOffsets();
	  		  enableEncoder();
//	  		  basicTimerStart();
	  		  enableMotors();
	  		  mouse.angle = 0.0f;

	  		  goal.x = GOAL_X;
	  		  goal.y = GOAL_Y;
	  		  searchMazeBySlalom(GOAL_LENGTH);
	  		  goal.x = 0;
	  		  goal.y = 0;
	  		  enableMotors();
	  		  searchMazeBySlalom(RETURN_GOAL_LENGTH);
	  		  goal.x = GOAL_X;
	  		  goal.y = GOAL_Y;

	  		  basicTimerStart();
	  		  enableMotors();

	  		  rotateSafteyR180();
	  		  changeDirection(DIR_SPIN_180);
	  		  basicTimerPause();
	  		  shutdownMotors();
	  		  break;

	  	  case 2:
	  		  //記憶された迷路を読み出す、付け焼刃の疑似最短モード、
	  		  MelodyUrara();
	  		  MF.FLAG.SCND = ON;
	  		  MF.FLAG.FIRST = OFF;
	  		  loadWallMap();

	  		  break;

	  	  case 3 :
	  		  break;

	  	  case 4 :		//	前壁制御
	  		  waitStarting();
	  		  getOffsets();
	  		  enableEncoder();
	  		  basicTimerStart();
	  		  enableMotors();

	  		  fixPostureByWallSensor();

	  		  basicTimerPause();
	  		  shutdownMotors();
	  		  break;

	  	  case 5 :		//	新制御方式の走行テスト
	  		 printf("--------Test Motions---------\n");
	  		 MelodySummer();
	  		 testMotions();
	  		  break;

	  	  case 8 :
	  		  break;

	  	  case 12:
	  		  printf("--------IMU Test---------\n");
	  		  testIMU();
	  		  break;

	  	  case 13:
	  		  printf("--------Encoders Test---------\n");
	  		  testEncoders();
	  		  break;

	  	  case 14:
	  		  printf("--------Wall Sensor Test---------\n");
	  		  testWallSensors();
	  		  break;

	  	  default:
	  		  break;
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_8, 160, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_4);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(160000000);
  LL_SetSystemCoreClock(160000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
