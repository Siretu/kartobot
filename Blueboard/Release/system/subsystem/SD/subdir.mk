# Add inputs and outputs from these tool invocations to the build variables 
SRC += \
../system/subsystem/SD/diskio.c \
../system/subsystem/SD/fattime.c \
../system/subsystem/SD/ff_test_term.c \
../system/subsystem/SD/ff.c \
../system/subsystem/SD/monitor.c \
../system/subsystem/SD/spi_sd_lpc17xx.c

COBJ += \
./system/subsystem/SD/diskio.o \
./system/subsystem/SD/fattime.o \
./system/subsystem/SD/ff_test_term.o \
./system/subsystem/SD/ff.o \
./system/subsystem/SD/monitor.o \
./system/subsystem/SD/spi_sd_lpc17xx.o