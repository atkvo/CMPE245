#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "lpc17xx_libcfg.h"
#include "uart_controls.h"
#include "samplingtimer.h"
#include "gpiocontrols.h"
#include "engine.h"

#define MAX_PRINT_LENGTH 1024
#define MAX_RX_LEN       1024
#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

int SYNC_BYTES      = 32;
int MIN_CONFIDENCE  = 3;
int ENABLE_EVENT_MSGS = 0;

//#define URX_BUF_LEN             1024
char RX_PAYLOAD[MAX_RX_LEN];
s_buff URX_BUF;
s_buff RX_BUF;
s_buff TEST_RX_BUF;
s_buff WINDOW;
s_buff PAYLOAD;
int SEND_PAYLOAD_FLAG = 0;
int EXTRACT_PAYLOAD_FLAG = 0;
int LISTEN_EN_FLAG = 0;
int RUN_TEST_FLAG = 0;

const char GREET[] = "\n\t**** SYSTEM INITIALIZED ****\n";
const char GREET_WAIT_CMD[]             = "\n\t$ ";
const char CMD_TIMER_START[]            = ">S1";
const char CMD_TIMER_STOP[]             = ">S0";
const char CMD_TIMER_QUERY[]            = ">SR?";
const char CMD_TIMER_SET[]              = ">SR";
const int CMD_TIMER_PARAM_INDEX = 3;
const char CMD_SEND_PAYLOAD[]           = ">TX";
const char CMD_SET_CUSTOM_PAYLOAD[]     = ">TXS";
const int CMD_SET_CUSTOM_PAYLOAD_PARAM_INDEX = 4;
const char CMD_SET_TXPIN_HIGH[]         = ">TXP1";
const char CMD_SET_TXPIN_LOW[]          = ">TXP0";
const char CMD_SET_LISTEN_ON[]          = ">L1";
const char CMD_SET_LISTEN_OFF[]         = ">L0";
const char CMD_QUERY_LISTEN[]           = ">L?";
const char CMD_GET_CONFIDENCE[]         = ">C?";
const char CMD_SET_CONFIDNECE[]         = ">C";
const int CMD_SET_CONFIDENCE_PARAM_INDEX = 2;
const char CMD_RUN_TEST[]               = ">TEST";
const char CMD_DEBUG_ENABLE[]           = ">DE1";
const char CMD_DEBUG_DISABLE[]          = ">DE0";


int PAYLOAD_PUSH_INDEX = 0;
void RIT_IRQHandler(void) {
    if(SEND_PAYLOAD_FLAG == 1) {
        if(PAYLOAD_PUSH_INDEX < PAYLOAD.bits) {
            // uprintf("TX: %i", PAYLOAD.stream[PAYLOAD_PUSH_INDEX]);
            setTx(PAYLOAD.stream[PAYLOAD_PUSH_INDEX]);
            PAYLOAD_PUSH_INDEX++;
        }
        else {
            if (ENABLE_EVENT_MSGS) uprintf("\nEVENT: PAYLOAD TRANSMITTED\n");
            SEND_PAYLOAD_FLAG = 0;
            PAYLOAD_PUSH_INDEX = 0;
        }
    }
    if(LISTEN_EN_FLAG) {
        if(RX_BUF.bits < MAX_RX_LEN - 1) {
            int bit = readRxPin();
            // uprintf("  RX: %i -- bits: %i\r\n", bit, RX_BUF.bits);
            setBlue(bit);
            pushBitToBuffer(&RX_BUF, bit);
        } 
        else {
            if (ENABLE_EVENT_MSGS) uprintf("\nEVENT: BUFFER FILLED\n");
            LISTEN_EN_FLAG = 0;
            EXTRACT_PAYLOAD_FLAG = 1;
            samplerStop();
        }
    } 
    // clear the interrupt by setting 1
    LPC_RIT->RICTRL |= (1 << RITINT);
}

