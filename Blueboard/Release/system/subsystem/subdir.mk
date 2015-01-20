# Add inputs and outputs from these tool invocations to the build variables 
SRC += \
../system/subsystem/Accel.c \
../system/subsystem/DMA.c \
../system/subsystem/EEPROM.c \
../system/subsystem/Gyro.c \
../system/subsystem/I2C.c \
../system/subsystem/Magneto.c \
../system/subsystem/motor.c \
../system/subsystem/Servo.c \
../system/subsystem/Time2.c \
../system/subsystem/UART.c \
../system/subsystem/Wheels.c

COBJ += \
./system/subsystem/Accel.o \
./system/subsystem/DMA.o \
./system/subsystem/EEPROM.o \
./system/subsystem/Gyro.o \
./system/subsystem/I2C.o \
./system/subsystem/Magneto.o \
./system/subsystem/motor.o \
./system/subsystem/Servo.o \
./system/subsystem/Time2.o \
./system/subsystem/UART.o \
./system/subsystem/Wheels.o

CPPSRC += \
../system/subsystem/XBeeLib.cpp

CPPOBJ += \
./system/subsystem/XBeeLib.o
