#ifndef _UART_CONTROLS_
#define _UART_CONTROLS_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "LPC17xx.h"

#if !defined(CHECK_BIT)
    #define CHECK_BIT(var, pos) ((var) & (1<<(pos)))
#endif

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

#define MAX_PRINT_LENGTH   1024

void uart_init(uint32_t baud);

void uart_tx(char ch);

char uart_rx();
void uprintf(const char * fmt, ...);
#endif