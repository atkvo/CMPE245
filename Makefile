# put your *.o targets here, make should handle the rest!

SRCS =   \
	 system_LPC17xx.c \
	 newlibstubs.c \
	 startup_LPC17xx.c \
	 uart_controls.c \
	 samplingtimer.c \
	 gpiocontrols.c \
	 engine.c \
 	 main.c
 
	 
# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

PROJ_NAME=tx_test
LPC_CORE=$(HOME)/dev/arm/LPC17XX_CORE
LAB=lab1

# that's it, no need to change anything below this line!

###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
OBJSIZE=arm-none-eabi-size

CFLAGS  = -g3  -O0 -Wall -Tlpc17xx.ld
# Define the device we are using
CFLAGS += -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))"
CFLAGS += -D PACK_STRUCT_END=__attribute\(\(packed\)\) 
CFLAGS += -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\)	
CFLAGS += -D__USE_CMSIS
CFLAGS += -mthumb -mcpu=cortex-m3 
CFLAGS += -fno-builtin -mfloat-abi=soft	-ffunction-sections -fdata-sections -fmessage-length=0 -funsigned-char
 
ODFLAGS	= -x
LDFLAGS += -Wl,-Map,$(PROJ_NAME).map,-L=$(LPC_CORE)/Drivers

###################################################

vpath %.c src
vpath %.c engine/
vpath %.c $(LPC_CORE)/Core/Src 
vpath %.c $(LPC_CORE)/Drivers/Src

ROOT=$(shell pwd)

CFLAGS += -Iinc 
CFLAGS += -Iengine/ 
CFLAGS += -I$(LPC_CORE)/Inc 
CFLAGS += -I$(LPC_CORE)/Core/Inc
CFLAGS += -I$(LPC_CORE)/Drivers/Inc
CFLAGS += -I$(LPC_CORE)/Drivers

LIBS = -L$(LPC_CORE)/Drivers -llpcdriver
# LIBS = -LDrivers

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: Drivers proj

all: Drivers proj

Drivers:
	$(MAKE) -C $(LPC_CORE)/Drivers/
	
proj: 	$(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(LDFLAGS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	$(OBJDUMP) -x $(PROJ_NAME).elf > $(PROJ_NAME).dmp
	@echo " "
	@$(OBJSIZE) -d $(PROJ_NAME).elf

clean:
	$(MAKE) -C $(LPC_CORE)/Drivers clean
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	rm -f $(PROJ_NAME).dmp
	rm -f $(PROJ_NAME).map

ocdstart:
	sudo openocd -c "interface cmsis-dap" -f embedded-artists-lpc1769.cfg	
	# sudo openocd -c "interface cmsis-dap" -f /usr/local/share/openocd/scripts/target/lpc17xx.cfg	

ocdconnect:
	telnet localhost 4444

gdbconnect:
	arm-none-eabi-gdb --eval-command="target remote localhost:3333 && tui enable" $(PROJ_NAME).elf

pushtobranch:
	git push origin master:$(LAB)

print-%: ; @echo $*=$($*)