void UART3_IRQHandler(void) {
    if ((LPC_UART3->IIR & U_IIR_RDA_INT_ID) == U_IIR_RDA_INT_ID) {
        if(URX_BUF.bits >= (BUF_SIZE - 1)) {
            clearBuffer(&URX_BUF);
            uprintf("\nERROR: Input buffer overrun.\r\n");
        }
        else {
            char ch = uart_rx();
            if(ch == '\n') {
                uprintf("\tACK: %s\n", URX_BUF.stream);
                if (strcmp(CMD_TIMER_START, URX_BUF.stream) == 0) {
                    samplerStart();
                }
                else if (strcmp(CMD_TIMER_STOP, URX_BUF.stream) == 0) {
                    samplerStop();
                }
                else if (strcmp(CMD_TIMER_QUERY, URX_BUF.stream) == 0) {
                    uprintf("\n\tSample rate: %i\n", getSampleRate());
                }
                else if (strstr(URX_BUF.stream, CMD_TIMER_SET) != NULL) {
                    char * tmp;
                    int newrate = strtol(&URX_BUF.stream[CMD_TIMER_PARAM_INDEX], &tmp, 10);
                    if(newrate > 0) {
                        uprintf("\tSetting rate to: %i Hz\n", newrate);
                        samplerStop();
                        samplerSetHz(newrate);
                        // samplerStart();
                    }
                    else { 
                        uprintf("\tInvalid rate: %i Hz\n", newrate);
                    }
                }
                else if (strcmp(CMD_GET_CONFIDENCE, URX_BUF.stream) == 0) {
                    uprintf("\n\tMin. Confidence: %i\n", MIN_CONFIDENCE);
                }
                else if (strstr(URX_BUF.stream, CMD_SET_CONFIDNECE) != NULL) {
                    char * tmp;
                    int min = strtol(&URX_BUF.stream[CMD_SET_CONFIDENCE_PARAM_INDEX], &tmp, 10);
                    if(min > 0) {
                        uprintf("\n\tSetting min. confidence to: %i\n", min);
                        MIN_CONFIDENCE = min;
                    }
                    else { 
                        uprintf("\n\tInvalid confidence: %i \n", min);
                    }
                }
                else if (strcmp(CMD_QUERY_LISTEN, URX_BUF.stream) == 0) {
                    uprintf("\n\tLISTENER ENABLE FLAG: %i\n", LISTEN_EN_FLAG);
                    LISTEN_EN_FLAG = 0;
                }
                else if (strcmp(CMD_SET_LISTEN_ON, URX_BUF.stream) == 0) {
                    uprintf("\n\tEnabling listener.\n");
                    LISTEN_EN_FLAG = 1;
                }
                else if (strcmp(CMD_SET_LISTEN_OFF, URX_BUF.stream) == 0) {
                    uprintf("\n\tDisabling listener.\n");
                    LISTEN_EN_FLAG = 0;
                }
                else if (strcmp(CMD_SET_TXPIN_HIGH, URX_BUF.stream) == 0) {
                    uprintf("\n\tSetting TX pin HIGH. (1)\n");
                    setTx(1);
                }
                else if (strcmp(CMD_SET_TXPIN_LOW, URX_BUF.stream) == 0) {
                    uprintf("\n\tSetting TX pin LOW   (0)\n");
                    setTx(0);
                }
                else if (strcmp(CMD_SEND_PAYLOAD, URX_BUF.stream) == 0) {
                    SEND_PAYLOAD_FLAG = 1;
                    if(IS_SAMPLING == 0) { samplerStart(); }
                }
                else if (strstr(URX_BUF.stream, CMD_SET_CUSTOM_PAYLOAD) != NULL) {
                    if(URX_BUF.stream[CMD_SET_CUSTOM_PAYLOAD_PARAM_INDEX] != '\0') {
                        clearBuffer(&PAYLOAD);
                        generateSync(&PAYLOAD, 32);
                        addPayload(&PAYLOAD, &URX_BUF.stream[CMD_SET_CUSTOM_PAYLOAD_PARAM_INDEX]);
                        uprintf("\n\tNew payload: %s\n", &URX_BUF.stream[CMD_SET_CUSTOM_PAYLOAD_PARAM_INDEX]);
                    }
                }
                else if (strcmp(CMD_RUN_TEST, URX_BUF.stream) == 0) {
                    uprintf("\n\tRunning local test.\n");
                    RUN_TEST_FLAG = 1;
                }
                else if (strcmp(CMD_DEBUG_ENABLE, URX_BUF.stream) == 0) {
                    uprintf("\n\tEnabilng debug event messages.\n");
                    ENABLE_EVENT_MSGS = 1;
                }
                else if (strcmp(CMD_DEBUG_DISABLE, URX_BUF.stream) == 0) {
                    uprintf("\n\tDisabling debug event messages.\n");
                    ENABLE_EVENT_MSGS = 0;
                }
                else {
                    uprintf("\n\tUNKNOWN COMMAND\n");
                }
                uprintf(GREET_WAIT_CMD);
                clearBuffer(&URX_BUF);
                // command is entered
            }
            else if (ch != '\r') {
                pushCharToBuffer(&URX_BUF, ch);
            }
        }
    }
}

