#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lpc17xx_libcfg.h"
#include "uart_controls.h"

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
                uprintf("\tACK: %s", URX_BUF.stream);
                uprintf(GREET_WAIT_CMD);
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
    initBuffer(&URX_BUF);
    uart_init(9600);
    NVIC_EnableIRQ(UART3_IRQn);
    uprintf(GREET);
    uprintf(GREET_WAIT_CMD);
    while(1) {
        // rxChar = uart_rx();
        // uprintf("I've received: %c\n", rxChar);
        // uart_tx(rxChar);
    }
    return 0 ;
}
