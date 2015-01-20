/*
 * quad.h
 *
 *  Created on: 19 Jul 2011
 *      Author: exuvo
 *
 * ---------------------------------------------------------------------
 *  ATMega8
 * ---------------------------------------------------------------------
 *  Fuse lock byte
 *  0x2f = - - 1 1   1 1 1 1
 *             \ /   \ / \ /
 *              |     |   +----- LB 2..1 (11 = No memory lock)
 *              |     +------------ BLB0 02..01 (11 = no restrictions in Application section.)
 *              +------------ BLB1 12..11 (11 = no restrictions in Boot Loader section.)
 *  Fuse high byte:
 *  0xc7 = 1 1 0 0   0 1 1 1 <-- BOOTRST (1 = jump to application at start)
 *         ^ ^ ^ ^   ^ \ /
 *         | | | |   |  +------- BOOTSZ 0..1 (11 = 256B starting at 0xF80)
 *         | | | |   + --------- EESAVE (0 = preserve EEPROM over chip erase)
 *         | | | +-------------- CKOPT (0 = high frequency oscillator)
 *         | | +---------------- SPIEN (0 = allow serial programming)
 *         | +------------------ WDTON (1 = watchdog disabled at start)
 *         +-------------------- RSTDISBL (1 = reset pin is enabled)
 *  Fuse low byte:
 *  0x2f = 0 0 1 0   1 1 1 1
 *         ^ ^ \ /   \--+--/
 *         | |  |       +------- CKSEL 3..0 (1111 = crystal oscillator)
 *         | |  +--------------- SUT 1..0 (10 = startup time 16K CK + 4.1ms)
 *         | +------------------ BODEN (0 = Brown out detector enabled)
 *         +-------------------- BODLEVEL (0 = brown out @ 4V)
 *                  _____                            _____  _____
 *                  RESET -      (PCINT14/RESET) PC6|1    \/   28|PC5 (ADC5/SCL/PCINT13) - I2C SCL
 *        MATRIX_SR_DATA  -        (PCINT16/RXD) PD0|2         27|PC4 (ADC4/SDA/PCINT12) - I2C SDA
 *        MATRIX_SR_SCK   -        (PCINT17/TXD) PD1|3  ATmega 26|PC3 (ADC3/PCINT11)     -
 *        MATRIX_SR_LATCH -       (PCINT18/INT0) PD2|4     8   25|PC2 (ADC2/PCINT10)     - BAR_SR_CLK
 *                        -  (PCINT19/OC2B/INT1) PD3|5         24|PC1 (ADC1/PCINT9)      - BAR_SR_LATCH
 *                        -     (PCINT20/XCK/T0) PD4|6         23|PC0 (ADC0/PCINT8)      - BAR_SR_DATA
 *                     5V -                      VCC|7         22|GND                    - 0V
 *                     0V -                      GND|8         21|AREF                   - 0.1uF - GND
 *                  16MHz - (PCINT6/XTAL1/TOSC1) PB6|9         20|AVCC                   - 5V
 *                  16MHz - (PCINT7/XTAL2/TOSC2) PB7|10        19|PB5 (SCK/PCINT5)       - SCK
 *                        -    (PCINT21/OC0B/T1) PD5|11        18|PB4 (MISO/PCINT4)      - MISO
 *                        -  (PCINT22/OC0A/AIN0) PD6|12        17|PB3 (MOSI/OC2A/PCINT3) - MOSI
 *                        -       (PCINT23/AIN1) PD7|13        16|PB2 (SS/OC1B/PCINT2)   - BAR_9
 *                        -   (PCINT0/CLKO/ICP1) PB0|14        15|PB1 (OC1A/PCINT1)      - BAR_10
 *                                                  |____________|
 *
 *
 *
 *
 *
 *	I2C Address list:
 *	Ourselves	0x1		1
 *
 *	Timers:
 *	0: time
 *	1: PWM
 *	2: PWM
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "utils.h"

#include <Print.h>
#include <Streaming/Streaming.h>
#include "time.h"
#include <SimpleTimer/SimpleTimer.h>

extern uint16_t load;
extern SimpleTimer timer;

//Dynamic memory allocation
void * operator new(size_t size);
void * operator new[](size_t size);
void operator delete(void * ptr);
void operator delete[](void * ptr);
__extension__ typedef int __guard __attribute__((mode (__DI__)));
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual(void);

#endif
