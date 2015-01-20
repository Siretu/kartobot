

Using the GNU-Toolchain with the Luminary LM3S811 Cortex-M3

by Martin Thomas, Kaiserslautern, Germany 
<eversmith@heizung-thomas.de>
http://www.siwawi.arubi.uni-kl.de/avr_projects


Version 20061111


-----------------------------------------------------------

Version-History
---------------
20061011 - Initial release
20061013 - Included EULA.txt as requested by Luminary Micro.
           The driver-library and the examples are covered by 
           this "EULA".
           All modifications and extension I made are free
           but of cause I can give no warranty.
20061014 - * Fixed wrong array-index in game-demo (scroll)
           * modified code to avoid warnings from gcc at
             high warning-level
20061024 - * Update LMI library to version 852
           * Moved flash-programming-config and linker-script
             to avoid duplicate files
           * libluminary.a now located in package "root" directory
           * changed directory-name src->lib_src
           * Small modifications in the makefiles
           * Update of this readme
20061102 - * Update to LMI library version 920
           * added some prototypes and statics to avoid warnings
           * Sync example-source with files from the library
           * Changed directory-name src_lib back to src to avoid
             changes in the original LMI example-sources
           * Disabled -Wcast-align. The LMI code is not clean 
             in the game-example (cast a char* to unsigned long*)
             but I don't have the time to "fix" this every time 
             when LMI releases a new library. But I don't like 
             warnings too...
           * Separated makefiles for examples. The Makefile in the
             subdirectories just defines the source-files and includes
             everything else from the file examples.mk.
           * Added "my" version of startup_gcc to additional
             examples from the LMI sources and included them.
           * search for comments mthomas so find my modifications
           * Update of this readme
20061111 - * update to LMI libarary version 935
           * tested with arm-none-eabi-gcc (CodeSourcery ARM Sourcery 
             G++ 2006q3-26) 4.1.1. I will no longer use or test with 
             older versions of the Codesourcery toolchain.
           * Modified ISR-declarations according to Codesourcery-readme 
             (attribute interrupt, see appendix below).
           * added "multiprogrammer"-support: makefiles not prepared to
             use OpenOCD (beta with CM3-support), lmiflash or uVision
             as flash-programming tool
           * included OpenOCD binary and lmiflash
           * Updated this readme
			 

Used Hardware
-------------

Luminary Micro Evaluation Kit EV-LM3S811 (a.k.a. EVB-LM3S811, a.k.a.
EKK-LM3S811) with LM3S811 controller (ARMv7/Cortex-M3-core).


Used Software
-------------

* Codesourcery arm-none-eabi GNU-Toolchain
  WinARM in Version 20060606 does not offer ARMv7/Cortex-M3-
  support since the gcc-sources of version 4.1.1 which have been
  used to build the binaries in WinARM do not support this. 
  So the Codesourcery-toolchain has been used:
  - Used for testing: 2006q3-26-release ("CSL"-gcc 4.1.1)
    Download available from http://www.codesourcery.com
  - install the toolchain (setup included)
  - verify that the PATH environment-variable 
    includes an entry for the Codesourcery toolchain i.e.
    C:\>PATH
    C:\Programme\CodeSourcery\Sourcery G++>path
    PATH=C:\Programme\CodeSourcery\Sourcery G++\bin;C:\Progra...

--

  FOR flash-prgramming:
   
* OpenOCD development branch cortex_m3
  Flash-programming of the LM3S811 (and a lot more) can be done with OpenOCD 
  now. OpenOCD is "multiplattform" and also available for non MS-Windows systems
  (Linux, BSD/MAC OSX etc.). For further details see
  http://www.siwawi.arubi.uni-kl.de/avr_projects/arm_projects/openocd_cortex/
  An Win32-executable of openocd compiled from the OpenOCD cortex_m3 branch 
  SVN113 is included in this package (directory tools/). OpenOCD can also be 
  used as gdb-server.
  
  OR:

* lmiflash (Win32 only)
  lmiflash is available from the Luminary web-site. A copy is included in this 
  package (directory tools/).

  OR:

* Keil/ARM uVision/MDK303Beta2 (Win32 only)
  - from the LM3S811 Evaluation Kit CD version 1.01 
    Downlaod from http://www.luminarymicro.com 
  - install the MDK (setup included) 
  ONLY for flash-programming, not needed to build the applications. 

--  

