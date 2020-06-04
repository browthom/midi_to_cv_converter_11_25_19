/*
 * max11300.h
 *
 *  Created on: Nov 15, 2019
 *      Author: browt
 */

#ifndef MAX11300_H_
#define MAX11300_H_

#include <stdint.h>

// FUNCID
// (3:0) -> (0101) -> DAC Output
// -> Goes to PORT_PRM_ADDR(15:12)
//
// FUNCPRM
// Bit 11 -> X
// (10:8) -> (001) -> 0 to +10V Range
// (7:5) -> (XXX) -> ADC functionality only
// (4:0) -> (XXXXX)
// -> Goes to PORT_PRM_ADDR(11:0)
//
// ADCCTL
// (1:0) -> (00) -> Idle
// DACCTL
// (3:2) -> (01) -> Immediate update for DAC configured ports
// DACREF
// (Bit 6) -> (1) -> Internal Reference
// BRST
// (Bit 13) -> (0) -> Default Power Mode
// BRST
// (Bit 14) -> (0) -> Default address incrementing mode.
// -> Goes to Device Control Register
//

// 0000 0000 0100 0100
#define DEV_CTRL_WORD_1 0x00
#define DEV_CTRL_WORD_0 0x44
// 0101 0001 0000 0000
#define DAC_PORT_PRM_WORD_1 0x51
#define DAC_PORT_PRM_WORD_0 0x00

// Device Control Register
// 0001 0000 + R/W bit
#define DEV_CTRL_ADDR 0x10

// Port Configurations
// 0010 0000 + R/W bit
#define PORT_PRM_ADDR_0_15 0x20
// 0011 0000 + R/W bit
#define PORT_PRM_ADDR_16_19 0x30

// DAC port data
// 0110 0000 + R/W bit
#define DAC_DATA_ADDR_0_15 0x60
// 0111 0000 + R/W bit
#define DAC_DATA_ADDR_16_19 0x70

void write_vco_pitch(int vco_number, uint16_t pitch);
void write_vco_velocity(int vco_number, uint16_t velocity);
uint16_t convert_to_vco_value(uint8_t value);
uint16_t scale_dac_value(uint8_t value);
void configure_dac_port(int dac_number, uint8_t dac_value_1, uint8_t dac_value_2);
void configure_dacs(void);

#endif /* MAX11300_H_ */
