# Add inputs and outputs from these tool invocations to the build variables 
SRC += \
../system/FreeRTOS/heap_2.c \
../system/FreeRTOS/heap_3.c \
../system/FreeRTOS/list.c \
../system/FreeRTOS/port.c \
../system/FreeRTOS/queue.c \
../system/FreeRTOS/tasks.c

COBJ += \
./system/FreeRTOS/heap_2.o \
./system/FreeRTOS/heap_3.o \
./system/FreeRTOS/list.o \
./system/FreeRTOS/port.o \
./system/FreeRTOS/queue.o \
./system/FreeRTOS/tasks.o