#include "LPC18xx.h"
#include "config.h"
#include "EMC.h"
#include "timer.h"
#include "lpc_types.h"
#include "lcd_params.h"
#include "lcd_driver.h"
#include "lpc_swim.h"
#include "lcd_type.h"
#include "kiosk_usb.h"
#include "kiosk_lcd.h"
#include "emc.h"

extern SWIM_WINDOW_T win1;

/** Mass Storage Class driver interface configuration and state information. This structure is
 *  passed to all Mass Storage Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_MS_Host_t FlashDisk_MS_Interface =
	{
		.Config =
			{
				.DataINPipeNumber       = 1,
				.DataINPipeDoubleBank   = false,

				.DataOUTPipeNumber      = 2,
				.DataOUTPipeDoubleBank  = false,
			},
	};


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
void kiosk_usb_setup(void)
{
	SetupHardware();

	puts_P(PSTR(ESC_FG_CYAN "Mass Storage Host Demo running.\r\n" ESC_FG_BLACK));

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	sei();

/*
	for (;;)
	{
		MassStorageHost_Task();
		
		MS_Host_USBTask(&FlashDisk_MS_Interface);
		USB_USBTask();
	}
*/
}

/** FAT Fs structure to hold the internal state of the FAT driver for the Dataflash contents. */
static FATFS DiskFATState;

/** FAT Fs structure to hold a FAT file handle for the log data write destination. */
static FIL Slide;

typedef struct bmpfile_header {
  unsigned char magic[2];
  uint32_t filesz;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
}__attribute__ ((packed)) BMPFILEHEADER;
BMPFILEHEADER bmpFileHeader;

typedef struct {
  uint32_t header_sz;
  int32_t width;
  int32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
}__attribute__ ((packed)) BITMAPINFOHEADER;
BITMAPINFOHEADER bitmapInfoHeader;

int kioskUsbDeviceAttached = FALSE;

int kiosk_usb_ready(void)
{
	return !MS_Host_TestUnitReady(&FlashDisk_MS_Interface, 0);
}

void kiosk_usb_task(void)
{
    uint32_t PrevSelectedPipe = Pipe_GetCurrentPipe();

	MS_Host_USBTask(&FlashDisk_MS_Interface);
	USB_USBTask();

    Pipe_SelectPipe(PrevSelectedPipe);
}

typedef struct
{
	char *slideAddress;
	int slideSize;
}__attribute__ ((packed)) KIOSK_SLIDES;
KIOSK_SLIDES kioskSlides[KIOSK_SLIDE_COUNT];

static int kioskNumSlides = 0;

int kioskSlide = 0;

void kiosk_usb_load_slides(void)
{
	uint8_t ErrorCode;
	UINT bytesRead;
	int i;

	kioskNumSlides = 0;
	kioskSlide = 0;

	f_mount(0, &DiskFATState);

	for (i = 0; i < KIOSK_SLIDE_MAX; i++)
	{
		char fileName[12];

		// Form the filename based on the loop counter
		sprintf(fileName, "slide%02d.bmp", i + 1);
		ErrorCode = f_open(&Slide, fileName, FA_READ);

		if (!ErrorCode)
		{

			// Read the bmp file header
			ErrorCode = f_read (&Slide,
								(void *)&bmpFileHeader,
								sizeof(bmpFileHeader),
								&bytesRead);

			// check for a valid file format
			if (!ErrorCode &&
					bytesRead == sizeof(bmpFileHeader) &&
					bmpFileHeader.magic[0] == 0x42 &&
					bmpFileHeader.magic[1] == 0x4d &&
					Slide.fsize == bmpFileHeader.filesz &&
					bmpFileHeader.bmp_offset < bmpFileHeader.filesz)
			{
				int bmpSize = bmpFileHeader.filesz - bmpFileHeader.bmp_offset;

				// Read the bitmap info header
				ErrorCode = f_read (&Slide,
									(void *)&bitmapInfoHeader,
									sizeof(bitmapInfoHeader),
									&bytesRead);

				if (i)
					kioskSlides[i].slideAddress = kioskSlides[i-1].slideAddress + kioskSlides[i-1].slideSize;
				else
					kioskSlides[i].slideAddress = (char *)LCD_FRAME_BUF;

				kioskSlides[i].slideSize = bmpSize;

				//  clear the memory where the bitmap sits
				memset(kioskSlides[i].slideAddress, 0, kioskSlides[i].slideSize);

				kiosk_lcd_load_display(kioskNumSlides + 1);

				// Read the bitmap into memory (inverted vertically)
				for(int row = 0; row < bitmapInfoHeader.height; row++)
					ErrorCode = f_read (&Slide,
										(void *)kioskSlides[i].slideAddress,
										kioskSlides[i].slideSize,
										&bytesRead);
				kioskNumSlides++;

				f_close(&Slide);
				continue;

			}
			f_close(&Slide);
		}
		break;
	}

	// unmount
	f_mount(0, NULL);

}

