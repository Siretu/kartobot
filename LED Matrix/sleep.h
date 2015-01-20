/*
 * power.h
 *
 *  Created on: 21 Jul 2011
 *      Author: exuvo
 */

#ifndef POWER_H_
#define POWER_H_
#ifdef __cplusplus
extern "C" {
#endif

void SleepIdle();
void SleepStandby();
void SleepPowerDown();
void awaken();

#ifdef __cplusplus
}
#endif
#endif /* POWER_H_ */
