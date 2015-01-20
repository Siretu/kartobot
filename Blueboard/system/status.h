/*
 * status.h
 *
 *  Created on: 17 Mar 2012
 *      Author: exuvo
 */

#ifndef STATUS_H_
#define STATUS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "lpc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void Status_Run(void *pvParameters);
extern const xTaskParameters statusTaskDefinition;

#ifdef __cplusplus
}
#endif

#endif /* STATUS_H_ */
