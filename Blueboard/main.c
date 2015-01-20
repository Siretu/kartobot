/*
 * This file demonstrates the use of FreeRTOS-MPU.  It creates tasks in both
 * User mode and Privileged mode, and using both the original xTaskCreate() and
 * the new xTaskCreateRestricted() API functions.  The purpose of each created
 * task is documented in the comments above the task function prototype (in
 * this file), with the task behaviour demonstrated and documented within the 
 * task function itself.  In addition a queue is used to demonstrate passing
 * data between protected/restricted tasks as well as passing data between an
 * interrupt and a protected/restricted task.
 */

#include "LPC17xx.h"
#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "main.h"
#include "UART.h"
#include "I2C.h"
#include "sonar.h"
#include "navigation.h"
#include "Time2.h"
#include "term_io.h"
#include "XBee.h"
#include "main.h"
#include "Wheels.h"
#include "faults.h"
#include "DMA.h"
#include "Audio.h"
#include "motor.h"
#include "Magneto.h"
#include "Gyro.h"
#include "Accel.h"
#include "Position.h"
#include "Power.h"
#include "AHRS.h"
#include "status.h"
#include "Servo.h"
#include "Display.h"
#include "USB.h"
#include "utils.h"
#include <stdarg.h>
#include "diskio.h"
#include "ff_test_term.h"
//#include <malloc.h>

struct TASKS tasks;
uint8_t lulz;//Test variable

void vApplicationIdleHook( void ){

}

/* SysTick 1ms */
void vApplicationTickHook( void ){
	static uint32_t div10;

	if (++div10 >= 10) {
		div10 = 0;
		extern void disk_timerproc(void);
		disk_timerproc(); /* Disk timer function (100Hz) */
	}

	ff_test_term_timer_callback();
}

static void xputsD(void *str){
	if(str != NULL){
		xputs((const char*) str);
		vPortFree(str);
	}
	vTaskDelete(NULL);
}

void xputsDelayed(const char* str){
	xTaskCreate(xputsD,							/* code. */
				(signed char*) "xputsD",	/* name. */
				64,												/* Stack depth in words. */
				str,											/* Task parameters. */
				8,												/* Priority and mode. */
				NULL											/* Handle. */
				);
}

static portTickType xFlashRate  = FLASH_NORMAL;
static void flash(void *pvParameters){
	while(1){
		/* Delay for half the flash period then turn the LED on. */
		LPC_GPIO1->FIOSET = (1 << 29);
		vTaskDelay(xFlashRate);

		/* Delay for half the flash period then turn the LED off. */
		LPC_GPIO1->FIOCLR = (1 << 29);
		vTaskDelay(xFlashRate);
	}
}

void FlashRate(portTickType rate){
	xFlashRate = rate;
}

portTickType FlashRateGet(){
	return xFlashRate;
}

static uint32_t buttonPressed  = 0;
void Button(uint32_t state){
	uint32_t diff = Time_Millis() - buttonPressed;

	if(state && buttonPressed != 0){
		if(diff < 800){
			USB_ChangeDescriptor(DESCRIPTOR_TOGGLE);
		}
	}

	buttonPressed = Time_Millis();
}

