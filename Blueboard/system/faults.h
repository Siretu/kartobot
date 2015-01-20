/*
 * faults.h
 *
 *  Created on: 22 Nov 2011
 *      Author: exuvo
 */

#ifndef FAULTS_H_
#define FAULTS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOSConfig.h"

#define HARD_FAULT 1
#define MEMORY_FAULT 2
#define BUS_FAULT 3
#define USAGE_FAULT 0

typedef struct{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;	//address of faulting instruction = return address - 2
	uint32_t psr;

//	uint32_t BFAR;
	uint32_t CFSR;
	uint32_t HFSR;
	uint32_t DFSR;
	uint32_t AFSR;

	char taskName[configMAX_TASK_NAME_LEN];
	uint8_t faultType;
	uint32_t status;
	void * address;
	uint8_t faultCounter;
}StackTrace;

#ifdef __cplusplus
}
#endif
#endif /* FAULTS_H_ */
