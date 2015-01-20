/*****************************************************************************/
/* startup_LPC17xx.c: Startup file for LPC17xx device series                 */
/*****************************************************************************/

#include "system_LPC17xx.h"
#include "LPC17xx.h"


// Dummy handler.
void Default_Handler (void) {LPC_GPIO1->FIOSET = (1 << 29); while (1); }

// Weakly bind all interrupt vectors to the dummy handler.
void __attribute__((weak)) Reset_Handler(void);
void __attribute__((weak)) NMI_Handler(void);
void __attribute__((weak)) HardFault_Handler(void);
void __attribute__((weak)) MemManage_Handler(void);
void __attribute__((weak)) BusFault_Handler(void);
void __attribute__((weak)) UsageFault_Handler(void);
void __attribute__((weak)) vPortSVCHandler(void);
void __attribute__((weak)) DebugMon_Handler(void);
void __attribute__((weak)) xPortPendSVHandler(void);
void __attribute__((weak)) xPortSysTickHandler(void);

void __attribute__((weak)) WDT_IRQHandler(void);
void __attribute__((weak)) TIMER0_IRQHandler(void);
void __attribute__((weak)) TIMER1_IRQHandler(void);
void __attribute__((weak)) TIMER2_IRQHandler(void);
void __attribute__((weak)) TIMER3_IRQHandler(void);
void __attribute__((weak)) UART0_IRQHandler(void);
void __attribute__((weak)) UART1_IRQHandler(void);
void __attribute__((weak)) UART2_IRQHandler(void);
void __attribute__((weak)) UART3_IRQHandler(void);
void __attribute__((weak)) PWM1_IRQHandler(void);
void __attribute__((weak)) I2C0_IRQHandler(void);
void __attribute__((weak)) I2C1_IRQHandler(void);
void __attribute__((weak)) I2C2_IRQHandler(void);
void __attribute__((weak)) SPI_IRQHandler(void);
void __attribute__((weak)) SSP0_IRQHandler(void);
void __attribute__((weak)) SSP1_IRQHandler(void);
void __attribute__((weak)) PLL0_IRQHandler(void);
void __attribute__((weak)) RTC_IRQHandler(void);
void __attribute__((weak)) EINT0_IRQHandler(void);
void __attribute__((weak)) EINT1_IRQHandler(void);
void __attribute__((weak)) EINT2_IRQHandler(void);
void __attribute__((weak)) EINT3_IRQHandler(void);
void __attribute__((weak)) ADC_IRQHandler(void);
void __attribute__((weak)) BOD_IRQHandler(void);
void __attribute__((weak)) USB_IRQHandler(void);
void __attribute__((weak)) CAN_IRQHandler(void);
void __attribute__((weak)) DMA_IRQHandler(void);
void __attribute__((weak)) I2S_IRQHandler(void);
void __attribute__((weak)) ENET_IRQHandler(void);
void __attribute__((weak)) RIT_IRQHandler(void);
void __attribute__((weak)) MCPWM_IRQHandler(void);
void __attribute__((weak)) QEI_IRQHandler(void);
void __attribute__((weak)) PLL1_IRQHandler(void);
void __attribute__((weak)) USBActivity_IRQHandler(void);
void __attribute__((weak)) CANActivity_IRQHandler(void);

#pragma weak NMI_Handler        = Default_Handler
#pragma weak HardFault_Handler  = Default_Handler
#pragma weak MemManage_Handler  = Default_Handler
#pragma weak BusFault_Handler   = Default_Handler
#pragma weak UsageFault_Handler = Default_Handler
#pragma weak vPortSVCHandler    = Default_Handler
#pragma weak DebugMon_Handler   = Default_Handler
#pragma weak xPortPendSVHandler = Default_Handler
#pragma weak xPortSysTickHandler= Default_Handler

#pragma weak WDT_IRQHandler     = Default_Handler
#pragma weak TIMER0_IRQHandler  = Default_Handler
#pragma weak TIMER1_IRQHandler  = Default_Handler
#pragma weak TIMER2_IRQHandler  = Default_Handler
#pragma weak TIMER3_IRQHandler  = Default_Handler
#pragma weak UART0_IRQHandler   = Default_Handler
#pragma weak UART1_IRQHandler   = Default_Handler
#pragma weak UART2_IRQHandler   = Default_Handler
#pragma weak UART3_IRQHandler   = Default_Handler
#pragma weak PWM1_IRQHandler    = Default_Handler
#pragma weak I2C0_IRQHandler    = Default_Handler
#pragma weak I2C1_IRQHandler    = Default_Handler
#pragma weak I2C2_IRQHandler    = Default_Handler
#pragma weak SPI_IRQHandler     = Default_Handler
#pragma weak SSP0_IRQHandler    = Default_Handler
#pragma weak SSP1_IRQHandler    = Default_Handler
#pragma weak PLL0_IRQHandler    = Default_Handler
#pragma weak RTC_IRQHandler     = Default_Handler
#pragma weak EINT0_IRQHandler   = Default_Handler
#pragma weak EINT1_IRQHandler   = Default_Handler
#pragma weak EINT2_IRQHandler   = Default_Handler
#pragma weak EINT3_IRQHandler   = Default_Handler
#pragma weak ADC_IRQHandler     = Default_Handler
#pragma weak BOD_IRQHandler     = Default_Handler
#pragma weak USB_IRQHandler     = Default_Handler
#pragma weak CAN_IRQHandler     = Default_Handler
#pragma weak DMA_IRQHandler     = Default_Handler
#pragma weak I2S_IRQHandler     = Default_Handler
#pragma weak ENET_IRQHandler    = Default_Handler
#pragma weak RIT_IRQHandler     = Default_Handler
#pragma weak MCPWM_IRQHandler   = Default_Handler
#pragma weak QEI_IRQHandler     = Default_Handler
#pragma weak PLL1_IRQHandler    = Default_Handler
#pragma weak USBActivity_IRQHandler  = Default_Handler
#pragma weak CANActivity_IRQHandler  = Default_Handler

