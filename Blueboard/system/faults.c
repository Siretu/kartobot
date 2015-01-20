/*
 * Faults.c
 *
 *  Created on: 28 Sep 2011
 *      Author: exuvo
 */

#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "term_io.h"
#include "faults.h"
#include <stdio.h>
#include "XBee.h"
#include "Display.h"
#include "utils.h"

//Hard Fault
#define FORCED 		(1 << 30)
#define VECTTBL 	(1 << 1)

//Memory Fault
#define MMARVALID 	(1 << 7)
#define MSTKERR 	(1 << 4)
#define MUNSTKERR 	(1 << 3)
#define DACCVIOL 	(1 << 1)
#define IACCVIOL 	(1 << 0)

//Bus Fault
#define BFARVALID 	(1 << 7)
#define STKERR 		(1 << 4)
#define UNSTKERR 	(1 << 3)
#define IMPRECISERR (1 << 2)
#define PRECISERR 	(1 << 1)
#define IBUSERR 	(1 << 0)

//Usage Fault
#define DIVBYZERO 	(1 << 9)
#define UNALIGNED 	(1 << 8)
#define NOCP 		(1 << 3)
#define INVPC 		(1 << 2)
#define INVSTATE 	(1 << 1)
#define UNDEFINSTR 	(1 << 0)

#ifndef PRIVILEGED_FUNCTION
#define PRIVILEGED_FUNCTION __attribute__((section("privileged_functions")))
#endif

static StackTrace trace;


PRIVILEGED_FUNCTION void HardFault_Handler( void ){
	__asm volatile
	(
			"	tst lr, #4						\n"
			"	ite eq							\n"
			"	mrseq r0, msp					\n"
			"	mrsne r0, psp					\n"
			"	b HardFault_HandlerC			\n"
			:::"r0"
	);
}

PRIVILEGED_FUNCTION void MemManage_Handler( void ){
	__asm volatile
	(
			"	tst lr, #4						\n"
			"	ite eq							\n"
			"	mrseq r0, msp					\n"
			"	mrsne r0, psp					\n"
			"	b MemManage_HandlerC			\n"
			:::"r0"
	);
}

PRIVILEGED_FUNCTION void BusFault_Handler( void ){
	__asm volatile
	(
			"	tst lr, #4						\n"
			"	ite eq							\n"
			"	mrseq r0, msp					\n"
			"	mrsne r0, psp					\n"
			"	b BusFault_HandlerC				\n"
			:::"r0"
	);
}

PRIVILEGED_FUNCTION void UsageFault_Handler( void ){
	__asm volatile
	(
			"	tst lr, #4						\n"
			"	ite eq							\n"
			"	mrseq r0, msp					\n"
			"	mrsne r0, psp					\n"
			"	b UsageFault_HandlerC			\n"
			:::"r0"
	);
}

