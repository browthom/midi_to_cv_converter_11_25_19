/*
 * max11300.c
 *
 *  Created on: Nov 24, 2019
 *      Author: browt
 */

#include <stdint.h>
#include "spi.h"
#include "max11300.h"

void write_vco_pitch(int vco_number, uint16_t pitch) {
    spi_three_byte_write(DAC_DATA_ADDR_0_15 + (vco_number-1), pitch);
}

void write_vco_velocity(int vco_number, uint16_t velocity) {
    spi_three_byte_write(DAC_DATA_ADDR_0_15 + 8 + (vco_number-1), velocity);
}

// MIDI Note 24 (C1) -> 1V -> 410
// MIDI Note 36 (C2) -> 2V -> 819
// MIDI Note 48 (C3) -> 3V -> 1229
// MIDI Note 60 (C4) -> 4V -> 1638
// MIDI Note 72 (C5) -> 5V -> 2048
// MIDI Note 84 (C6) -> 6V -> 2457
// MIDI Note 96 (C7) -> 7V -> 2867
// 0.0833mV / (10V / 2^12) = 34 LSB per half step
uint16_t convert_to_vco_value(uint8_t value) {
    // Lowest value: 21 -> 306 = 34*(21) - 408
    // Highest value: 108 -> 3,264 = 34*(108) - 408
    if (value != 0) {
        return ((34 * value) - 408);
    }
    return 0;
}

uint16_t scale_dac_value(uint8_t value) {
    // Scale 8-bit value to 12-bit value
    // A 16-bit value will be returned,
    // but the 4 most signficant bits will be 0.
    return (value * 32);
}

void configure_dac_port(int dac_number, uint8_t dac_value_1, uint8_t dac_value_2) {
    // Write Pitch
    spi_three_byte_write(DAC_DATA_ADDR_0_15 + (dac_number), scale_dac_value(dac_value_1));
    // Write Velocity
    spi_three_byte_write(DAC_DATA_ADDR_0_15 + 8 + (dac_number), scale_dac_value(dac_value_2));
}

void configure_dacs() {

    volatile int i, j = 0;

    // Step 1: BRST, THSHDN, ADCCONV
    // Step 2: Configure DACREF, DACCTL
    // 0000 0000 0100 0100
    spi_three_byte_write(DEV_CTRL_ADDR, 0x0044);

    for (i = 0; i < 200; i++) {};

    // Step 3: DAC data write
    // Write DAC Data
    for (i = 0; i < 20; i++) {
        spi_three_byte_write(DAC_DATA_ADDR_0_15 + i, 0x0000);
    }

    for (i = 0; i < 400; i++) {};

    // Step 4: Set Port Paremeters
    for (i = 0; i < 20; i++) {
        spi_three_byte_write(PORT_PRM_ADDR_0_15 + i, 0x5100);
        for (j = 0; j < 1000; j++) {};
    }
}

