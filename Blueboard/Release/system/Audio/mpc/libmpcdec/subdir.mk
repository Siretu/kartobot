# Add inputs and outputs from these tool invocations to the build variables 
SRC += \
../system/Audio/mpc/libmpcdec/huffman.c \
../system/Audio/mpc/libmpcdec/mpc_bits_reader.c \
../system/Audio/mpc/libmpcdec/mpc_decoder.c \
../system/Audio/mpc/libmpcdec/mpc_demux.c \
../system/Audio/mpc/libmpcdec/requant.c \
../system/Audio/mpc/libmpcdec/streaminfo.c \
../system/Audio/mpc/libmpcdec/synth_filter.c

COBJ += \
./system/Audio/mpc/libmpcdec/huffman.o \
./system/Audio/mpc/libmpcdec/mpc_bits_reader.o \
./system/Audio/mpc/libmpcdec/mpc_decoder.o \
./system/Audio/mpc/libmpcdec/mpc_demux.o \
./system/Audio/mpc/libmpcdec/requant.o \
./system/Audio/mpc/libmpcdec/streaminfo.o \
./system/Audio/mpc/libmpcdec/synth_filter.o

#CPPSRC += \

#CPPOBJ += \
