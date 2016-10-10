#include "gpiocontrols.h"


void initLeds() {
    LPC_PINCON->PINSEL1 &= (~(3 << 12));
    LPC_PINCON->PINSEL4 &= (~(3 << 12));    // set Pin 2.6 to GPIO
    LPC_PINCON->PINSEL4 &= (~(3 << 14));    // set Pin 2.7 to GPIO
}

void setLeds(int i) {
    if (i == 1) {
        // LPC_GPIO2->FIOSET |= (1 << TX_PIN);
        LPC_GPIO0->FIOSET |= (1 << LED_RED);
        LPC_GPIO3->FIOSET |= (1 << LED_GREEN);
        LPC_GPIO3->FIOSET |= (1 << LED_BLUE);
    }
    else {
        // LPC_GPIO2->FIOCLR |= (1 << TX_PIN);
        LPC_GPIO0->FIOCLR |= (1 << LED_RED);
        LPC_GPIO3->FIOCLR |= (1 << LED_GREEN);
        LPC_GPIO3->FIOCLR |= (1 << LED_BLUE);
    }
}

void setTx(int i) {
    if (i == 1) { LPC_GPIO2->FIOSET |= (1 << TX_PIN); }
    else        { LPC_GPIO2->FIOCLR |= (1 << TX_PIN); }
}

void setBlue(int i) {
    if (i == 1) { LPC_GPIO3->FIOSET |= (1 << LED_BLUE); }
    else        { LPC_GPIO3->FIOCLR |= (1 << LED_BLUE); }
}

void initGpio(void) {
    LPC_PINCON->PINSEL1 &= (~(3 << 12));
    LPC_PINCON->PINSEL1 &= (~(3 << 12));
    // Set GPIO - P0_22 - to be output
    LPC_GPIO0->FIODIR |= (1 << PINSEL_PIN_22);
    LPC_GPIO3->FIODIR |= (1 << PINSEL_PIN_26);

    LPC_GPIO2->FIODIR |= (1 << TX_PIN);

    // set RX input
    LPC_GPIO2->FIODIR &= ~(1 << RX_PIN);
    LPC_GPIOINT->IO2IntEnR |= (1 << RX_PIN);    // enable rising edge interrupt
    LPC_GPIOINT->IO2IntEnF |= (1 << RX_PIN);    // enable falling edge interrupt
}

int readRxPin() {
    return (LPC_GPIO2->FIOPIN0 & ( 1 << RX_PIN) ? 1 : 0);
}

void clearRxInterrupt() {
    LPC_GPIOINT->IO2IntClr |= (1 << RX_PIN);
}