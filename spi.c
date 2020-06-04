/*
 * spi.c
 *
 *  Created on: Nov 24, 2019
 *      Author: browt
 */
#include "msp.h"
#include <stdint.h>

void init_spi() {
    // Chip Select Line
    P3->OUT |= BIT7;
    P3->DIR |= BIT7;

    P1->SEL0 |= BIT4 | BIT5 | BIT6 | BIT7;  // set 4-SPI pin as second function

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST | // Remain in reset state
            EUSCI_B_CTLW0_MST |             // SPI master
            EUSCI_B_CTLW0_SYNC |            // Synchronous mode
            EUSCI_B_CTLW0_CKPL |            // Clock polarity high
            EUSCI_B_CTLW0_MSB |             // MSB first
            EUSCI_B_CTLW0_MODE_1 |          // 4-pin mode
            EUSCI_B_CTLW0_STEM |            // STE mode select
            EUSCI_B_CTLW0_SSEL__ACLK;       // ACLK

    EUSCI_B0->BRW = 0x01;                   // /2,fBitClock = fBRCLK/(UCBRx+1).
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;// **Initialize USCI state machine**

    //EUSCI_B0->IE |= EUSCI_B__TXIE;      // Enable TX interrupt
}

void spi_three_byte_write(uint8_t address, uint16_t data) {

    volatile int i = 0;
    uint8_t device_addr = (address << 1);
    uint8_t data_msb = (data >> 8) & 0x00FF;
    uint8_t data_lsb = (data & 0x00FF);

    // Beginning of transmission
    P3->DIR &= ~BIT7;
    for (i = 0; i < 200; i++) {};
    /*Use a value of 200 for a delay for-loop between transmissions*/

    // Transmit characters
    EUSCI_B0->TXBUF = device_addr;
    // Wait until transmit buffer is clear
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    // Transmit characters
    EUSCI_B0->TXBUF = data_msb;
    // Wait until transmit buffer is clear
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    // Transmit characters
    EUSCI_B0->TXBUF = data_lsb;
    // Wait until transmit buffer is clear
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    for (i = 0; i < 200; i++) {};

    // End of Transmission. Deselect line
    P3->DIR |= BIT7;
}

//void EUSCIB0_IRQHandler(void) {
//
//}

