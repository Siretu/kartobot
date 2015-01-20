This example is a generic HID template. It contains no functionality, it only enumerates as a HID device.


Use the following project configurations to run on these MCU/board combinations:

                                  BSP              nxpUSBlib     Example_GenericHID     MCU
                        ======================  ==============   ===================  ========
LPC11U14 / LPCXpresso   LPC11Uxx_LPCXpressoRevB LPC11Uxx_Device        LPC11Uxx       LPC11U14
LPC1768 / LPCXpresso    LPC17xx_LPCXpressoRevB  LPC17xx_Device         LPC17xx        LPC1768
LPC1850 / Hitex         LPC18xx_HITEX           LPC18xx_Device         LPC18xx        LPC1850
LPC1850 / EA1800        LPC18xx_EA              LPC18xx_Device         LPC18xx        LPC1850

** be sure to change the MCU setting in the Example_GenericHID project file: Project->Properties->C/C++ Build->MCU settings->NXP->LPC28xx->LPC1850
** be sure to set the active build configuration: Window->Preferences->C/C++->Indexer->Use active build configuration