PRIVILEGED_FUNCTION static char* FaultPrintStackTrace(char *b){
	if(trace.faultType == HARD_FAULT){
		b+=sprintf (b, "\n[Hard Fault]\n");
		if(trace.status & FORCED){//Indicates a forced hard fault
			b+=sprintf (b, "FORCED\n");
		}
		if(trace.status & VECTTBL){//Indicates a BusFault on a vector table read during exception processing
			b+=sprintf (b, "VECTTBL\n");
		}
	}else if(trace.faultType == MEMORY_FAULT){
		b+=sprintf (b, "\n[Memory Fault]\n");
		if(trace.status & MMARVALID){//MMAR holds a valid fault address
			b+=sprintf (b, "MMAR %p\n", (void*) trace.address);
		}
		if(trace.status & MSTKERR){//MemManage fault on stacking for exception entry
			b+=sprintf (b, "MSTKERR\n");
		}
		if(trace.status & MUNSTKERR){//MemManage fault on unstacking for a return from exception
			b+=sprintf (b, "MUNSTKERR\n");
		}
		if(trace.status & DACCVIOL){//Data access violation flag
			b+=sprintf (b, "DACCVIOL\n");
		}
		if(trace.status & IACCVIOL){//Instruction access violation flag
			b+=sprintf (b, "IACCVIOL\n");
		}
	}else if(trace.faultType == BUS_FAULT){
		b+=sprintf (b, "\n[Bus Fault]\n");
		if(trace.status & BFARVALID){//BFAR holds a valid fault address
			b+=sprintf (b, "BFAR %p\n", (void*) trace.address);
		}
		if(trace.status & STKERR){//BusFault on stacking for exception entry
			b+=sprintf (b, "STKERR\n");
		}
		if(trace.status & UNSTKERR){//BusFault on unstacking for a return from exception
			b+=sprintf (b, "UNSTKERR\n");
		}
		if(trace.status & IMPRECISERR){//A data bus error has occurred
			b+=sprintf (b, "IMPRECISERR\n");
		}
		if(trace.status & PRECISERR){//A data bus error has occurred
			b+=sprintf (b, "PRECISERR\n");
		}
		if(trace.status & IBUSERR){//Instruction bus error
			b+=sprintf (b, "IBUSERR\n");
		}
	}else if(trace.faultType == USAGE_FAULT){
		b+=sprintf (b, "\n[Usage Fault]\n");
		if(trace.status & DIVBYZERO){//The processor has executed an SDIV or UDIV instruction with a divisor of 0.
			b+=sprintf (b, "DIVBYZERO\n");
		}
		if(trace.status & UNALIGNED){//The processor has made an unaligned memory access
			b+=sprintf (b, "UNALIGNED\n");
		}
		if(trace.status & NOCP){//The processor has attempted to access a coprocessor.
			b+=sprintf (b, "NOCP\n");
		}
		if(trace.status & INVPC){//Invalid PC load UsageFault, caused by an invalid PC load by EXC_RETURN
			b+=sprintf (b, "INVPC\n");
		}
		if(trace.status & INVSTATE){//The processor has attempted to execute an instruction that makes illegal use of the EPSR
			b+=sprintf (b, "INVSTATE\n");
		}
		if(trace.status & UNDEFINSTR){//The processor has attempted to execute an undefined instruction
			b+=sprintf (b, "UNDEFINSTR\n");
		}
	}

	b+=sprintf (b, "Task \"%s\"\n", trace.taskName);
	b+=sprintf (b, "R0 = %lX\n", trace.r0);
	b+=sprintf (b, "R1 = %lX\n", trace.r1);
	b+=sprintf (b, "R2 = %lX\n", trace.r2);
	b+=sprintf (b, "R3 = %lX\n", trace.r3);
	b+=sprintf (b, "R12 = %lX\n", trace.r12);
	b+=sprintf (b, "LR = %lX\n", trace.lr);
	b+=sprintf (b, "PC = %p\n", (void*) trace.pc);
	b+=sprintf (b, "PSR = %lX\n", trace.psr);
//	b+=sprintf (b, "BFAR = %lX\n", trace.BFAR);
	b+=sprintf (b, "CFSR = %lX\n", trace.CFSR);
	b+=sprintf (b, "HFSR = %lX\n", trace.HFSR);
	b+=sprintf (b, "DFSR = %lX\n", trace.DFSR);
	b+=sprintf (b, "AFSR = %lX\n", trace.AFSR);
	return b;
}

PRIVILEGED_FUNCTION static void FaultSendStackTrace(){
	{//XBee
		uint8_t buffer[512];

		uint16_t term_io_getF(char *buff, uint16_t len);
		uint16_t size = term_io_getF((char*) buffer, sizeof(buffer));
		if(size > 0){
			XBee_SendImmediately(buffer, size);
		}

		uint8_t *b = (uint8_t*) FaultPrintStackTrace((char*)buffer);
		XBee_SendImmediately(buffer, b-buffer);
	}
	{//Display
		Display_FaultError(trace.pc, (uint32_t)trace.address,
						   ((uint16_t)trace.faultType << 8) | ((trace.status & _BV(30)) >> 28) | ((trace.status & 0x380) >> 2) | (trace.status & 0x1F) );
	}

	LPC_GPIO1 -> FIOCLR = (1 << 0)|(1 << 1)|(1 << 4)|(1 << 8)|(1 << 9)|(1 << 10)|(1 << 14)|(1 << 15);//Disable motors
}

