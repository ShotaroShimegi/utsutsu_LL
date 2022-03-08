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

#include"System/music.h"
#include"System/sensing.h"
#include"System/motion.h"
#include"System/log.h"
#include"System/flags.h"

#include"Hardware/ICM20689.h"
#include"Hardware/basic_timer.h"
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
  MelodySummer();

  initSensors();
  initMouseStatus();
  judgeFailSafe();

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
	  	  case 0:
	  		 MF.FLAG.SAFETY= 0;
	  		waitStarting();
	  		  for(uint16_t i = 0;i<MAX_LOG;i++){
	  			  printf("%.3lf, %.3lf, %.3lf, %.3lf,%.3lf, %.3lf \n",
	  					log[i].target_velocity, log[i].target_omega,log[i].real_velocity, log[i].real_omega,
						log[i].mileage, log[i].angle);
	  			  waitMs(1);
	  		  }
	  		  break;

	  	  case 1:
	  		  waitStarting();
	  		  getOffsets();
	  		  enableEncoder();
	  		  mouse.angle = 0.0f;
	  		  basicTimerStart();
	  		  enableMotors();

	  		  tim_counter = 0;
	  		  driveAccelMotion(90.0f, max.velocity,OFF);
	  		  slalomMotion(90);
	  		  driveAccelMotion(90.0f, 0.0f,OFF);
	  		  shutdownMotors();
	  		  break;
	  	  case 2 :			//	左無限スラローム
	  		  waitStarting();
	  		  getOffsets();
	  		  enableEncoder();
	  		  basicTimerStart();
	  		  enableMotors();

	  		  moveHalfSectionAccel(OFF, OFF);
	  		  tim_counter = 0;
  			  moveSlalomL90();
  			  moveOneSectionAccel(OFF);

	  		  for(uint8_t cnt=0;cnt<16;cnt++){
	  			  moveSlalomL90();
	  			  moveOneSectionAccel(OFF);
	  		  }
	  		  moveHalfSectionDecel(OFF);
	  		  spinRight180();

	  		  basicTimerPause();
	  		  shutdownMotors();
	  		  break;

	  	  case 3 :		//	右無限スラローム
	  		  waitStarting();
	  		  getOffsets();
	  		  enableEncoder();
	  		  basicTimerStart();
	  		  enableMotors();

	  		  tim_counter = 0;
	  		  moveHalfSectionAccel(OFF, OFF);
	  		  for(uint8_t cnt=0;cnt<16;cnt++){
	  			  displayBinaryValueWithLEDs(0xff);
	  			  moveSlalomR90();
	  			  displayBinaryValueWithLEDs(0x00);
	  			  moveOneSectionAccel(OFF);
	  		  }
	  		  moveHalfSectionDecel(OFF);
	  		  spinRight180();

	  		  basicTimerPause();
	  		  shutdownMotors();
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

	  	  case 12:
	  		  waitStarting();
	  		  getOffsets();
	  		  basicTimerStart();
	  		  MF.FLAG.SAFETY = 0;
	  		  while(1){
		  		printf("Angle : %lf\n", mouse.angle);
		  		waitMs(500);
	  		  }
	  		  break;

	  	  case 13:
	  		  waitStarting();
	  		  enableEncoder();
	  		  basicTimerStart();
	  		  MF.FLAG.SAFETY = 0;
	  		  while(1){
	  			printf("dist : %.2lf,  L :%.2lf, R : %.2lf \n", mouse.mileage,sensor.mileage_l_mm,sensor.mileage_r_mm);
	  			waitMs(500);
	  		  }
	  		  break;

	  	  case 14:
	  		  waitStarting();
	  		  getOffsets();
	  		  enableEncoder();
	  		  MF.FLAG.SAFETY = 0;
	  		  basicTimerStart();
	  		  shutdownMotors();
	  		printf("--------Wall Sensor Test---------\n");
	  		  while(1){
	  			  printf("FL: %4d L: %4d FF: %4d R: %4d FR: %4d\n",
	  					  sensor.wall_fl,sensor.wall_l,sensor.wall_ff,
						  sensor.wall_r,sensor.wall_fr);
	  			  waitMs(500);
	  		  }

	  		  break;

	  	  default:
	  		  break;
	  }

//	  shutdownMotors();

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