int state = 0;
void EINT3_IRQHandler() {
    if(IS_SAMPLING == false && LISTEN_EN_FLAG == 1) {
        if (ENABLE_EVENT_MSGS) uprintf("\nEVENT: RX PIN TRIGGERED. STARTING SAMPLER\n");
        samplerStart();
    }
    state = state ? 0 : 1;
    // setBlue(state);
    clearRxInterrupt();
}

void initializeBuffers() {
    initBuffer(&WINDOW);
    generateSync(&WINDOW, 32);

    initBuffer(&PAYLOAD);
    generateSync(&PAYLOAD, 32);
    addPayload(&PAYLOAD, "ANDREW5295\n");

    initBuffer(&TEST_RX_BUF);
    addPayload(&TEST_RX_BUF, "SOMEFILLER\n");
    generateSync(&TEST_RX_BUF, 32);
    addPayload(&TEST_RX_BUF, "TESTANDREW5295\n");

    initBuffer(&RX_BUF);
    initBuffer(&URX_BUF);
}
int main(void)
{
    SystemInit();
    SystemCoreClockUpdate();
    initializeBuffers();
    initGpio();
    initLeds();
    uart_init(9600);
    samplerInit(2000);
    samplerStop();
    NVIC_EnableIRQ(UART3_IRQn);
    NVIC_EnableIRQ(RIT_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);
    uprintf(GREET);
    uprintf("\nPAYLOAD SIZE: %i\n", PAYLOAD.bits);
    uprintf(GREET_WAIT_CMD);

    int payloadIndex = 0;
    while(1) {
        if(EXTRACT_PAYLOAD_FLAG == 1 || RUN_TEST_FLAG == 1) {
            if(RUN_TEST_FLAG == 1) {
                payloadIndex = scanForMatch(&WINDOW, &TEST_RX_BUF, SYNC_BYTES, MIN_CONFIDENCE);
                if(payloadIndex > 0) {
                    uprintf("\n\tPAYLOAD @ %i\n", payloadIndex);
                    extractPayload(&TEST_RX_BUF, RX_PAYLOAD, payloadIndex, MAX_RX_LEN);
                    uprintf("\tPAYLOAD: %s\n", RX_PAYLOAD);
                }
                else {
                    uprintf("\n\tPAYLOAD INDEX INVALID: %i\n", payloadIndex);
                }
                if (ENABLE_EVENT_MSGS) uprintf("\nEVENT: Test is done.\n");
                RUN_TEST_FLAG = 0;
                payloadIndex = 0;
            }
            else {
                memset(RX_PAYLOAD, 0, MAX_RX_LEN);
                if (ENABLE_EVENT_MSGS) uprintf("\nEVENT: BEGINNING EXTRACTION\n");
                payloadIndex = scanForMatch(&WINDOW, &RX_BUF, SYNC_BYTES, MIN_CONFIDENCE);
                if(payloadIndex > 0) {
                    uprintf("\n\tPAYLOAD @ %i\n", payloadIndex);
                    extractPayload(&RX_BUF, RX_PAYLOAD, payloadIndex, MAX_RX_LEN);
                    uprintf("\tPAYLOAD: %s", RX_PAYLOAD);
                }
                else {
                    uprintf("\n\tPAYLOAD INDEX INVALID: %i\n", payloadIndex);
                }
                if (ENABLE_EVENT_MSGS) uprintf("\nEVENT: EXTRACTION DONE. CLEARING RX BUFFER\n");
                clearBuffer(&RX_BUF);
                payloadIndex = 0;
                EXTRACT_PAYLOAD_FLAG = 0;
                uprintf(GREET_WAIT_CMD);
                LISTEN_EN_FLAG = 1;
            }
        }
        // rxChar = uart_rx();
        // uprintf("I've received: %c\n", rxChar);
        // uart_tx(rxChar);
    }
    return 0 ;
}
