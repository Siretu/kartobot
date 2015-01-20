/*
 * SimpleTimer.h
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

#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include <inttypes.h>

typedef void (*timer_callback)(void);

class SimpleTimer {

public:
    // maximum number of timers
	#define MAX_TIMERS 16

    // setTimer() constants
	#define RUN_FOREVER 0
	#define RUN_ONCE 1

    // constructor
    SimpleTimer();

    // this function must be called inside loop()
    bool run();

    // call function f every d milliseconds
    uint8_t setInterval(long d, timer_callback f);

    // call function f once after d milliseconds
    uint8_t setTimeout(long d, timer_callback f);

    // call function f every d milliseconds for n times
    uint8_t setTimer(long d, timer_callback f, uint16_t n);

    void updateTimer(long d, uint8_t id);

    // destroy the specified timer
    void deleteTimer(uint8_t numTimer);

    // returns true if the specified timer is enabled
    bool isEnabled(uint8_t numTimer);

    // enables the specified timer
    void enable(uint8_t numTimer);

    // disables the specified timer
    void disable(uint8_t numTimer);

    // enables or disables the specified timer
    // based on its current state
    void toggle(uint8_t numTimer);

    // returns the number of used timers
    uint8_t getNumTimers();

    // returns the remaining time until the closest timer should run
    uint8_t getNextInvoke();

private:
    // value returned by the millis() function
    // in the previous run() call
    unsigned long prev_millis[MAX_TIMERS];

    // pointers to the callback functions
    timer_callback callbacks[MAX_TIMERS];

    // delay values
    unsigned long delays[MAX_TIMERS];

    // number of runs to be executed for each timer
    uint16_t maxNumRuns[MAX_TIMERS];

    // number of executed runs for each timer
    uint16_t numRuns[MAX_TIMERS];

    // which timers are enabled
    bool enabled[MAX_TIMERS];

    // actual number of timers in use
    uint8_t numTimers;
};

#endif
