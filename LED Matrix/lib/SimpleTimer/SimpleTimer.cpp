/*
 * SimpleTimer.cpp
 *
 * SimpleTimer - A timer library for Arduino.
 * Author: mromani@ottotecnica.com
 * Copyright (c) 2010 OTTOTECNICA Italy
 *
 * This library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser
 * General Public License along with this library; if not,
 * write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "SimpleTimer.h"


SimpleTimer::SimpleTimer() {
    long current_millis = millis();

    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        enabled[i] = false;
        callbacks[i] = 0;
        prev_millis[i] = current_millis;
    }

    numTimers = 0;
}

bool SimpleTimer::run() {
	bool didSomething = false;
    // get current time
    unsigned long current_millis = millis();

    for (uint8_t i = 0; i < MAX_TIMERS; i++) {

        // only process active timers
        if (callbacks[i] && enabled[i]) {

			// is it time to process this timer ?
			if (current_millis - prev_millis[i] >= delays[i]) {

			    // update time
				prev_millis[i] = current_millis;
				didSomething = true;

				// "run forever" timers must always be executed
				if (maxNumRuns[i] == RUN_FOREVER) {
					(*callbacks[i])();
				}
				// other timers get executed the specified number of times
				else if (numRuns[i] < maxNumRuns[i]) {
					(*callbacks[i])();
					numRuns[i]++;

					// after the last run, delete the timer
					// to save some cycles
					if (numRuns[i] >= maxNumRuns[i]) {
						deleteTimer(i);
					}
				}
			}
        }
    }
    return didSomething;
}

uint8_t SimpleTimer::getNextInvoke(){
	// get current time
    unsigned long current_millis = millis();
    unsigned long nextInvoke = -1;

    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        if (callbacks[i] && enabled[i]) {
        	unsigned long delay = max(0, (long)(delays[i] - (current_millis - prev_millis[i])));
			if (delay < nextInvoke) {
				nextInvoke = delay;
			}
        }

    }
    return nextInvoke;
}

uint8_t SimpleTimer::setTimer(long d, timer_callback f, uint16_t n) {
    if (numTimers >= MAX_TIMERS) {
    	warn(9);
        return -1;
    }

    uint8_t free = -1;
    for(uint8_t i=0; i < MAX_TIMERS; i++){
    	if(callbacks[i] == 0){
    		free = i;
    		break;
    	}
    }

    if(free != -1){
    	delays[free] = d;
		callbacks[free] = f;
		maxNumRuns[free] = n;
		enabled[free] = true;
		numRuns[free] = 0;

		numTimers++;
    }

    return free;
}


uint8_t SimpleTimer::setInterval(long d, timer_callback f) {
    return setTimer(d, f, RUN_FOREVER);
}


uint8_t SimpleTimer::setTimeout(long d, timer_callback f) {
    return setTimer(d, f, RUN_ONCE);
}

void SimpleTimer::updateTimer(long d, uint8_t id){
	delays[id] = d;
}


void SimpleTimer::deleteTimer(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    // nothing to disable if no timers are in use
    if (numTimers == 0) {
        return;
    }

    callbacks[numTimer] = 0;
    enabled[numTimer] = false;
    delays[numTimer] = 0;

    // update number of timers
    numTimers--;
}


bool SimpleTimer::isEnabled(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return false;
    }

    return enabled[numTimer];
}


void SimpleTimer::enable(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    enabled[numTimer] = true;
}


void SimpleTimer::disable(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    enabled[numTimer] = false;
}


void SimpleTimer::toggle(uint8_t numTimer) {
    if (numTimer >= MAX_TIMERS) {
        return;
    }

    enabled[numTimer] = !enabled[numTimer];
}


uint8_t SimpleTimer::getNumTimers() {
    return numTimers;
}
