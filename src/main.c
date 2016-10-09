#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lpc17xx_libcfg.h"
#include "uart_controls.h"
#include "engine.h"

#define MAX_PRINT_LENGTH 1024
#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

//#define URX_BUF_LEN             1024

s_buff URX_BUF;

const char GREET[] = "**** Welcome ****\n";
const char GREET_WAIT_CMD[] = "Please enter your command and press enter: \n\r\t";
void UART3_IRQHandler(void) {
    if ((LPC_UART3->IIR & U_IIR_RDA_INT_ID) == U_IIR_RDA_INT_ID) {
        if(URX_BUF.bits >= (BUF_SIZE - 1)) {
            clearBuffer(&URX_BUF);
            uprintf("Input buffer overrun.\r\n");
        }
        else {
            char ch = uart_rx();
            pushToBuffer(&URX_BUF, ch);
            if(ch == '\n') {
                uprintf("\tACK: %s", URX_BUF.stream);
                uprintf(GREET_WAIT_CMD);
                clearBuffer(&URX_BUF);
                // command is entered
            }
        }
    }
}

#define RITINT   0
#define RITENCLR 1
#define RITEN    3
#define PCRIT    16
void ritSetHz(int desiredHz) {
    // default PCLK is SystemCoreClock/4
    int timerHz = SystemCoreClock/4;
    int countsRequired = timerHz/desiredHz;
    LPC_RIT->RICOMPVAL = countsRequired;
}
void ritInit(int desiredHz){
    LPC_SC->PCONP |= (1 << PCRIT);
    LPC_RIT->RICTRL |= (1 << RITINT);
    ritSetHz(desiredHz);
}

void ritStart() {
    LPC_RIT->RICOUNTER = 0;
    LPC_RIT->RICTRL |= (1 << RITEN);
    LPC_RIT->RICTRL |= (1 << RITENCLR);
}

void ritStop() {
    LPC_RIT->RICTRL &= ~(1 << RITEN);
    LPC_RIT->RICTRL &= ~(1 << RITENCLR);
}

void RIT_IRQHandler(void) {
    uprintf("TICK!\n");
    // clear the interrupt by setting 1
    LPC_RIT->RICTRL |= (1 << RITINT);
}

int main(void)
{
    SystemInit();
    SystemCoreClockUpdate();
    initBuffer(&URX_BUF);
    uart_init(9600);
    ritInit(2);
    ritStart();
    NVIC_EnableIRQ(UART3_IRQn);
    NVIC_EnableIRQ(RIT_IRQn);
    uprintf(GREET);
    uprintf(GREET_WAIT_CMD);
    while(1) {
        // rxChar = uart_rx();
        // uprintf("I've received: %c\n", rxChar);
        // uart_tx(rxChar);
    }
    return 0 ;
}
