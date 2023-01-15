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

/**
* @brief		壁センサ用LEDの発光関数、チャージポンプあるのでタイミングシビア
* @params 	LEDのON / OFF
* 1 . Front Left
* 2 . LEFT
* 3 . Front Center
* 4 . RIGHT
* 5 . Front Right
*/
void changeLEDs(uint8_t fl,uint8_t l, uint8_t ff, uint8_t r,uint8_t fr)
{
	changeFrontLeftLED(fl & 0x01);
	changeLeftLED( l & 0x01);
	changeFrontCenterLED( ff & 0x01);
	changeRightLED( r & 0x01);
	changeFrontRightLED(fr & 0x01);
}

/**
* @brief			壁センサの値取得
* @params 	欲しい壁センサの配列番号、0 : FL ~ 4 : FR
* @return		ADCの値、最大値4096
*/
uint16_t getWallADC(uint8_t number)
{
	return dma_data_buff[number];
}
/*
* @brief			ADCのDMAをスタート、1度だけ呼び出せばおｋ
*/
void startADCwithDMA(void)
{
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_0);
	LL_ADC_Enable(ADC1);

	ADC1_Start_DMA2();
}
/**
* @brief			ADCを止める、再び動くかは怪しい
*/
void pauseADCwithDMA(void){
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_0);
	LL_ADC_Disable(ADC1);
}
/**
* @brief			DMA割込みで動くタスク、再スタート
*/
void DMA_CallbackTask(void)
{
	LL_ADC_ClearFlag_EOCS(ADC2);
	ADC1_Start_DMA2();
}
