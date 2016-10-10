#include "samplingtimer.h"
void samplerSetHz(int desiredHz) {
    // default PCLK is SystemCoreClock/4
    int timerHz = SystemCoreClock/4;
    int countsRequired = timerHz/desiredHz;
    LPC_RIT->RICOMPVAL = countsRequired;
}
void samplerInit(int desiredHz){
    LPC_SC->PCONP |= (1 << PCRIT);
    LPC_RIT->RICTRL |= (1 << RITINT);
    samplerSetHz(desiredHz);
}

void samplerStart() {
    LPC_RIT->RICOUNTER = 0;
    LPC_RIT->RICTRL |= (1 << RITEN);
    LPC_RIT->RICTRL |= (1 << RITENCLR);
}

void samplerStop() {
    LPC_RIT->RICTRL &= ~(1 << RITEN);
    LPC_RIT->RICTRL &= ~(1 << RITENCLR);
}

int getSampleRate() {
    if (LPC_RIT->RICOMPVAL > 0) {
        int timerHz = SystemCoreClock/4;
        return (timerHz/LPC_RIT->RICOMPVAL);
    }
    else {
        return 0;
    }
}