* Build-Tools (make.exe, sh.exe etc.)
  Unix/Linux users will already have the needed tools installed 
  and don't need any additional ones. For Windows-users:
  If you are already using WinARM skip this step.
  Of cause other tools could be used as long as they include a 
  gnu-make and the needed tools to support make. Other toolset:
  mingw, cygwin, unxutils, tools from WinAVR (also from mingw), 
  Yagarto's openocd-package etc.
  The tools wich come with WinARM have been used for testing. 
  - Download WinARM-20060606 from 
    http://www.siwawi.arubi.uni-kl.de/avr_projects/arm_projects
  - unpack the archive to C:\WinARM
  - extend the PATH environment variable so it includes
    at least c:\WinARM\utils\bin.
 


Create the Luminary Library
---------------------------

The original code of the Luminary driver-library is available
from the Luminary web-site.

A special makefile is included in this archive which uses the
new feature of gcc4 to remove unused code. The time-consuming
build-process implemented in the original Luminary makefile to 
separate every function in a separate object is not longer needed. 

A simple "make all" in the src-subdirectory will do to build 
the library libluminary.a:

C:\WinARM\examples\lm3s811_evalboard\lib_src>make all
arm-none-eabi-gcc -c -mcpu=cortex-m3 -I. -gdwarf-2 -DBUILD_ALL -Os -Wall -Wstric
t-prototypes -Wundef -Wa,-adhlns=adc.lst  -std=gnu99 -ffunction-sections -fdata-
sections -mthumb  adc.c -o adc.o
arm-none-eabi-gcc -c -mcpu=cortex-m3 -I. -gdwarf-2 -DBUILD_ALL -Os -Wall -Wstric
t-prototypes -Wundef -Wa,-adhlns=comp.lst  -std=gnu99 -ffunction-sections -fdata
-sections -mthumb  comp.c -o comp.o
[...]
arm-none-eabi-gcc -c -mcpu=cortex-m3 -I. -gdwarf-2 -DBUILD_ALL -Os -Wall -Wstric
t-prototypes -Wundef -Wa,-adhlns=watchdog.lst  -std=gnu99 -ffunction-sections -f
data-sections -mthumb  watchdog.c -o watchdog.o
arm-none-eabi-gcc -c -mcpu=cortex-m3 -I. -x assembler-with-cpp -Wa,-adhlns=cpu.l
st,-gdwarf-2 -mthumb -Dgcc -DBUILD_ALL cpu.S -o cpu.o
arm-none-eabi-ar rcs ../libluminary.a adc.o comp.o flash.o gpio.o i2c.o interrup
t.o pwm.o ssi.o sysctl.o systick.o timer.o uart.o watchdog.o cpu.o


C:\WinARM\examples\lm3s811_evalboard\lib_src>dir ..\*.a
 Datenträger in Laufwerk C: ist WINNT

 Verzeichnis von C:\WinARM\examples\lm3s811_evalboard

24.10.2006  xx:xx              154.090 libluminary.a
               1 Datei(en)        154.090 Bytes



Building an example-application
-------------------------------

The example source-code is from the Luminary DriverLib. I have modified the 
code a little bit to avoid warnings and included a special linker-script 
and makefiles. Special attribute have been added to the ISR declarations.
The startup_gcc.c-file in the examples directory have been modified a 
little bit for an extra stack-section which as used in the linker-script 
with alignment and by the ISR attribute.

Change to one of the examples-directories and type "make all".
I have included Makefiles which are prepared to use the Codesourcery-toolchain
and modified the linker-script for compatibility with the gcc4 unused code
removal feature. The stack is assigned to a separate section. Memory-address
of this section is assigned by a linker-script entry (with aligment).

Example for the "hello"-application:

> "make.exe" all

-------- begin (mode: ROM_RUN) --------
arm-none-eabi-gcc (CodeSourcery ARM Sourcery G++ 2006q3-26) 4.1.1
Copyright (C) 2006 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


Compiling C: hello.c
arm-none-eabi-gcc -c -mthumb -mcpu=cortex-m3 -mthumb-interwork -I. -gdwarf-2 -DROM_RUN 
 -Dgcc -D__WinARM__ -D__WINARMSUBMDL_lm3s811__  -Os -ffunction-sections -fdata-sections 
 -Wall -Wimplicit  -Wpointer-arith -Wswitch -Wredundant-decls -Wreturn-type -Wshadow 
 -Wunused -Wa,-adhlns=hello.lst   -Wcast-qual -MD -MP -MF .dep/hello.o.d 
 -Wnested-externs  -std=gnu99 -Wmissing-prototypes  -Wstrict-prototypes 
 -Wmissing-declarations hello.c -o hello.o 

