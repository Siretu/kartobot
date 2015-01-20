LMIFLASH Flash Programming Tool
(c) 2006 Luminary Micro, Inc. 

Version 1.0.226, 11/10/2006

================================================================================
Hardware/Software Requirements

- Windows 2000, XP or later
- Luminary Micro FTDI driver (found at http://www.luminarymicro.com)
- Luminary Micro LM3Sxxx development board, LM3S811 evaluation board


================================================================================
Running LMIFLASH

LMIFLASH is intended to be run from the command line.  To run the application,
open a console window.  To do this, click on Start>Run... and type "cmd", or
select "Console Prompt" from the Start>All Program>Accessories menu (Win XP).

Once the console window is running, you have 3 options for running LMIFLASH:

1. Navigate to the directory where LMIFLASH resides and execute
2. Type the full path of where LMIFLASH resides and execute
3. Add LMIFLASH to your PATH so that it can be executed from anywhere

To display a list of commands, type "help".  For example:

    >lmiflash help
    
    
================================================================================
Commands (also visible by using the "help" feature)

-f	Filename, must be a binary (*.bin) file

-m	Mass erase the flash.  If -m or -e is not specified, a mass
    is performed.

-e	Erase address range (1kB pages).  Can specify individual pages
    by starting address only, or an address range.  Specifying the
	  range 0-0x400 will erase pages 0 and 1 since 0x400 is in page 1.
	  If -m or -e is not specified, a mass erase is performed.

-o	Programming address offset (if not 0x0)

-x  Crystal frequency fed into the device.  This flag helps calculate
	  flash programming timings.  The default value is set to 6 (since
	  all Luminary eval/dev boards ship with a 6MHz crystal.  If you see
	  programming errors, it's most likely due to flash timings.
	
	        Crystal value (MHz)    -x value to pass
	            3.50 - 4.49                4
	            4.50 - 5.49                5
	            5.50 - 6.49                6
	            6.50 - 7.49                7
	            7.50 - 8.49                8
	            8.50 - 9.49                9
	
-v	Verify the flash content after programming

-r  Perform a reset after program verify erase operation

Usage examples:

	  >lmiflash -f filename.bin -v -r
	  >lmiflash -f filename.bin -x 6 -v -r
	  >lmiflash -e 0-0xfff -f filename.bin
	  >lmiflash -m -f filename.bin -o 0x1000
	  >lmiflash -m -v
	  >lmiflash -e 0x400
	  >lmiflash -e 0-0x3ff    


================================================================================
Release Notes

v1.0.226, 11/10/2006

 -  Fixed problem where entering a filename with "./" or ".." would crash the 
    tool.  (example: ./foo/foo.bin or ../../foo/foo.bin)

v1.0.222, 11/07/2006
   
 -  Initial release.


================================================================================
Known Issues

v1.0.226, 11/10/2006

 -  Same as below.

v1.0.222, 11/07/2006
  
 -  The -r option may appear to not work with the LM3S811 evaluation board due 
    to the OLED display.  Depending on how the display is configured, it may 
    not restart after the reset is asserted.  The -r option performs a core 
    reset. 
    
 -  If more than one Luminary Micro board (evaluation or development) is plugged
    in while running LMIFLASH, it is possible that windows will generate a blue
    screen and crash.  This appears to be a problem with the FTDI driver, and 
    will hopefully be addressed in the future.
    