#ifndef _SAMPLING_TIMER_
#define _SAMPLING_TIMER_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "LPC17xx.h"

#define RITINT   0
#define RITENCLR 1
#define RITEN    3
#define PCRIT    16

void samplerSetHz(int desiredHz);
void samplerInit(int desiredHz);
void samplerStart();
void samplerStop();

#endif