PRIVILEGED_FUNCTION static void FaultStackTrace(unsigned long * args){
	LPC_GPIO1->FIOSET = (1 << 29);
	xTaskHandle task = xTaskGetCurrentTaskHandle();

	/* args contains: r0, r1, r2, r3, r12, r14, the return address and xPSR. */
	trace.r0 = ((unsigned long) args[0]);
	trace.r1 = ((unsigned long) args[1]);
	trace.r2 = ((unsigned long) args[2]);
	trace.r3 = ((unsigned long) args[3]);

	trace.r12 = ((unsigned long) args[4]);
	trace.lr =  ((unsigned long) args[5]);
	trace.pc =  ((unsigned long) args[6]);
	trace.psr = ((unsigned long) args[7]);

//	trace.BFAR = SCB->BFAR;
	trace.CFSR = SCB->CFSR;
	trace.HFSR = SCB->HFSR;
	trace.DFSR = SCB->DFSR;
	trace.AFSR = SCB->AFSR;

	const signed char * name = pcTaskGetTaskName(task);
	for(uint8_t i=0; i<sizeof(trace.taskName); i++){
		trace.taskName[i] = name[i];
	}
	trace.taskName[sizeof(trace.taskName)-1] = 0;

	trace.status = 0;
	trace.address = NULL;
	trace.faultCounter++;
}

/*
 */
PRIVILEGED_FUNCTION void HardFault_HandlerC(unsigned long * args){
	FaultStackTrace(args);
	trace.faultType = HARD_FAULT;

	uint32_t status = SCB->HFSR;
	trace.status = status;

	if(status & FORCED){//Indicates a forced hard fault
		/* Generated by escalation of a fault with configurable priority that cannot be handled, either because of priority or because it is disabled.
		 * The HardFault handler must read the other fault status registers to find the cause of the fault. */

	}

	if(status & VECTTBL){//Indicates a BusFault on a vector table read during exception processing
		/* This error is always handled by the hard fault handler.
		 * The PC value stacked for the exception return points to the instruction that was preempted by the exception. */

	}

	FaultSendStackTrace();
	while(1);
}

/* Called when a task tries to access memory it is not allowed to access.
 */
PRIVILEGED_FUNCTION void MemManage_HandlerC(unsigned long * args){
	FaultStackTrace(args);
	trace.faultType = MEMORY_FAULT;

	uint8_t status = SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk;
	trace.status = status;
	void * mmfar = (void *) SCB->MMFAR;

	if(status & MMARVALID){//MMAR holds a valid fault address
		trace.address = mmfar;
	}else{
		mmfar = NULL;
	}

	if(status & MSTKERR){//MemManage fault on stacking for exception entry

	}

	if(status & MUNSTKERR){//MemManage fault on unstacking for a return from exception

	}

	if(status & DACCVIOL){//Data access violation flag
		/* The PC value stacked for the exception return points to the faulting instruction.
		 * The processor has loaded the MMAR with the address of the attempted access */

	}

	if(status & IACCVIOL){//Instruction access violation flag
		/* This fault occurs on any access to an XN region.
		 * The PC value stacked for the exception return points to the faulting instruction.
		 * The processor has not written a fault address to the MMAR. */

	}

	FaultSendStackTrace();
//	vTaskDelete(NULL);//Memory allocated by the task code is not automatically freed, and should be freed before the task is deleted.
//	SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTACT_Msk;
	while(1);
}

/* Called on bus errors
 */
PRIVILEGED_FUNCTION void BusFault_HandlerC(unsigned long * args){
	FaultStackTrace(args);
	trace.faultType = BUS_FAULT;

	uint8_t status = (SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) >> 8;
	trace.status = status;
	void * bfar = (void *) SCB->BFAR;

	if(status & BFARVALID){//BFAR holds a valid fault address
		trace.address = bfar;
	}else{
		bfar = NULL;
	}

	if(status & STKERR){//BusFault on stacking for exception entry

	}

	if(status & UNSTKERR){//BusFault on unstacking for a return from exception

	}

	if(status & IMPRECISERR){//A data bus error has occurred
		/* The return address in the stack frame is not related to the instruction that caused the error */

	}

	if(status & PRECISERR){//A data bus error has occurred
		/* The PC value stacked for the exception return points to the instruction that caused the fault.
		 * The processor has loaded the BFAR with the faulting address. */

	}

	if(status & IBUSERR){//Instruction bus error
		/* The processor detects the instruction bus error on prefetching an instruction,
		 * but it sets the IBUSERR flag to 1 only if it attempts to issue the faulting instruction.
		 * The processor has not written a fault address to the BFAR. */

	}

	FaultSendStackTrace();
//	vTaskDelete(NULL);//Memory allocated by the task code is not automatically freed, and should be freed before the task is deleted.
//	SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTACT_Msk;
	while(1);
}

/* Called on usage errors
 */