static void fail(void *pvParameters){
	extern uint32_t sonarUnresonable;
	extern uint32_t sonarQueueFull;
	extern uint32_t sonarLeft;
	extern uint32_t sonarRight;
	extern uint32_t wheelsFails;
	extern uint32_t magnetoOverflow;
	extern uint32_t magnetoFails;
	extern uint32_t audioUnderrun;
	extern uint32_t powerFails;
	extern uint32_t gyroFails;
	extern uint32_t accelFails;
	extern uint8_t sbrkFails;

	uint32_t sonarU = 0;
	uint32_t sonarQF = 0;
	uint32_t sonarL = 0;
	uint32_t sonarR = 0;
	uint32_t wheelF = 0;
	uint32_t magnetoO = 0;
	uint32_t magnetoF = 0;
	uint32_t audioU = 0;
	uint32_t powerF = 0;
	uint32_t gyroF = 0;
	uint32_t accelF = 0;
	uint8_t sbrkF = 0;

	while(1){
		if(sonarL != sonarLeft){
			xprintf("Sonar Left is now %lu \n", sonarLeft);
			sonarL = sonarLeft;
		}

		if(sonarR != sonarRight){
			xprintf("Sonar Right is now %lu \n", sonarRight);
			sonarR = sonarRight;
		}

		if(sonarU != sonarUnresonable){
			xprintf("Sonar Unreasonable is now %lu \n", sonarUnresonable);
			sonarU = sonarUnresonable;
		}

		if(sonarQF != sonarQueueFull){
			xprintf("Sonar QueueFull is now %lu \n", sonarQueueFull);
			sonarQF = sonarQueueFull;
		}

		if(wheelF != wheelsFails){
			xprintf("Wheel fails is now %lu \n", wheelsFails);
			wheelF = wheelsFails;
		}

		if(magnetoO != magnetoOverflow){
			xprintf("Magneto overflow is now %lu \n", magnetoOverflow);
			magnetoO = magnetoOverflow;
		}

		if(magnetoF != magnetoFails){
			xprintf("Magneto fails is now %lu \n", magnetoFails);
			magnetoF = magnetoFails;
		}

		if(audioU != audioUnderrun){
			xprintf("Audio underrun is now %lu \n", audioUnderrun);
			audioU = audioUnderrun;
		}

		if(powerF != powerFails){
			xprintf("Power fails is now %lu \n", powerFails);
			powerF = powerFails;
		}

		if(gyroF != gyroFails){
			xprintf("Gyro fails is now %lu \n", gyroFails);
			gyroF = gyroFails;
		}

		if(accelF != accelFails){
			xprintf("Accel fails is now %lu \n", accelFails);
			accelF = accelFails;
		}

		if(sbrkF != sbrkFails){
			xprintf("sbrk fails is now %u \n", sbrkFails);
			sbrkF = sbrkFails;
		}

		if(!(LPC_GPIO2->FIOPIN & _BV(10))){//Button low
			if(buttonPressed != 0 && Time_Millis() - buttonPressed > 800){

				buttonPressed = 0;
			}
		}

		vTaskDelay(100);
	}
}

