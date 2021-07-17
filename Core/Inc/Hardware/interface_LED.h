/*
 * interface_LED.h
 *
 *  Created on: 2021/07/16
 *      Author: Shotaro Shimegi
 */

#ifndef INC_HARDWARE_INTERFACE_LED_H_
#define INC_HARDWARE_INTERFACE_LED_H_

#include <stdint.h>

typedef enum {
	OFF,
	ON
} LED_STATES;

void setLED1State(uint8_t state);
void setLED2State(uint8_t state);
void setLED3State(uint8_t state);
void setLED4State(uint8_t state);
void setLED5State(uint8_t state);


#endif /* INC_HARDWARE_INTERFACE_LED_H_ */
