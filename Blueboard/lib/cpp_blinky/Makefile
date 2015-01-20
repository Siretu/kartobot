##############################################################################
# Product: BLINKY/C++ example application
# Date of the Last Update:  Jan 13, 2010
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2007-2010 Quantum Leaps, LLC. All rights reserved.
#
# Contact information:
# Quantum Leaps Web site:  http://www.quantum-leaps.com
# e-mail:                  info@quantum-leaps.com
##############################################################################

#-----------------------------------------------------------------------------
# tools
#
ifeq ($(GNU_ARM),)
GNU_ARM = C:/tools/devkitPro/devkitARM/bin
endif

CC    := $(GNU_ARM)/arm-eabi-g++
ASM   := $(GNU_ARM)/arm-eabi-as
LINK  := $(GNU_ARM)/arm-eabi-g++
BIN   := $(GNU_ARM)/arm-eabi-objcopy
RM    := rm -rf
MKDIR := mkdir

BLDDIR  = .

CCINC   = -I$(BLDDIR)

APP_DEP = $(BLDDIR)/bsp.h \
	$(BLDDIR)/arm_exc.h \
	$(BLDDIR)/isr.h

APP_NAME = blinky
ARM_CORE = arm7tdmi


ifeq (rel, $(CONF))       # Release configuration ............................

BINDIR = rel

CCFLAGS = -c -mcpu=$(ARM_CORE) -mthumb-interwork -Os \
	-fno-rtti -fno-exceptions \
	-mlong-calls -ffunction-sections -Wall -DNDBEBUG -o$@

ASMFLAGS = -mcpu=$(ARM_CORE) -mthumb-interwork -o$@

LINKFLAGS = -T .\$(APP_NAME).ld -o $(BINDIR)\$(APP_NAME).elf \
	-Wl,-Map,$(BINDIR)\$(APP_NAME).map,--cref,--gc-sections

else                     # default Debug configuration .......................

BINDIR = dbg

CCFLAGS = -g -c -mcpu=$(ARM_CORE) -mthumb-interwork -O \
	-fno-rtti -fno-exceptions \
	-mlong-calls -ffunction-sections -Wall -o$@

ASMFLAGS = -g -mcpu=$(ARM_CORE) -mthumb-interwork -o$@

LINKFLAGS = -T .\$(APP_NAME).ld -o $(BINDIR)\$(APP_NAME).elf \
	-Wl,-Map,$(BINDIR)\$(APP_NAME).map,--cref,--gc-sections

endif

#.............................................................................

all: $(BINDIR)/$(APP_NAME).bin

$(BINDIR)/$(APP_NAME).bin : $(BINDIR)/$(APP_NAME).elf
	$(BIN) -O binary $(BINDIR)/$(APP_NAME).elf $(BINDIR)/$(APP_NAME).bin

$(BINDIR)/$(APP_NAME).elf : \
	./$(APP_NAME).ld \
	$(BINDIR)/startup.o \
	$(BINDIR)/arm_exc.o \
	$(BINDIR)/low_level_init.o \
	$(BINDIR)/isr.o \
	$(BINDIR)/bsp.o \
	$(BINDIR)/blinky.o \
	$(BINDIR)/mini_cpp.o \
	$(BINDIR)/no_heap.o
	$(LINK) \
	$(BINDIR)/startup.o \
	$(BINDIR)/arm_exc.o \
	$(BINDIR)/low_level_init.o \
	$(BINDIR)/isr.o \
	$(BINDIR)/bsp.o \
	$(BINDIR)/blinky.o \
	$(BINDIR)/mini_cpp.o \
	$(BINDIR)/no_heap.o \
	$(LINKFLAGS)


$(BINDIR)/startup.o: $(BLDDIR)/startup.s
	$(ASM) $(ASMFLAGS) $<

$(BINDIR)/arm_exc.o: $(BLDDIR)/arm_exc.s
	$(ASM) $(ASMFLAGS) $<


# choose the ARM or THUMB compilation for each module...

$(BINDIR)/low_level_init.o: $(BLDDIR)/low_level_init.cpp $(APP_DEP)
	$(CC) -marm $(CCFLAGS) $(CCINC) $<

$(BINDIR)/isr.o: $(BLDDIR)/isr.cpp $(APP_DEP)
	$(CC) -marm $(CCFLAGS) $(CCINC) $<

$(BINDIR)/bsp.o: $(BLDDIR)/bsp.cpp $(APP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)/blinky.o: $(BLDDIR)/blinky.cpp $(APP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)/mini_cpp.o: $(BLDDIR)/mini_cpp.cpp
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)/no_heap.o: $(BLDDIR)/no_heap.cpp
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<


.PHONY : clean
clean:
	-$(RM) $(BINDIR)/*.o \
	$(BINDIR)/*.bin \
	$(BINDIR)/*.elf \
	$(BINDIR)/*.map
