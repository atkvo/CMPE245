#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lpc17xx_libcfg.h"

#define MAX_PRINT_LENGTH 1024
#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

#define URX_BUF_LEN             1024
// char URX_BUF[URX_BUF];
typedef struct
{
    char stream[URX_BUF_LEN];
    int i;
} RX_BUFFER;

RX_BUFFER URX_BUF;

const char GREET[] = "**** Welcome ****\n";
const char GREET_WAIT_CMD[] = "Please enter your command and press enter: \n\r\t";

#define PCUART2            24
#define PCUART3            25

#define U_IER_RBR_INT_EN   0
#define U_IIR_RDA_INT_ID   (0x2 << 1) 
#define U_FCR_FIFO_ENABLE  0x00
#define U_FCR_FIFO_RX_RST  0x01
#define U_FCR_FIFO_TX_RST  0x02

#define U_LCR_WORD_LEN     0x00
#define U_LCR_STOP_BIT     0x02
#define U_LCR_PARITY_EN    0x03
#define U_LCR_PARITY_SEL   0x04
#define U_LCR_DIV_LATCH    0x07

#define U_LSR_RDR          0x00
#define U_LSR_THRE         0x05
#define U_LSR_TEMT         0x06

void initBuffer(RX_BUFFER *buf) {
    buf->i = 0;
}

void clearBuffer(RX_BUFFER *buf) {
    buf->i = 0;
    for(int i = 0; i < URX_BUF_LEN; i++) {
        buf->stream[i] = 0;
    }
}

void addToBuffer(RX_BUFFER *buf, char c) {
    if(buf->i >= URX_BUF_LEN - 1) {
        clearBuffer(buf);
    } 
    buf->stream[buf->i] = c;
    buf->i++;
}

void uart_init(uint32_t baud) {
    initBuffer(&URX_BUF);
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
void UART3_IRQHandler(void) {
    if ((LPC_UART3->IIR & U_IIR_RDA_INT_ID) == U_IIR_RDA_INT_ID) {
        if(URX_BUF.i >= (URX_BUF_LEN - 1)) {
            clearBuffer(&URX_BUF);
            uprintf("Input buffer overrun.\r\n");
        }
        else {
            char ch = uart_rx();
            addToBuffer(&URX_BUF, ch);
            if(ch == '\n') {
                uprintf("RX: %s", URX_BUF.stream);
                clearBuffer(&URX_BUF);
                // command is entered
            }
        }
    }
}

int main(void)
{
    SystemInit();
    SystemCoreClockUpdate();
    uart_init(9600);
    NVIC_EnableIRQ(UART3_IRQn);
    uprintf(GREET);
    while(1) {
        // rxChar = uart_rx();
        // uprintf("I've received: %c\n", rxChar);
        // uart_tx(rxChar);
    }
    return 0 ;
}
