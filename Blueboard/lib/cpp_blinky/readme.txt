Building and Debugging the Blinky Example
=========================================

The Blinky example for ARM has been tested with the AT91SAM7S-EK board
and the devkitARM GNU-based toolset, which is part of the devkitPro 
distribution (https://sourceforge.net/projects/devkitpro/files/devkitARM).
The example can be built from command line or from the Eclipse IDE.
The provided Makefile is designed to be used in both cases. The Makefile
also supports building the Debug, and Release configurations.


Building the Examples from Command Line
---------------------------------------
To build the example code from command line you need the GNU-make utility.
You can obtain GNU-make version 3.81 and other UNIX-style file utilities
commonly used in Makefiles from:

http://www.state-machine.com/resources/goodies.php#Make

The utilities are compiled to run natively on Win32 without any additional DLLs.


The Blinky code contains Makefile, which uses the devkitARM GNU-based toolset.
The Makefile assumes that the toolchain is installed in the directory:

C:/tools/devkitPro/devkitARM/bin

You need to adjust the symbol GNU_ARM at the top of the Makefile
if you have installed the toolset in a different location. Atlternatively
you can also use a different GNU-ARM distribution.

The Makefile supports two build configurations: Debug (default) and Release.
You choose the build configuration by defining the CONF symbol at the 
command line, as shown in the table below:


Build Configuration             |  Build command
--------------------------------+-----------------
Debug (default)                 |  make 
Release                         |  make CONF=rel
Clean the Debug configuration   |  make clean
Clean the Release configuration |  make CONF=rel clean



Building the Examples from Eclipse
----------------------------------
The example code contains the Eclipse projects for building and debugging
the Blinky example with Eclipse (http://www.eclipse.org/downloads/).
The provided Eclipse projects are Makefile-type projects, which use the 
same Makefiles that you can call from the command line. In fact the Makefiles
are specifically designed to allow building all supported configurations
from Eclipse. To use the project in Eclipse, you need to import a Makefile-
type project into the Eclipse workspace.

The provided Makefile allows you to create and configure the build
configurations from the "Project | Build Configurations | Manage..."
sub-menu. For the Release configuration, you should set the make command
to "make CONF=rel". The provided  Makefile also correctly supports the
clean targets, so invoking "Project | Clean..." menu for any build
configuration works as expected.

To read more about using Eclipse to build and debug ARM projects, please
download the Application Note: "QP™ and ARM7/9 with GNU" available at 
http://www.state-machine.com/arm/AN_QP_and_ARM7_ARM9-GNU.pdf



Downloading to Flash and Debugging the Examples
===============================================
 
You have several options for downloading and debugging the example.
The devkitPro project also includes the Insight GDB front-end (https://sourceforge.net/projects/devkitpro/files/Insight), which
is perhaps the simplest way. The other option is to use Eclipse with
the Zylin Embedded CDT plugin. Please refer to the Zylin website at http://opensource.zylin.com/embeddedcdt.html for more information how to 
install the CDT plugin in Eclipse.
  
Starting the GDB Server 
-----------------------
Regardless whether you use Insight, Eclipse, or even just the raw GDB,
you would need a J-Tag pod and a GDB Server software. This example uses
the J-Link pod from SEGGER and the J-Link GDB Server software v4.08l
available for download from www.segger.com/cms/downloads.html. However, you 
might just as well use other J-Tag pods and other GDB server software,
such as OpenOCD (http://openocd.berlios.de).
 
You launch the J-Link GDB server by clicking on the JLinkGDBServer.exe
application in the SEGGER  directory.

NOTE: You need a license to run the J-Link GDB Server. SEGGER now
offers free licenses for Non-Commercial-Use, see 
http://www.segger.com/cms/j-link-arm-for-non-commercial-use.html. 

Downloading to Flash and Debugging with Insight
----------------------------------------------- 
The example code contains the GDB command file jlink.gdb file as well as
the insight-jlink.bat batch file to launch the Insight debugger. 
To start debugging you can simply double-clink on the insight-jlink.bat
batch file, which will launch the Insight debugger and load the
dbg\blinky.elf image to the flash memory of the AT91SAM7S microcontroller.
As the application is running, the status LEDs at the top of the board
should blink.  

Downloading to Flash and Debugging with Eclipse 
-----------------------------------------------
You can also use Eclipse to debug the Blinky application. Configuring the
Eclipse debugger is somewhat involved and requires a plugin to improve
Embedded debugging with the Eclipse CDT. The YAGARTO website provides
a step-by-step tutorial (see http://www.yagarto.de/howto/yagarto2/)
of how to install Eclipse and the Zylin Embedded CDT plugin

Before you can actually download the application to the board and
start debugging, you need to lanuch the SEGGER J-Link GDB Server
as discussed in the previous section.

the Application Note: "QP™ and ARM7/9 with GNU" available at 
http://www.state-machine.com/arm/AN_QP_and_ARM7_ARM9-GNU.pdf provides
screen shots of configuring the debug session in Eclipse (see Section 2.2.3)

 
Contact information
===================
Quantum Leaps, LLC
103 Cobble Ridge Drive
Chapel Hill, NC 27516
USA

info@quantum-leaps.com
www.state-machine.com
