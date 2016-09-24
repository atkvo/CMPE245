# LPC1769

## Modifications
Extracted the libraries from vpcola's repository to a separate git repo so I don't need to copy over entire libraries just to start a new project. Makefile has been modified to support this. 

Boiler plate code intended for LPC1769 

## Original README
https://github.com/vpcola/LPC1769

Bare metal build for LPC1769 projects

This is my own project template for developing on the LPC1769/68 microcontroller. It uses a slightly modified linker script 
and startup code for building on GCC (arm-none-eabi) tools. This example shows a blinking LED on the Embedded Artists LPC1769 board.

I believe this can also be ported to other boards supporting the LPC1769/68 microcontrollers.

Loading the binary to the board is done via openocd. An openocd board cfg file is provided for flashing the Embedded Artists LPC1769 
board (CMSIS-DAP transport) and the MBED LPC1768 board.