Compiling C: startup_gcc.c

[...]

Linking: main.elf
[...]

Creating load file for Flash: main.hex
arm-none-eabi-objcopy -O ihex main.elf main.hex

Creating Extended Listing: main.lss
arm-none-eabi-objdump -h -S -D main.elf > main.lss

Creating Symbol Table: main.sym
arm-none-eabi-nm -n main.elf > main.sym

Size after:
main.elf  :
section            size        addr
.text              2420           0
.bss                  4   536870912
.stackarea          260   536870916
.comment            432           0
.debug_aranges     1248           0
.debug_pubnames    2675           0
.debug_info        7091           0
.debug_abbrev      2080           0
.debug_line        3155           0
.debug_frame       2448           0
.debug_str         2919           0
.debug_loc         4358           0
.ARM.attributes      16           0
.debug_ranges        96           0
Total             29202



Errors: none
-------- end --------


Now there is a hex-file with the flash-image in the directory:

C:\WinARM\examples\lm3s811_evalboard\ev-lm3s811\hello>dir *.hex
 Volume in Laufwerk C: hat keine Bezeichnung.

 Verzeichnis von C:\WinARM\examples\lm3s811_evalboard\ev-lm3s811\hello

10.11.2006  21:23             6.829 main.hex
               1 Datei(en)          6.829 Bytes

The makefile (examples.mk which gets included from the project-specific
makefiles) also has an option to create image-files in "raw binary"-format.
(FOMAT=binary). This is useful to create a binary which should be "flashed"
with OpenOCD and lmiflash.



Transfer the code to the controller's flash memory with uVision
---------------------------------------------------------------

The package includes "project files" for Keil/ARM uVision
flash-programming. Verify that FORMAT is set to ihex in examples.mk
and FLASHTOOL is set to UVISION. Type "make program" and uVision will 
be called with the correct parameters to flash the controller thru 
the internal programming hardware on the eval-board (FTDI2232):

C:\WinARM\examples\lm3s811_evalboard\ev-lm3s811\hello>make program

Programming LM3S811 with uVision
C:\Keil\uv3\Uv3.exe -f ../../tools/uvisionflash811.uv2 -ouvisionflash.txt


Contents of the log-file on success:

C:\WinARM\examples\lm3s811_evalboard\ev-lm3s811\hello>type uvisionflash.txt
Load "main.hex"
Found Luminary Micro LM3S811-C0
Flash=65536, RAM=8192
Flash Erase Done.
Flash Write Done: 2472 bytes programmed.
Flash Verify Done: 2472 bytes verified.


Transfer the code to the controller's flash memory with OpenOCD
---------------------------------------------------------------

To create a binary-file (main.bin) which can be programmed with
OpenOCD: modify the examples.mk so the format variable is set as 
binary (FORMAT=binary) and FLASHTOOL is set to OPENOCD. make program
will than call OpenOCD to upload the data to the flash.
Further details on using OpenOCD with Cortex M3 can be found on:
http://www.siwawi.arubi.uni-kl.de/avr_projects/arm_projects/openocd_cortex/


Transfer the code to the controller's flash memory with lmiflash
----------------------------------------------------------------

In examples.mk set FORMAT=binary and FLASHTOOL=LMIFLASH. 
make program will call lmiflash.exe and upload the data to the flash-
memory.



Appendix
--------

From the Codesourcery 2006q3-26 readme pdf:

"Because of a discrepancy between the ARMv7M Architecture and the ARM EABI 
it is not safe to use normal C functions directly as interrupt handlers. 
The EABI requires the stack be 8-byte aligned, whereas ARMv7M only guarantees
4-byte alignment when calling an interrupt vector. This can cause subtle 
runtime failures, usually when 8-byte types are used. Functions that are used 
directly as interrupt handlers should be annotated with 
__attribute__((__interrupt__)). This tells the compiler to add special stack 
alignment code to the function prologue."

mthomas: I have added this attribute to the ISR-declarations and wrapped
them inside #ifdef __GNUC__ #endif for compatibilty with other toolschains.



TODOs
-----
- update examples.mk so sh.exe is no longer needed to avoid dependency of
  sh.exe. Codesourcery package comes with make.exe and rm.exe but without sh.
- sync with or use "default"-linker-scripts in lib/ 
- linker-script for "RAM"

---

Have fun,
Martin Thomas
