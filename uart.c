#include "msp.h"
#include <string.h>
#include "uart.h"
#include "midi.h"
/*
 * uart_pc.c
 *
 *  Created on: Sep 20, 2019
 *      Author: browt
 */

uint8_t pitch_buffer;
uint8_t buffer[50];
int buffer_index = 0;
enum state {neutralState = 0, waitingForKeyValue = 1, waitingForVelocity = 2} MIDI_State = neutralState;

void init_uart_pc() {
    // Configure UART pins for MSP432 to PC Communication
    P1->SEL0 |= BIT2 | BIT3;                                                // set 2-UART pin as secondary function

    // Configure UART for MSP432 to PC Communication
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;                                 // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST |                                 // Remain eUSCI in reset
                      EUSCI_A_CTLW0_SSEL__SMCLK;                            // Configure eUSCI clock source for SMCLK
    EUSCI_A0->BRW = 78;                                                     // N = 12000000/115200 = 104.166
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;    // (INT) (N / 16) = 6.510416666
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;                                // Initialize eUSCI
//    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;                                    // Clear eUSCI RX interrupt flag
//    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;                                        // Enable USCI_A0 RX interrupt
}

void init_uart_midi() {
    /*Setup Code for MIDI Device to MSP432 Communication*/
    P3->SEL0 |= BIT2 | BIT3;                                                // P3.2 -> RX; P3.3 -> TX

    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST;                                 // Reset eUSCI
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK;      // Keep eUSCI in reset; Set BRCLK source to SMCLK
    EUSCI_A2->BRW = 6;                                                     // Clock Prescaler setting of baud-rate generator
                                                                            // N = f_BRCLK / baud_rate -> 12,000,000/31,250 = 384
                                                                            // INT (N / 16)  INT (384 / 16) = 24
                                                                            // N = f_BRCLK / baud_rate -> 3,000,000/31,250 = 96
                                                                            // INT (N / 16)  INT (384 / 16) = 6
                                                                            // These Calculations were done with the help of Section 24.3.10
    EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;    // OS16 enables oversampling mode;
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;                                // Initialize eUSCI_A2
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;                                    // Clear eUSCI RX interrupt flag
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;                                        // Enable Receive Interrupt
}

// UART ISR for communication between MSP432 and PC
//void EUSCIA0_IRQHandler(void) {
//
//    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){
//
//        // Check if the TX buffer is empty first
//        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//
//        // Echo the received character back
//        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
//    }
//}

// UART ISR for communication between MSP432 and MIDI Device
void EUSCIA2_IRQHandler(void) {

    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG){

        // Echo the received character back
        uint8_t midi_char = EUSCI_A2->RXBUF;

        // If not 0xF8 or 0xFE
        if (midi_char != TIMING_CLOCK && midi_char != ACTIVE_SENSING) {
            // For debugging purposes
            if (buffer_index < 50) {
                buffer[buffer_index] = midi_char;
                buffer_index++;
            }
        }

        // If Status Byte
        if ((midi_char & 0x80) == 0x80) {

            // If byte is 'note on event', look for pitch and velocity values
            if ((midi_char & 0xF0) == NOTE_ON_EVENT && MIDI_State == neutralState) {
                MIDI_State = waitingForKeyValue;
            }

        // If Data Byte
        } else if ((midi_char & 0x80) != 0x80) {

            // Retrieve Pitch (Note) Value
            if (MIDI_State == waitingForKeyValue) {
                pitch_buffer = midi_char;
                MIDI_State = waitingForVelocity;

            // Retrieve Velocity Value
            } else if (MIDI_State == waitingForVelocity) {

                // Velocity is 0; thus the note has been released
                if (midi_char == 0) {

                    int i = 0;

                    // Search for index of vco that has this pitch
                    while (pitch_buffer != pitch[i] && i < 8) {
                        i++;
                    }

                    // Ensure index out of bounds does not occur
                    if (i < 8) {

                        // Clear CVs for specific VCO
                        pitch[i] = 0;
                        velocity[i] = 0;

                        // Indicate that a voice has been changed
                        CV_CHANGE_FLAG[i] = 1;
                    }

                // Note has been pressed and velocity data
                // has been received.
                } else {

                    int i = 0;

                    // Find first available
                    while (pitch[i] != 0 && i < 8) {
                        i++;
                    }

                    // Ensure index out of bounds does not occur
                    if (i < 8) {
                        pitch[i] = pitch_buffer;
                        velocity[i] = midi_char;

                        // Indicate that a voice has been changed
                        CV_CHANGE_FLAG[i] = 1;
                    }
                }

                MIDI_State = waitingForKeyValue;
            }
        }
    }
}
