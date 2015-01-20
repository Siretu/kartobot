This example implements a mass storage class device that operates on the internal SRAM in the MCU.

When the example is run it will enumerate as a mass storage device and show up in your Windows file explorer as a drive with the volume label "nxpUSBlib".

The drive will be 6.5k in size and contain a single file called "README.TXT" which contains the text "This is a USB Memory Device demonstration. "

 
Use the following project configurations to run on these MCU/board combinations:

                                  BSP              nxpUSBlib     Example_MassStorageDevice    MCU
                        ======================  ==============   =========================  ========
LPC11U14 / LPCXpresso   LPC11Uxx_LPCXpressoRevB LPC11Uxx_Device           LPC11Uxx          LPC11U14
LPC1768 / LPCXpresso    LPC17xx_LPCXpressoRevB  LPC17xx_Device            LPC17xx           LPC1768
LPC1850 / Hitex         LPC18xx_HITEX           LPC18xx_Device            LPC18xx           LPC1850
LPC1850 / EA1800        LPC18xx_EA              LPC18xx_Device            LPC18xx           LPC1850
 
** be sure to change the MCU setting in the Example_MassStorageDevice project file: Project->Properties->C/C++ Build->MCU settings->NXP->LPC28xx->LPC1850
** be sure to set the active build configuration: Window->Preferences->C/C++->Indexer->Use active build configuration

On the Hitex LPC1850 Evaluation board this example will use USB 0 which is marked as X2

On the LPCXpresso Rev B base board this example will use USB 0 which is marked as X1

