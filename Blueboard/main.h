/*
 * main.h
 *
 *  Created on: 15 Oct 2011
 *      Author: exuvo
 */

#ifndef MAIN_H_
#define MAIN_H_

//TODO solder a xbee pin to blueboard reset. Would enable remote reset.

/* ERRATA
 * P1.31 as AD0.5 not working, 50 Ohm to ground.
 * P3.25 as PWM1.2 not working.
 */

/* Tasks
 * LOW Priority
 * 0 =
 * 1 = filesystem
 * 2 = Navigation
 * 3 = Display, console
 * 4 = Audio
 * 5 = Sonar, Power
 * 6 = XBee In, XBee Out, USB, USB Switch
 * 7 = Position, AHRS
 * 8 = xprintfDelayed
 * 9 = Fail, Flash, Status
 * 10 = Init
 * HIGH Priority
 */

/* DMA Channels
 * HIGH Priority
 * 0 = Audio
 * 1 = UART TODO
 * 2 =
 * 3 =
 * 4 = SD Card TX
 * 5 = SD Card RX
 * 6 =
 * 7 =
 * LOW Priority
 */

/* NVIC
 * HIGH Priority
 * 0 =
 * 1 =
 * 2 =
 * 3 =
 * 4 =
 * 5 =
 * 6 =
 * 7 =
 * 8 =
 * 9 =
 * 10 = MAX_SYSCALL
 * 11 =
 * 12 =
 * 13 =
 * 14 = TIMER2_IRQHandler - Servo PWM
 * 15 = TIMER0_IRQHandler - Sonar
 * 16 = DMA
 * 17 = USB
 * 18 = EINT3 - QuadratureEncoder
 * 19 = UART0, UART1, UART3
 * 20 = I2C0, I2C1
 * 21 =
 * 22 = ADC
 * 23 =
 * 24 =
 * 25 =
 * 26 =
 * 27 =
 * 28 =
 * 29 =
 * 30 =
 * 31 = FreeRTOS Kernel
 * LOW Priority
 */

/* Timers
 * Systick = 1ms;
 * 0 =  1us. Time2.h, Sonar, I2C timeout.
 * 1 = 10us. Run time statistics.
 * 2 = 20ms. PWM for servo control http://www.brookshiresoftware.com/rd_how_servos_work.htm
 * 3 =
 */

/* PWM Channels 100kHz
 * 0 = Timer Reset
 * 1 = See errata
 * 2 = Motor RL
 * 3 =
 * 4 = Motor RR
 * 5 = Motor FL
 * 6 = Motor FR
 * 7 =
 *
 */

/* ADC Channels 200kHz
 * 0 = Battery Voltage
 * 1 = Errata: Internal sync inputs not operational.
 * 2 = Battery Current
 * 3 =
 * 4 =
 * 5 =
 * 6 =
 * 7 =
 *
 */

/* I2C 0
 * 0x03 = Self
 * 0x1E - Magnetometer - HMC5883/5854
 * 0x50 = EEPROM - AT24C256
 * 0x53 = Accelerometer - ADXL345
 * 0x68 = Gyroscope - ITG3200
 */

/* I2C 1
 * 0x01 = LED matrix
 * 0x03 = Self
 * 0x04 = Camera
 */

struct TASKS{
	xTaskHandle xbeeIn;
	xTaskHandle xbeeOut;
	xTaskHandle usb;
	xTaskHandle flash;
	xTaskHandle faults;
	xTaskHandle audio;
	xTaskHandle sonar;
	xTaskHandle navigation;
	xTaskHandle fail;
	xTaskHandle position;
	xTaskHandle magnetometer;
	xTaskHandle power;
	xTaskHandle ahrs;
	xTaskHandle status;
	xTaskHandle display;
	xTaskHandle filesystem;
	xTaskHandle console;
};
extern struct TASKS tasks;

#define FLASH_NORMAL 500
#define FLASH_WARN 200
#define FLASH_FATAL 80

#define FlashNormal() FlashRate(FLASH_NORMAL)
#define FlashWarn() FlashRate(FLASH_WARN)
#define FlashFatal() FlashRate(FLASH_FATAL)

void FlashRate(portTickType FlashRate);
portTickType FlashRateGet();

void xputsDelayed(const char* str);

#endif /* MAIN_H_ */
