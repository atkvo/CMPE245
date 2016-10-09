#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "lpc17xx_libcfg.h"
#include "uart_controls.h"
#include "engine.h"

#define MAX_PRINT_LENGTH 1024
#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

//#define URX_BUF_LEN             1024

s_buff URX_BUF;

const char GREET[] = "**** Welcome ****\n";
const char GREET_WAIT_CMD[] = "Please enter your command and press enter: \n\r\t";
const char CMD_TIMER_START[] = "TIMERSTART";
const char CMD_TIMER_STOP[] = "TIMERSTOP";
const char CMD_TIMER_SET[] = "TIMERSET";
const int CMD_TIMER_PARAM_INDEX = 8;

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

void UART3_IRQHandler(void) {
    if ((LPC_UART3->IIR & U_IIR_RDA_INT_ID) == U_IIR_RDA_INT_ID) {
        if(URX_BUF.bits >= (BUF_SIZE - 1)) {
            clearBuffer(&URX_BUF);
            uprintf("Input buffer overrun.\r\n");
        }
        else {
            char ch = uart_rx();
            if(ch == '\n') {
                uprintf("\tACK: %s", URX_BUF.stream);
                if (strcmp(CMD_TIMER_START, URX_BUF.stream) == 0) {
                    ritStart();
                }
                else if (strcmp(CMD_TIMER_STOP, URX_BUF.stream) == 0) {
                    ritStop();
                }
                else if (strstr(URX_BUF.stream, CMD_TIMER_SET) != NULL) {
                    char * tmp;
                    int newrate = strtol(&URX_BUF.stream[CMD_TIMER_PARAM_INDEX], &tmp, 10);
                    if(newrate > 0) {
                        uprintf("\tSetting rate to: %i Hz\n", newrate);
                        ritStop();
                        ritSetHz(newrate);
                        ritStart();
                    }
                    else { 
                        uprintf("\tInvalid rate: %i Hz\n", newrate);
                    }
                }
                uprintf(GREET_WAIT_CMD);
                clearBuffer(&URX_BUF);
                // command is entered
            }
            else if (ch != '\r') {
                pushToBuffer(&URX_BUF, ch);
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