static portSTACK_TYPE flashTaskStack[ 64 ];
static const xTaskParameters flashTaskDefinition =
{
		flash,   										/* pvTaskCode */
		(signed char *) "flash",   	/* pcName */
		64,             						/* usStackDepth - defined in words, not bytes. */
    NULL,            						/* pvParameters */
    9 | portPRIVILEGE_BIT,      /* uxPriority */
    flashTaskStack,      				/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

static portSTACK_TYPE failTaskStack[ 70 ];
static const xTaskParameters failTaskDefinition =
{
		fail,   									/* pvTaskCode */
		(signed char *) "fail",   /* pcName */
		70,             					/* usStackDepth - defined in words, not bytes. */
    NULL,            					/* pvParameters */
    9 | portPRIVILEGE_BIT,    /* uxPriority */
    failTaskStack,      			/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

//Anything that might call an API function or cause a context switch!
void init(void *pvParameters){
	Audio_Init();
	AHRS_Init();
	I2C0_Test();

	vTaskDelay(200);

	xprintf("Free Memory %lu %lu\n", (uint32_t) xPortGetFreeHeapSize(), (uint32_t) xPortGetFreeHeapSize2());

//	uint32_t s = 0;
//	void* p = 1;
//	while(p != NULL){
//		p = pvPortMalloc2(s+=32);
//		extern char* get_heap_end(void);
//
//		xprintf("Malloc %lx %lx %lu\n", p, get_heap_end(), s);
//		vPortFree2(p);
//		vTaskDelay(15);
//	}

	vTaskDelete(NULL);
}

void filesystem(void *pvParameters){
	//TODO When a task uses file IO use this thread for disk access.
	DSTATUS old = STA_NOINIT | STA_NODISK;

	while(1){
		DSTATUS ds = disk_status(0);
		if((old & STA_NODISK) && !(ds & STA_NODISK) && (ds & STA_NOINIT)){//Automount
			ds = disk_initialize(0);
			if(!(ds & STA_NOINIT)){
				FRESULT fr = f_mount(0, &Fatfs[0]);
				if(fr == FR_OK){
					xprintf("Auto mounted SD card\n");
				}else{
					xprintf("Failed to auto mount SD card\n");
					put_rc(fr);
				}
			}else{
				xprintf("Failed to auto initialise SD card\n");
			}
		}else if(!(old & STA_NODISK) && !(old & STA_NOINIT) && (ds & STA_NODISK)){//Detect removal
			xprintf("SD card removed.\n");
		}
		old = ds;

		vTaskDelay(100);
	}
}

void console(void *pvParameters){
	ff_test_term();
}

void main() __attribute__ ((noreturn));
void main(){
	LPC_GPIO1->FIODIR |= (1 << 29);//Debug LED
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk|SCB_SHCSR_BUSFAULTENA_Msk|SCB_SHCSR_MEMFAULTENA_Msk;// Enable Usage Fault, Bus Fault, and MMU Fault
	SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;// Enable trapping of divide by zero
	SystemCoreClockUpdate();

	__disable_irq();

//  if(mallopt(M_TOP_PAD, 1024) == 0){
//		//Failed
//	}

	DMA_Init();
	UARTS_Init();
	XBee_Init();
	USB_Init2();
	term_io_Init();
	Time_Init();
	I2CS_Init();
	Power_Init();
	Sonar_Init();
	Navigation_Init();
	Wheels_Init();
	Motor_Init();
	Position_Init();
	Servo_Init();
	SDSocket_Init();

	xTaskCreateRestricted( &flashTaskDefinition, &tasks.flash);
	xTaskCreateRestricted( &XBeeInTaskDefinition, &tasks.xbeeIn);
	xTaskCreateRestricted( &XBeeOutTaskDefinition, &tasks.xbeeOut);
	xTaskCreateRestricted( &usbTaskDefinition, &tasks.usb);
	xTaskCreateRestricted( &sonarTaskDefinition, &tasks.sonar);
	xTaskCreateRestricted( &navigationTaskDefinition, &tasks.navigation);
	xTaskCreateRestricted( &AudioTaskDefinition, &tasks.audio);
	xTaskCreateRestricted( &failTaskDefinition, &tasks.fail);
	xTaskCreateRestricted( &positionTaskDefinition, &tasks.position);
	xTaskCreateRestricted( &powerTaskDefinition, &tasks.power);
	xTaskCreateRestricted( &ahrsTaskDefinition, &tasks.ahrs);
	xTaskCreateRestricted( &statusTaskDefinition, &tasks.status);
	xTaskCreateRestricted( &displayTaskDefinition, &tasks.display);
	xTaskCreate(init,								/* code. */
				(signed char*) "init",		/* name. */
				100,											/* Stack depth in words. */
				NULL,											/* Task parameters. */
				10 | portPRIVILEGE_BIT,		/* Priority and mode. */
				NULL											/* Handle. */
				);
	xTaskCreate(filesystem,						/* code. */
				(signed char*) "filesystem",		/* name. */
				128,											/* Stack depth in words. */
				NULL,											/* Task parameters. */
				1 | portPRIVILEGE_BIT,		/* Priority and mode. */
				&tasks.filesystem											/* Handle. */
				);
	xTaskCreate(console,						/* code. */
				(signed char*) "console",	/* name. */
				192,											/* Stack depth in words. */
				NULL,											/* Task parameters. */
				3 | portPRIVILEGE_BIT,		/* Priority and mode. */
				&tasks.display						/* Handle. */
				);

	/* Start the scheduler. */
	__enable_irq();
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	LPC_GPIO1->FIOSET = (1 << 29);
	while(1);
}

void vConfigureTimerForRunTimeStats( void ){
	TIM_TIMERCFG_Type tc;
	tc.PrescaleOption = TIM_PRESCALE_USVAL; //Overflows after 12Hours: 0xFFFFFFFF / (1/0.000010) = 11.9H
	tc.PrescaleValue =  10UL;	/* Good enough to get a decent resolution, but not too fast so as to overflow all the time. */
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &tc);

	TIM_ResetCounter(LPC_TIM1);
	TIM_Cmd(LPC_TIM1, ENABLE);
}


