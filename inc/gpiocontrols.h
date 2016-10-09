#ifndef _GPIO_CONTROLS_
#define _GPIO_CONTROLS_

#include "LPC17xx.h"

#define PINSEL_PIN_22 22    // P0.22 red led 
#define PINSEL_PIN_25 25    // P3.25 green led
#define PINSEL_PIN_26 26    // P3.26 blue led

#define LED_GREEN   PINSEL_PIN_25
#define LED_BLUE    PINSEL_PIN_26
#define LED_RED     PINSEL_PIN_22

#define TX_PIN 6    // Pin 2.6
#define RX_PIN 7    // Pin 2.7

void initGpio();
void initLeds();
void setLeds(int i);
void setTx(int i);
void setBlue(int i);
void clearRxInterrupt();


#endif