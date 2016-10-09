
#include "uart_controls.h"
void uart_init(uint32_t baud) {
    uint32_t uartPclk, pclk, regVal;
    LPC_SC->PCONP |= (1 << PCUART3);
    LPC_PINCON->PINSEL0 |= (0x02 << 0);    // set P0.0 RXD3
    LPC_PINCON->PINSEL0 |= (0x02 << 2);    // set P0.1 TXD3
    LPC_UART3->FCR  = (1 << U_FCR_FIFO_ENABLE) | (1 << U_FCR_FIFO_RX_RST) | (1 << U_FCR_FIFO_TX_RST);

    LPC_UART3->LCR |= (3 << U_LCR_WORD_LEN);    // data:        8 bit
    LPC_UART3->LCR &= ~(1 << U_LCR_PARITY_EN);  // parity:      None
    LPC_UART3->LCR &= ~(1 << U_LCR_STOP_BIT);   // stop bits:   1 bit

    // Enable RBR interrupts
    LPC_UART3->IER |= (1 << U_IER_RBR_INT_EN);    // enable RDA interrupts

    LPC_UART3->LCR |= (1 << U_LCR_DIV_LATCH);   // enable access to divisors
    // setup baud rate
    uartPclk = (LPC_SC->PCLKSEL1 >> 18) & 0x03; // get PCLK selection for UART3
    switch (uartPclk) {
        case 0x00:
            pclk = SystemCoreClock/4;
            break;
        case 0x01:
            pclk = SystemCoreClock;
            break;
        case 0x02:
            pclk = SystemCoreClock/2;
            break;
        case 0x03:
            pclk = SystemCoreClock/8;
            break;
    }

    regVal = (pclk/(16 * baud));
    LPC_UART3->DLL = regVal & 0xFF;
    LPC_UART3->DLM = (regVal >> 0x08) & 0xFF;
    // done setting up baud rate
    LPC_UART3->LCR &= ~(1 << U_LCR_DIV_LATCH);  // disable access to divisors
}

void uart_tx(char ch) {
    while(CHECK_BIT(LPC_UART3->LSR, U_LSR_THRE) == 0);  // wait for TX reg to empty
    LPC_UART3->THR = ch;                                // load TX char
}

char uart_rx() {
    char rx = 'X';
    volatile int i = 0;
    while(CHECK_BIT(LPC_UART3->LSR, U_LSR_RDR) == 0) i++;   // wait until char is received
    rx = LPC_UART3->RBR;
    return rx;
}

void uprintf(const char * fmt, ...) {
    char c[MAX_PRINT_LENGTH];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(c, fmt, argptr);

    int i = 0;
    while(c[i] != '\0' && i < MAX_PRINT_LENGTH) {
        uart_tx(c[i]);
        i++;
    }

    va_end(argptr);
}