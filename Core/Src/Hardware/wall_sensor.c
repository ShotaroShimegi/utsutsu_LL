/*
 * wall_sensor.c
 *
 *  Created on: 2021/07/17
 *      Author: sssho
 */

#include"Hardware/wall_sensor.h"
#include"main.h"
#include"adc.h"

uint16_t dma_data_buff[5];
void ADC1_Start_DMA2(){
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_0);
	LL_DMA_ConfigAddresses(DMA2, LL_DMA_STREAM_0,
			LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
			(uint32_t)dma_data_buff, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_0,5);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_0);

	LL_ADC_REG_StartConversionSWStart(ADC1);
}

void startADCwithDMA(void)
{
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_0);
	LL_ADC_Enable(ADC1);

	ADC1_Start_DMA2();
}

void DMA_CallbackTask(void)
{
	LL_ADC_ClearFlag_EOCS(ADC2);
	ADC1_Start_DMA2();
}

uint16_t getWallADC(uint8_t number){
	return dma_data_buff[number];
}

/**
* @brief		壁センサ用LEDの発光関数、チャージポンプあるのでタイミングシビア
* @param	state	LEDのON / OFF
* @details
*/

void changeFrontCenterLED(uint8_t state) {
	if (state == 1) 	FrontCenter_GPIO_Port->BSRR = FrontCenter_Pin;
	else					FrontCenter_GPIO_Port->BSRR = (FrontCenter_Pin << 16);
}

void changeFrontRightLED(uint8_t state) {
	if (state == 1) 	FrontRight_GPIO_Port->BSRR = FrontRight_Pin;
	else					FrontRight_GPIO_Port->BSRR = (FrontRight_Pin << 16);
}

void changeFrontLeftLED(uint8_t state) {
	if (state == 1) 	FrontLeft_GPIO_Port->BSRR = FrontLeft_Pin;
	else					FrontLeft_GPIO_Port->BSRR = (FrontLeft_Pin << 16);
}

void changeRightLED(uint8_t state) {
	if (state == 1) 	Right_GPIO_Port->BSRR = Right_Pin;
	else					Right_GPIO_Port->BSRR = (Right_Pin<< 16);
}

void changeLeftLED(uint8_t state){
	if (state == 1) 	Left_GPIO_Port->BSRR = Left_Pin;
	else					Left_GPIO_Port->BSRR = (Left_Pin<< 16);
}
