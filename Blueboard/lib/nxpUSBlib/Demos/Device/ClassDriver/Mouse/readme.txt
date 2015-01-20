This example implements a HID class device with mouse report descriptors. Mouse movement reports are sent to the host by the device when buttons or the joystick is pressed on the development board.


Use the following project configurations to run on these MCU/board combinations:

                                  BSP              nxpUSBlib     Example_MouseDevice    MCU
                        ======================  ==============   ===================  ========
LPC11U14 / LPCXpresso   LPC11Uxx_LPCXpressoRevB LPC11Uxx_Device        LPC11Uxx       LPC11U14
LPC1768 / LPCXpresso    LPC17xx_LPCXpressoRevB  LPC17xx_Device         LPC17xx        LPC1768

** be sure to change the MCU setting in the Example_MouseDevice project file: Project->Properties->C/C++ Build->MCU settings->NXP->LPC28xx->LPC1850
** be sure to set the active build configuration: Window->Preferences->C/C++->Indexer->Use active build configuration

 

On the LPCXpresso Rev B base board this example will move the mouse cursor when the joystick is operated. Pressing the joystick down will cause a mouse left click.

 

** NOTE: The LPCXpresso Rev B base board needs power from one of the two mini USB connectors. It will not operate properly if powered only via the JTAG power on the LPCXpresso development board.

