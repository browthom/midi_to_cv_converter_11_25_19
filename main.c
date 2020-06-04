#include "msp.h"
#include "spi.h"
#include "uart.h"
#include "max11300.h"

int main(void) {

    // Stop WDT
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // Configure GPIO
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;

    // Setup SPI communication with MAX11300
    init_spi();

    // Delay a certain amount to make sure that the MAX11300 power supplies
    // have enough power initialization time before configuring DACs.
    volatile uint32_t k = 0;
    for (k = 0; k < 300000; k++);

    // Configure DACs on the MAX11300
    configure_dacs();

    // Initialize UART communication with MIDI keyboard
    init_uart_midi();

    // Enable eUSCIA2 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn));// |
                    //1 << ((EUSCIB0_IRQn));
    // Enable global interrupt
    __enable_irq();

    int cv_flag_index = 0;

    while (1) {

        if (CV_CHANGE_FLAG[cv_flag_index] == 1) {
            // Clear flag
            CV_CHANGE_FLAG[cv_flag_index] = 0;

            // Set Pitch and velocity
            configure_dac_port(cv_flag_index, pitch[cv_flag_index], velocity[cv_flag_index]);
        }

        if (cv_flag_index < 7) {
            cv_flag_index++;
        } else {
            cv_flag_index = 0;
        }
    }
}