void kiosk_usb_stop_slides(void)
{
	vTimer_Disable(1); // timer 1 runs the slide show

	kiosk_lcd_idle_display();

}

void kiosk_usb_run_slides(void)
{
	vTimer_Enable(1); // timer 1 runs the slide show

}

// Called by a timer at 100hZ
void kiosk_advance_slide(void)
{
	static int runningCounter = 1001;
	int ADCPotValue = ADCRead( 0, 0 ) / (0x400 / 10);

	runningCounter += ADCPotValue; // ADCPotValue can be a value between 0 and 100

	if (runningCounter > 1000)
	{
		runningCounter = 0;

		if (kioskNumSlides)
		{
			kiosk_lcd_set_display_pointer(kiosk_get_slide_addr(kioskSlide++));
			if (kioskSlide == kioskNumSlides)
				kioskSlide = 0;
		}
	}

}


uint32_t kiosk_get_slide_addr(int slideNum)
{
	if (slideNum > kiosk_get_num_slides())
		return 0;

	return (uint32_t)kioskSlides[slideNum].slideAddress;
}

int kiosk_get_num_slides(void)
{
	return kioskNumSlides;
}
/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if ARCH==ARCH_LPC
	bsp_init();
#else
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#endif
	/* Hardware Initialization */
	Serial_Init(9600, false);
	LEDs_Init();
	USB_Init();

	/* Create a stdio stream for the serial port for stdin and stdout */
	Serial_CreateStream(NULL);
}


/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(void)
{
	puts_P(PSTR("Device Attached.\r\n"));
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
	kioskUsbDeviceAttached = TRUE;
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(void)
{
	puts_P(PSTR("\r\nDevice Unattached.\r\n"));
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	kioskUsbDeviceAttached = FALSE;
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);

	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];

	if (USB_Host_GetDeviceConfigDescriptor(1, &ConfigDescriptorSize, ConfigDescriptorData,
	                                       sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful)
	{
		puts_P(PSTR("Error Retrieving Configuration Descriptor.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		return;
	}

	if (MS_Host_ConfigurePipes(&FlashDisk_MS_Interface,
	                           ConfigDescriptorSize, ConfigDescriptorData) != MS_ENUMERROR_NoError)
	{
		puts_P(PSTR("Attached Device Not a Valid Mass Storage Device.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		return;
	}

	if (USB_Host_SetDeviceConfiguration(1) != HOST_SENDCONTROL_Successful)
	{
		puts_P(PSTR("Error Setting Device Configuration.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		return;
	}

	uint8_t MaxLUNIndex;
	if (MS_Host_GetMaxLUN(&FlashDisk_MS_Interface, &MaxLUNIndex))
	{
		puts_P(PSTR("Error retrieving max LUN index.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		USB_Host_SetDeviceConfiguration(0);
		return;
	}

	printf_P(PSTR("Total LUNs: %d - Using first LUN in device.\r\n"), (MaxLUNIndex + 1));

	if (MS_Host_ResetMSInterface(&FlashDisk_MS_Interface))
	{
		puts_P(PSTR("Error resetting Mass Storage interface.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		USB_Host_SetDeviceConfiguration(0);
		return;
	}

	SCSI_Request_Sense_Response_t SenseData;
	if (MS_Host_RequestSense(&FlashDisk_MS_Interface, 0, &SenseData) != 0)
	{
		puts_P(PSTR("Error retrieving device sense.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		USB_Host_SetDeviceConfiguration(0);
		return;
	}

	if (MS_Host_PreventAllowMediumRemoval(&FlashDisk_MS_Interface, 0, true))
	{
		puts_P(PSTR("Error setting Prevent Device Removal bit.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		USB_Host_SetDeviceConfiguration(0);
		return;
	}

	SCSI_Inquiry_Response_t InquiryData;
	if (MS_Host_GetInquiryData(&FlashDisk_MS_Interface, 0, &InquiryData))
	{
		puts_P(PSTR("Error retrieving device Inquiry data.\r\n"));
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		USB_Host_SetDeviceConfiguration(0);
		return;
	}

	printf_P(PSTR("Vendor \"%.8s\", Product \"%.16s\"\r\n"), InquiryData.VendorID, InquiryData.ProductID);

	puts_P(PSTR("Mass Storage Device Enumerated.\r\n"));
	LEDs_SetAllLEDs(LEDMASK_USB_READY);
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t ErrorCode)
{
	USB_Disable();

	printf_P(PSTR(ESC_FG_RED "Host Mode Error\r\n"
	                         " -- Error Code %d\r\n" ESC_FG_WHITE), ErrorCode);

	LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	for(;;);
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t ErrorCode,
                                            const uint8_t SubErrorCode)
{
	printf_P(PSTR(ESC_FG_RED "Dev Enum Error\r\n"
	                         " -- Error Code %d\r\n"
	                         " -- Sub Error Code %d\r\n"
	                         " -- In State %d\r\n" ESC_FG_WHITE), ErrorCode, SubErrorCode, USB_HostState);

	LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
}

