################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../EEPROM.cpp \
../main.cpp \
../twi.cpp 

C_SRCS += \
../ADC.c \
../PWM.c \
../bar.c \
../matrix.c \
../sleep.c \
../time.c \
../utils.c 

OBJS += \
./ADC.o \
./EEPROM.o \
./PWM.o \
./bar.o \
./main.o \
./matrix.o \
./sleep.o \
./time.o \
./twi.o \
./utils.o 

C_DEPS += \
./ADC.d \
./PWM.d \
./bar.d \
./matrix.d \
./sleep.d \
./time.d \
./utils.d 

CPP_DEPS += \
./EEPROM.d \
./main.d \
./twi.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\Users\exuvo\workspace\LED Matrix\lib" -I"C:\Users\exuvo\workspace\LED Matrix\lib\arduino" -Wall -Os -fpack-struct -fshort-enums -fdata-sections -ffunction-sections -fno-inline-small-functions -ffreestanding -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega8 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++  -I"C:\Users\exuvo\workspace\LED Matrix\lib" -I"C:\Users\exuvo\workspace\LED Matrix\lib\arduino" -Wall -Os -fpack-struct -fshort-enums -fdata-sections -ffunction-sections -fno-inline-small-functions -ffreestanding -std=gnu++98 -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega8 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


