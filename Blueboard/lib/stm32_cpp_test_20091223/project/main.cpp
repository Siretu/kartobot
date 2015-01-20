/*******************************************************************************
* Project            : using C++ with the GNU tool-chain for
*                      ARM based controllers
* File Name          : main.cpp
* Author             : Martin Thomas
* Version            : see VERSION_STRING below
* Date               : see VERSION_STRING below
* Description        : Main program body
********************************************************************************
* License: 3BSD
*******************************************************************************/

#define VERSION_STRING "V1.2.0 12/2009"


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "platform_config.h"
#include "timebase.h"
#include "term_io.h"

#include "myclasses.h"

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static Root root(1000);

/* External function prototypes ----------------------------------------------*/
extern "C" char* get_heap_end(void);
extern "C" char* get_stack_top(void);

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void cplusplustest(void);

/* Public functions -- -------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	uint32_t now, last, cnt;

	/* System Clocks Configuration */
	RCC_Configuration();

	/* NVIC configuration */
	NVIC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

	/* Turn on/off LED(s) */
	GPIO_SetBits(GPIO_LED, GPIO_Pin_LED1 /*| GPIO_Pin_LED4*/);

	/* Setup time-base (which uses the SysTick Timer) */
	if ( timebase_config() ) {
		/* Capture error */
		while (1) {;}
	}

	comm_init();

	xprintf("Hello from a C++ demo by Martin Thomas\nVersion " VERSION_STRING "\n");

	/* test newlib's stdio iprintf. remove to save flash-memory-space.
	 * stdio functions also need some stack space, so don't use
	 * them if the amount of free RAM is very limited */
	// iprintf("stdio iprintf is working %lu\n", timebase_get_count_ms());

	last = timebase_get_count_ms();

	cnt = 3;
	while(cnt) {
		now = timebase_get_count_ms();
		if ( timebase_abs_diff_ms(now, last) > 1000 ) {
			last = now;
			cplusplustest();
			cnt--;
		}
	}

	xprintf("Heap-Stack collision test\n");
	cnt=0;
	while(1) {
		uint32_t* ptr;
		ptr = new uint32_t;
		if (cnt < 200 ) delete ptr;
		xprintf("ghe()--> 0x%lx - 0x%lx <--gst(), cnt: %ld\n",
				(uint32_t)get_heap_end(), (uint32_t)get_stack_top(), ++cnt);
	}

	return 0;
}


/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : cplusplustest
* Description    : demo for some C++ features
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void cplusplustest(void)
{
	Root *pm1, *pm2, *pm;

	root.say_hello();
	pm1 = new Child1(2000);
	pm1->say_hello();
	pm2 = new Child2(3000);
	pm2->say_hello();
	pm1->say_hello();
	pm2->say_hello();
	pm = pm1;
	pm->say_hello();
	pm = pm2;
	pm->say_hello();
	delete pm1;
	delete pm2;
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();

  /* Enable USART1, GPIOA, GPIOD and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOx
                         | RCC_APB2Periph_AFIO, ENABLE);
  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Enable GPIO_LED clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_LED, ENABLE);

  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

#if defined(USE_STM3210B_EVAL) || defined(USE_EK_STM32F)
  /* Enable the USART2 Pins Software Remapping */
  GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#endif

  /* Configure USART1 TX (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 TX as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_TxPin;
  GPIO_Init(GPIOx_PF, &GPIO_InitStructure);

  /* Configure USART1 RX (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 RX as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_RxPin;
  GPIO_Init(GPIOx_PF, &GPIO_InitStructure);

  /* Configure GPIO for LEDs as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LED1 | GPIO_Pin_LED2 | GPIO_Pin_LED3 | GPIO_Pin_LED4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);

#if defined(USE_MINI_STM32)
  /* touch-controller's CS (PA4), SD-Card's CS (PB6) and DataFlash CS (PB7) pulled high = unselect */
  /* PB6 and PB7 have an external 4,7kOhm pull-up*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef VECT_TAB_RAM
/* vector-offset (TBLOFF) from bottom of SRAM. defined in linker script */
extern uint32_t _isr_vectorsram_offs;
void NVIC_Configuration(void)
{
  /* Set the Vector Table base location at 0x20000000+_isr_vectorsram_offs */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, (uint32_t)&_isr_vectorsram_offs);
}
#else
extern uint32_t _isr_vectorsflash_offs;
void NVIC_Configuration(void)
{
  /* Set the Vector Table base location at 0x08000000+_isr_vectorsflash_offs */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)&_isr_vectorsflash_offs);
}
#endif /* VECT_TAB_RAM */


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(const uint8_t* file, const uint8_t* function, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  xprintf("\n !!! Wrong parameter value detected\n");
  xprintf(" - file %s\n", file);
  xprintf(" - function %s\n", function);
  xprintf(" - line %lu\n", line);

#if 0
  /* Infinite loop */
  while (1)
  {
  }
#endif
}
#endif


