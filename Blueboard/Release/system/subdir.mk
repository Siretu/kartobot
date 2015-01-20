# Add inputs and outputs from these tool invocations to the build variables 
SRC += \
../system/Display.c \
../system/EEPROM.c \
../system/faults.c \
../system/Power.c \
../system/sonar.c \
../system/status.c \
../system/USB.c

COBJ += \
./system/Display.o \
./system/faults.o \
./system/Power.o \
./system/sonar.o \
./system/status.o \
./system/USB.o

CPPSRC += \
../system/Xbee.cpp

CPPOBJ += \
./system/Xbee.o
