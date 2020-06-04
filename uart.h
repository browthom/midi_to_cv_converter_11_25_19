/*
 * uart.h
 *
 *  Created on: Sep 20, 2019
 *      Author: browt
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

int CV_CHANGE_FLAG[8];
uint8_t pitch[8], velocity[8];

void init_uart_pc();
void init_uart_midi();
void EUSCIA0_IRQHandler(void);
void EUSCIA2_IRQHandler(void);

#endif /* UART_H_ */
