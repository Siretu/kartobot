

FreeRTOS example for LMI L3S811 on EK-LM3S811
using the GNU Toolchain (gcc et al)

by Martin Thomas, Kaiserslautern, Germany 
<eversmith@heizung-thomas.de>
http://www.siwawi.arubi.uni-kl.de/avr_projects

Version 20061114

---------------------------------------------------------------


Versions history

20061114 - mt - * pathes and filenames Unix/Linux-compatible
                * attribute for ISR's (see Codesourcery readme pdf)
20061025 - mt - Intial release

---

FreeRTOS (www.freertos.org) offers support for the LMI LM3S811 
Cortex-M3 controller. In the FreeRTOS Version 4.1.1 there has
been no example for the GNU-toolchain (gcc et al) for this
target and the LMI evaluation-board (but an example for the
RealView Toolchain as in Keil uVision). 

This package includes an adapted version of the demo-application
which can be built with the GNU-toolchain. I did not change
much in the code:

- makefile based on the WinARM-template has been added
- "generic" startup_gcc which has already been used in 
  my ports of the examples has been added and adapted
- linker-script has been added
- makefile for the LMI driver-library is included
- uVision project-workspace for "command-line-flashing" is
  included
- changed some path-separators from backslash to slash
- upper-/lower-case in includes
- added attribute(interrupt) for ISRs as recommended
  in the Codesourcery readme-file


Used Software:

- FreeRTOS V4.1.1 (needed parts included in this package)
- LMI Driver-Lib Version 852 (included in this package)

- Codesourcery GNU-toolchain for ARM Version 2006q3-26 
  (available for download from Codesourcery)
- Support utilites: make, sh etc. from WinARM (in WinARM/utils/bin)
  or WinAVR (WinAVR/utils/bin)
  Of cause other utilities can be used too (i.e. from MinGW,
  Cygwin, unxutils etc.). Just make, sh, echo, rm should do.
  (WinARM is available from the "siwawi-URL" given above).
  Unix/Linux users will have the utils already installed
- Keil uVision for Flash-Programming (Win32 only)
  (the evaluation-version will do, available from www.keil.com)
  Update: alternatives OpenOCD (multiplattform) or lmiflash (Win32 only)


Step-By-Step
------------

Install the Codesourcery Toolchain, verify that the
compiler-frontend is in the systems search-path. For example 
like this:
  
C:\WinARM\examples\lm3s811_FreeRTOS_gcc>path
PATH=C:\Programme\CodeSourcery\Sourcery G++\bin;C:\Programme[...]

C:\WinARM\examples\lm3s811_FreeRTOS_gcc>arm-none-eabi-gcc --version
arm-none-eabi-gcc (GCC) 4.1.0 (CodeSourcery ARM)
[...]
  
---

Make sure the build utilites are availble and in the search-path:

C:\WinARM\examples\lm3s811_FreeRTOS_gcc>path
PATH=[...]C:\WinARM\utils\bin[...]

C:\WinARM\examples\lm3s811_FreeRTOS_gcc>sh --version
GNU bash, version 2.04.0(1)-release (i686-pc-msys)
Copyright 1999 Free Software Foundation, Inc.

C:\WinARM\examples\lm3s811_FreeRTOS_gcc>make --version
GNU Make 3.81
[...]

--- 

Install Keil's MDK-ARM if you want to use it's flash-programming
functions to upload the code to the eval.-board 

---

Build the LMI driver-library (libluminary.a). A makefile is included
so a simple make lib in the Demo\CORTEX_LM3S811_GCC\LuminaryCode\src-
directory will do.

C:\WinARM\examples\lm3s811_FreeRTOS_gcc\Demo\CORTEX_LM3S811_GCC\LuminaryCode\src
>make lib
arm-none-eabi-gcc -c -mcpu=cortex-m3 -I. -gdwarf-2 -DBUILD_ALL -Os -Wall -Wstric
t-prototypes -Wundef -Wa,-adhlns=adc.lst  -std=gnu99 -ffunction-sections -fdata-
[...]
arm-none-eabi-gcc -c -mcpu=cortex-m3 -I. -x assembler-with-cpp -Wa,-adhlns=cpu.l
st,-gdwarf-2 -mthumb -Dgcc -DBUILD_ALL cpu.S -o cpu.o
arm-none-eabi-ar rcs ../libluminary.a adc.o comp.o flash.o gpio.o i2c.o interrup
t.o pwm.o ssi.o sysctl.o systick.o timer.o uart.o watchdog.o cpu.o

