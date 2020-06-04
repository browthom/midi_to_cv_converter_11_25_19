/*
 * spi.h
 *
 *  Created on: Nov 24, 2019
 *      Author: browt
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void init_spi(void);
void spi_three_byte_write(uint8_t address, uint16_t data);

#endif /* SPI_H_ */
