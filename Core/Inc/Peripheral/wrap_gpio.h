/*
 * wrap_gpio.h
 *
 *  Created on: 2021/03/20
 *      Author: sssho
 */

#ifndef INC_PERIPHERAL_WRAP_GPIO_H_
#define INC_PERIPHERAL_WRAP_GPIO_H_

void changeLED1Status(uint8_t state);
void changeLED2Status(uint8_t state);
void changeLED3Status(uint8_t state);
void changeLED4Status(uint8_t state);
void changeLED5Status(uint8_t state);
void changeLED6Status(uint8_t state);
void changeLED7Status(uint8_t state);
void changeLED8Status(uint8_t state);

void changeFrontCenterStatus(uint8_t state);
void changeFrontRightStatus(uint8_t state);
void changeFrontLeftStatus(uint8_t state);
void changeRightStatus(uint8_t state);
void changeLeftStatus(uint8_t state);

void changeSPI_CSStatus(uint8_t state);

#endif /* INC_PERIPHERAL_WRAP_GPIO_H_ */