// The signature of Cortex-M3 interrupt handlers.
typedef void (* const Interrupt_Handler_P)(void);

// Start of the stack (last RAM address; exported in the linker script)
extern unsigned long __cs3_stack;


//*****************************************************************************
//
// Interrupt vectors table for Cortex M3.  Note that the proper constructs
// must be placed on this to ensure that it ends up at physical address
// 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".cs3.reset_vector")))
Interrupt_Handler_P reset_vectors[] = {
	(void (*)(void))&__cs3_stack,/* Top of Stack                 */
    Reset_Handler,               /* Reset Handler                */
};
__attribute__ ((section(".cs3.interrupt_vector")))
Interrupt_Handler_P interrupt_vectors[] = {
	// Standard Cortex-M3 interrupts:
	(void (*)(void))&__cs3_stack,/* Top of Stack                 */
    Reset_Handler,               /* Reset Handler                */
    NMI_Handler,                 /* NMI Handler                  */
    HardFault_Handler,           /* Hard Fault Handler           */
    MemManage_Handler,           /* MPU Fault Handler            */
    BusFault_Handler,            /* Bus Fault Handler            */
    UsageFault_Handler,          /* Usage Fault Handler          */
    0,                           /* Reserved                     */
    0,                           /* Reserved                     */
    0,                           /* Reserved                     */
    0,                           /* Reserved                     */
    vPortSVCHandler,             /* SVCall Handler               */
    DebugMon_Handler,            /* Debug Monitor Handler        */
    0,                           /* Reserved                     */
    xPortPendSVHandler,          /* PendSV Handler               */
    xPortSysTickHandler,         /* SysTick Handler              */

    // Vendor specific interrupts for LPC1768:
    WDT_IRQHandler,              /* 16: Watchdog Timer               */
    TIMER0_IRQHandler,           /* 17: Timer0                       */
    TIMER1_IRQHandler,           /* 18: Timer1                       */
    TIMER2_IRQHandler,           /* 19: Timer2                       */
    TIMER3_IRQHandler,           /* 20: Timer3                       */
    UART0_IRQHandler,            /* 21: UART0                        */
    UART1_IRQHandler,            /* 22: UART1                        */
    UART2_IRQHandler,            /* 23: UART2                        */
    UART3_IRQHandler,            /* 24: UART3                        */
    PWM1_IRQHandler,             /* 25: PWM1                         */
    I2C0_IRQHandler,             /* 26: I2C0                         */
    I2C1_IRQHandler,             /* 27: I2C1                         */
    I2C2_IRQHandler,             /* 28: I2C2                         */
    SPI_IRQHandler,              /* 29: SPI                          */
    SSP0_IRQHandler,             /* 30: SSP0                         */
    SSP1_IRQHandler,             /* 31: SSP1                         */
    PLL0_IRQHandler,             /* 32: PLL0 Lock (Main PLL)         */
    RTC_IRQHandler,              /* 33: Real Time Clock              */
    EINT0_IRQHandler,            /* 34: External Interrupt 0         */
    EINT1_IRQHandler,            /* 35: External Interrupt 1         */
    EINT2_IRQHandler,            /* 36: External Interrupt 2         */
    EINT3_IRQHandler,            /* 37: External Interrupt 3         */
    ADC_IRQHandler,              /* 38: A/D Converter                */
    BOD_IRQHandler,              /* 39: Brown-Out Detect             */
    USB_IRQHandler,              /* 40: USB                          */
    CAN_IRQHandler,              /* 41: CAN                          */
    DMA_IRQHandler,              /* 42: General Purpose DMA          */
    I2S_IRQHandler,              /* 43: I2S                          */
    ENET_IRQHandler,             /* 44: Ethernet                     */
    RIT_IRQHandler,              /* 45: Repetitive Interrupt Timer   */
    MCPWM_IRQHandler,            /* 46: Motor Control PWM            */
    QEI_IRQHandler,              /* 47: Quadrature Encoder Interface */
    PLL1_IRQHandler,             /* 48: PLL1 Lock (USB PLL)          */
    USBActivity_IRQHandler,		 /* 49: USB Activity 				 */
    CANActivity_IRQHandler		 /* 50: CAN Activity			 	 */
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext, __cs3_region_start_ram, _edata, __bss_start__, __bss_end__,
__ram1_start__, __ram1_end__, __ram2_start__, __ram2_end__;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied main() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
__attribute__ ((section(".cs3.reset")))
void Reset_Handler(void){
	// Initialise clocks
	SystemInit();

	unsigned long *s, *d;
	// Copy initialisation data to RAM (.data section)
	s = &_etext;
	d = &__cs3_region_start_ram;
	while (d < &_edata) *d++ = *s++;

	// Zero the remaining allocated RAM (.bss, .ram1, .ram2 section)
	d = &__bss_start__;
	while (d < &__bss_end__)  *d++ = 0;

	d = &__ram1_start__;
	while (d < &__ram1_end__)  *d++ = 0;

	d = &__ram2_start__;
	while (d < &__ram2_end__)  *d++ = 0;

	/* Call CTORS of static objects */
	extern void __libc_init_array(void);
	__libc_init_array();

	// Call the application's entry point.
	extern void main(void);
	main();
}