---

Build the example application

C:\WinARM\examples\lm3s811_FreeRTOS_gcc\Demo\CORTEX_LM3S811_GCC>make all

-------- begin (mode: ROM_RUN) --------
arm-none-eabi-gcc (GCC) 4.1.0 (CodeSourcery ARM)
Copyright (C) 2006 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


Compiling C: main.c
arm-none-eabi-gcc -c -mthumb -mcpu=cortex-m3 -mthumb-interwork -I. -gdwarf-2 -DR
OM_RUN -Dgcc -DGCC_ARMCM3_LM3S102 -D__WinARM__ -D__WINARMSUBMDL_lm3s811__  -O2 -
ffunction-sections -fdata-sections -Wall -Wcast-align -Wimplicit  -Wpointer-arit
h -Wswitch -Wredundant-decls -Wreturn-type -Wshadow -Wunused -Wa,-adhlns=main.ls
t  -ILuminaryCode -ILuminaryCode/src -I../../Source/include -I../Common/include
-I../../Source/portable/GCC/ARM_CM3/ -Wcast-qual -MD -MP -MF .dep/main.o.d -Wnes
ted-externs  -std=gnu99 -Wmissing-prototypes  -Wstrict-prototypes -Wmissing-decl
arations main.c -o main.o
[...]

Linking: main.elf
arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -mthumb-interwork -I. -gdwarf-2 -DROM_
RUN -Dgcc -DGCC_ARMCM3_LM3S102 -D__WinARM__ -D__WINARMSUBMDL_lm3s811__  -O2 -ffu
nction-sections -fdata-sections -Wall -Wcast-align -Wimplicit  -Wpointer-arith -
Wswitch -Wredundant-decls -Wreturn-type -Wshadow -Wunused -Wa,-adhlns=main.lst
-ILuminaryCode -ILuminaryCode/src -I../../Source/include -I../Common/include -I.
./../Source/portable/GCC/ARM_CM3/ -Wcast-qual -MD -MP -MF .dep/main.elf.d    mai
n.o startup_gcc.o LuminaryCode/osram96x16.o ../Common/Minimal/semtest.o ../Commo
n/Minimal/integer.o ../Common/Minimal/PollQ.o ../Common/Minimal/BlockQ.o ../../S
ource/tasks.o ../../Source/list.o ../../Source/queue.o ../../Source/portable/Mem
Mang/heap_2.o ../../Source/portable/GCC/ARM_CM3/port.o     --output main.elf -no
startfiles -Wl,-Map=main.map,--cref,--gc-sections -lc  -lm -lc -lgcc   -LLuminar
yCode -lluminary -T./lm3s811-ROM.ld

Creating load file for Flash: main.hex
arm-none-eabi-objcopy -O ihex main.elf main.hex

Creating Extended Listing: main.lss
arm-none-eabi-objdump -h -S -D main.elf > main.lss

Creating Symbol Table: main.sym
arm-none-eabi-nm -n main.elf > main.sym

Size after:
main.elf  :
section            size        addr
.text              9120           0
.data                 4   536870912
.bss               7276   536870916
.stackarea          256   536878192
.comment            629           0
.debug_aranges     2136           0
.debug_pubnames    4333           0
.debug_info       16204           0
.debug_abbrev      5102           0
.debug_line        7218           0
.debug_frame       4564           0
.debug_str         7377           0
.debug_loc         9441           0
.debug_ranges       392           0
.ARM.attributes      16           0
Total             74068


---

Upload the load-file main.hex to the LMI controller. The package
includes a "project-workspace" for uVision and a program-target
is in the makefile. Just type make program

C:\WinARM\examples\lm3s811_FreeRTOS_gcc\Demo\CORTEX_LM3S811_GCC>make program

Programming LM3S811 with uVision
C:\Keil\uv3\Uv3.exe -f uvisionflash811.uv2 -ouvisionflash.txt

uVision will start and transfers the contents of main.hex 
into the controller's flash-memory using the on-board JTAG-
interface of the evaluation-board.

---

Press the reset-key on the evaluation-eoard to start the
FreeRTOS-Demo.

---

That's it for now.

Have fun,
Martin Thomas
