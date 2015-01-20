This example implements an audio output host class.

When buttons are pressed on the development board samples from a square wave generator are sent to a USB audio device.

 

How it works:

Program diagnostics are sent out the serial port while the program is running. The serial port is set to 9600 baud.

When the program is start up you should see "Audio Output Host Demo running." in the terminal window.

When USB speakers are plugged into the type A USB recepticle the terminal will show: "Device Attached." "Audio Device Enumerated."

** If the application is start while the speakers are plugged in, they will need to be unplugged and plugged back in. This is because the host is unable to determine if the speakers are on the bus, it can only detect the speakers being plugged into the recepticle.

Once the terminal shows the audio device is enumerated you should hear the square wave from the speakers when the button is pressed.

 

 

Use the following project configurations to run on these MCU/board combinations:

                                  BSP              nxpUSBlib     Example_AudioOutputHost    MCU
                        ======================  ==============   ======================  ========
LPC1768 / LPCXpresso    LPC17xx_LPCXpressoRevB  LPC17xx_Device         LPC17xx           LPC1768
LPC1850 / Hitex         LPC18xx_HITEX           LPC18xx_Device         LPC18xx           LPC1850
LPC1850 / EA1800        LPC18xx_EA              LPC18xx_Device         LPC18xx           LPC1850

** be sure to change the MCU setting in the Example_AudioOutputHost project file: Project->Properties->C/C++ Build->MCU settings->NXP->LPC28xx->LPC1850
** be sure to set the active build configuration: Window->Preferences->C/C++->Indexer->Use active build configuration

 

Hitex LPC1850 Evaluation board:

Connect a serial cable between UART (X1) on the Hitex board and a PC running a terminal program to view program diagnostic output.

Connect the USB speakers to the type A receptacle labeled X10

The 4 cap sense pads labeled T1-T4 are the buttons that send the samples.

 

LPCXpresso base board Rev B:

Connect a USB cable between the mini USB receptacle labled X3 and a PC running a terminal program to view program diagnostic output.

Connect the USB speakers to the type A receptacle labeled J60.

Place jumpers on 1-2, 3-4, 5-6 of J61 positioned directly behind the type A USB receptacle

The button marked 'WAKEUP SW4' will send samples out when pressed.
