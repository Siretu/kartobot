# Makefile to create the Luminary Micro Library 
# by Martin Thomas, Kaiserslautern, Germany
#
# party based on the WinAVR and WinARM template makefiles
#

# using the Codesourcery Q1-2006 ARM-Toolchain

TCHAIN = arm-none-eabi

CPU = cortex-m3

# library-filename will be libluminary.a
TARGET =luminary

# C-Sources
SRC  = adc.c   comp.c   flash.c 
SRC += gpio.c  i2c.c    interrupt.c pwm.c 
SRC += qei.c   ssi.c    sysctl.c    systick.c
SRC += timer.c uart.c   watchdog.c
# Assembler-Sources
ASRC = cpu.S


OPT = s
DEBUG = dwarf-2
EXTRAINCDIRS = 
CSTANDARD = -std=gnu99
CDEFS = -DBUILD_ALL

CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS)
CFLAGS += -O$(OPT)
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wa,-adhlns=$(<:%.c=%.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)
# prepare for using GCC 4 unused code removal
CFLAGS += -ffunction-sections -fdata-sections
# must be thumb for cortex-m3
CFLAGS += -mthumb

ASFLAGS = -Wa,-adhlns=$(<:%.S=%.lst),-g$(DEBUG)
# must be thumb for cortex-m3
ASFLAGS += -mthumb -Dgcc -DBUILD_ALL

SHELL = sh
CC = $(TCHAIN)-gcc
AR = $(TCHAIN)-ar

#OBJCOPY = $(TCHAIN)-objcopy
#OBJDUMP = $(TCHAIN)-objdump
#SIZE = $(TCHAIN)-size
#NM = $(TCHAIN)-nm

REMOVE = rm -f
REMOVEDIR = rm -rf

# Define all object files.
OBJ = $(SRC:%.c=%.o) $(ASRC:%.S=%.o) 

# Define all listing files.
LST = $(SRC:%.c=%.lst) $(ASRC:%.S=%.lst) 

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mcpu=$(CPU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mcpu=$(CPU) -I. -x assembler-with-cpp $(ASFLAGS)

# target for make all -> just create go to target lib
all: lib

%.o : %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

%.o : %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@
	
lib: $(OBJ)
	$(AR) rcs ../lib$(TARGET).a $(OBJ)

# Target: clean project.
clean :
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) ../lib$(TARGET).a
	$(REMOVEDIR) .dep


# Create object files directory
# $(shell mkdir $(OBJDIR) 2>/dev/null)


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all build clean lib
