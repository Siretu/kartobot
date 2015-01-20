# Add inputs and outputs from these tool invocations to the build variables 
SRC += \
../system/subsystem/USB/cdcuser.c \
../system/subsystem/USB/DiskImg.c \
../system/subsystem/USB/mscuser.c \
../system/subsystem/USB/usbcore.c \
../system/subsystem/USB/usbdesc.c \
../system/subsystem/USB/usbhw.c \
../system/subsystem/USB/usbuser.c

COBJ += \
./system/subsystem/USB/cdcuser.o \
./system/subsystem/USB/DiskImg.o \
./system/subsystem/USB/mscuser.o \
./system/subsystem/USB/usbcore.o \
./system/subsystem/USB/usbdesc.o \
./system/subsystem/USB/usbhw.o \
./system/subsystem/USB/usbuser.o