PRIVILEGED_FUNCTION void UsageFault_HandlerC(unsigned long * args){
	FaultStackTrace(args);
	trace.faultType = USAGE_FAULT;

	uint16_t status = (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) >> 16;
	trace.status = status;

	if(status & DIVBYZERO){//The processor has executed an SDIV or UDIV instruction with a divisor of 0.
		/* The PC value stacked for the exception return points to the instruction that performed the divide by zero.
		 * Enable trapping of divide by zero by setting the DIV_0_TRP bit in the CCR to 1. */

	}

	if(status & UNALIGNED){//The processor has made an unaligned memory access
		/* Enable trapping of unaligned accesses by setting the UNALIGN_TRP bit in the CCR to 1. */

	}

	if(status & NOCP){//The processor has attempted to access a coprocessor.
		/* The processor does not support coprocessor instructions. */

	}

	if(status & INVPC){//Invalid PC load UsageFault, caused by an invalid PC load by EXC_RETURN
		/* the processor has attempted an illegal load of EXC_RETURN to the PC, as a result of an invalid context, or an invalid EXC_RETURN value.
 	 	 * The PC value stacked for the exception return points to the instruction that tried to perform the illegal load of the PC */

	}

	if(status & INVSTATE){//The processor has attempted to execute an instruction that makes illegal use of the EPSR
		/* The PC value stacked for the exception return points to the instruction that attempted the illegal use of the EPSR.
		 * This bit is not set to 1 if an undefined instruction uses the EPSR */

	}

	if(status & UNDEFINSTR){//The processor has attempted to execute an undefined instruction
		/* The PC value stacked for the exception return points to the undefined instruction.
		 * An undefined instruction is an instruction that the processor cannot decode. */

	}

	FaultSendStackTrace();
//	vTaskDelete(NULL);//Memory allocated by the task code is not automatically freed, and should be freed before the task is deleted.
//	SCB->SHCSR &= ~SCB_SHCSR_USGFAULTACT_Msk;
	while(1);
}

/* If configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2 then this function will automatically get called if a task overflows its stack. */
PRIVILEGED_FUNCTION void vApplicationStackOverflowHook(xTaskHandle *task, signed char *pcTaskName){
	uint8_t buffer[128];
	uint8_t s = sprintf((char*) buffer, "Task \"%s\" has died due to a StackOverflow.\n", pcTaskName);
	XBee_SendImmediately(buffer, s);

	vTaskDelete(task);//TODO Memory allocated by the task code is not automatically freed, and should be freed before the task is deleted.
}

extern void xputsF (const char* str);

/* If configUSE_MALLOC_FAILED_HOOK is set to 1 then this function will be called automatically if a call to pvPortMalloc() fails.  pvPortMalloc()
 * is called automatically when a task, queue or semaphore is created. */
PRIVILEGED_FUNCTION void vApplicationMallocFailedHook(){
	xTaskHandle task = xTaskGetCurrentTaskHandle();

	void *p = malloc(60);

	if(p){
		sprintf((char*) p, "pvPortMalloc: Out of memory! \"%s\" Free %lu\n", pcTaskGetTaskName(task), (uint32_t) xPortGetFreeHeapSize());
		xputsF(p);
		free(p);
	}else{
		uint8_t buffer[60];
		uint8_t s = sprintf((char*) buffer, "pvPortMalloc: Out of memory! \"%s\" Free %lu\n", pcTaskGetTaskName(task), (uint32_t) xPortGetFreeHeapSize());
		XBee_SendImmediately(buffer, s);

		vTaskDelete(task);//TODO Memory allocated by the task code is not automatically freed, and should be freed before the task is deleted.
	}

}

PRIVILEGED_FUNCTION void vApplicationMallocFailedHook2(){
	xTaskHandle task = xTaskGetCurrentTaskHandle();

	void *p = malloc(60);

	if(p){
		sprintf((char*) p, "pvPortMalloc2: Out of memory! \"%s\" Free %lu\n", pcTaskGetTaskName(task), (uint32_t) xPortGetFreeHeapSize2());
		xputsF(p);
		free(p);
	}else{
		uint8_t buffer[60];
		uint8_t s = sprintf((char*) buffer, "pvPortMalloc2: Out of memory! \"%s\" Free %lu\n", pcTaskGetTaskName(task), (uint32_t) xPortGetFreeHeapSize2());
		XBee_SendImmediately(buffer, s);

		vTaskDelete(task);//TODO Memory allocated by the task code is not automatically freed, and should be freed before the task is deleted.
	}
}

