/*****************************************************************************
 *   adc.c:  ADC C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.07.20  ver 1.00    Preliminary version, first Release
 *
*****************************************************************************/
#include "LPC18xx.h"			/* LPC13xx Peripheral Registers */
#include "adc.h"

volatile uint32_t ADC0Value[ADC_NUM], ADC1Value[ADC_NUM];
volatile uint32_t ADC0IntDone = 0, ADC1IntDone = 0;

#if BURST_MODE
volatile uint32_t channel0_flag, channel1_flag; 
#endif

#if ADC_INTERRUPT_FLAG
/******************************************************************************
** Function name:		ADC_IRQHandler
**
** Descriptions:		ADC interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void ADC0_IRQHandler (void) 
{
  uint32_t regVal;
  
  regVal = LPC_ADC0->STAT;		/* Read ADC will clear the interrupt */
  if ( regVal & 0x0000FF00 )	/* check OVERRUN error first */
  {
	regVal = (regVal & 0x0000FF00) >> 0x08;
	/* if overrun, just read ADDR to clear */
	/* regVal variable has been reused. */
	switch ( regVal )
	{
	  case 0x01:
		regVal = LPC_ADC0->DR[0];
		break;
	  case 0x02:
		regVal = LPC_ADC0->DR[1];
		break;
	  case 0x04:
		regVal = LPC_ADC0->DR[2];
		break;
	  case 0x08:
		regVal = LPC_ADC0->DR[3];
		break;
	  case 0x10:
		regVal = LPC_ADC0->DR[4];
		break;
	  case 0x20:
		regVal = LPC_ADC0->DR[5];
		break;
	  case 0x40:
		regVal = LPC_ADC0->DR[6];
		break;
	  case 0x80:
		regVal = LPC_ADC0->DR[7];
		break;
	  default:
		break;
	}
	LPC_ADC0->CR &= 0xF8FFFFFF;	/* stop ADC now */ 
	ADC0IntDone = 1;
	return;	
  }

  if ( regVal & ADC_ADINT )
  {
	switch ( regVal & 0xFF )	/* check DONE bit */
	{
	  case 0x01:
		ADC0Value[0] = ( LPC_ADC0->DR[0] >> 6 ) & 0x3FF;
	    break;
	  case 0x02:
		ADC0Value[1] = ( LPC_ADC0->DR[1] >> 6 ) & 0x3FF;
	    break;
	  case 0x04:
		ADC0Value[2] = ( LPC_ADC0->DR[2] >> 6 ) & 0x3FF;
	    break;
	  case 0x08:
		ADC0Value[3] = ( LPC_ADC0->DR[3] >> 6 ) & 0x3FF;
	    break;
	  case 0x10:
		ADC0Value[4] = ( LPC_ADC0->DR[4] >> 6 ) & 0x3FF;
	    break;
	  case 0x20:
		ADC0Value[5] = ( LPC_ADC0->DR[5] >> 6 ) & 0x3FF;
	    break;
	  case 0x40:
		ADC0Value[6] = ( LPC_ADC0->DR[6] >> 6 ) & 0x3FF;
	    break;
	  case 0x80:
		ADC0Value[7] = ( LPC_ADC0->DR[7] >> 6 ) & 0x3FF;
	    break;		
	  default:
	    break;
	}
#if BURST_MODE
	channel0_flag |= (regVal & 0xFF);
	if ( (channel0_flag & 0xFF) == 0xFF )
	{
	  /* All the bits in have been set, it indicates all the ADC 
	  channels have been converted. */
	  LPC_ADC0->CR &= 0xF8FFFFFF;	/* stop ADC now */ 
	  ADC0IntDone = 1;
	}
#else
	LPC_ADC0->CR &= 0xF8FFFFFF;	/* stop ADC now */ 
	ADC0IntDone = 1;
#endif
  }
  return;
}

/******************************************************************************
** Function name:		ADC_IRQHandler
**
** Descriptions:		ADC interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void ADC1_IRQHandler (void) 
{
  uint32_t regVal;
  
  regVal = LPC_ADC1->STAT;		/* Read ADC will clear the interrupt */
  if ( regVal & 0x0000FF00 )	/* check OVERRUN error first */
  {
	regVal = (regVal & 0x0000FF00) >> 0x08;
	/* if overrun, just read ADDR to clear */
	/* regVal variable has been reused. */
	switch ( regVal )
	{
	  case 0x01:
		regVal = LPC_ADC1->DR[0];
		break;
	  case 0x02:
		regVal = LPC_ADC1->DR[1];
		break;
	  case 0x04:
		regVal = LPC_ADC1->DR[2];
		break;
	  case 0x08:
		regVal = LPC_ADC1->DR[3];
		break;
	  case 0x10:
		regVal = LPC_ADC1->DR[4];
		break;
	  case 0x20:
		regVal = LPC_ADC1->DR[5];
		break;
	  case 0x40:
		regVal = LPC_ADC1->DR[6];
		break;
	  case 0x80:
		regVal = LPC_ADC1->DR[7];
		break;
	  default:
		break;
	}
	LPC_ADC1->CR &= 0xF8FFFFFF;	/* stop ADC now */ 
	ADC1IntDone = 1;
	return;	
  }

  if ( regVal & ADC_ADINT )
  {
	switch ( regVal & 0xFF )	/* check DONE bit */
	{
	  case 0x01:
		ADC1Value[0] = ( LPC_ADC1->DR[0] >> 6 ) & 0x3FF;
	    break;
	  case 0x02:
		ADC1Value[1] = ( LPC_ADC1->DR[1] >> 6 ) & 0x3FF;
	    break;
	  case 0x04:
		ADC1Value[2] = ( LPC_ADC1->DR[2] >> 6 ) & 0x3FF;
	    break;
	  case 0x08:
		ADC1Value[3] = ( LPC_ADC1->DR[3] >> 6 ) & 0x3FF;
	    break;
	  case 0x10:
		ADC1Value[4] = ( LPC_ADC1->DR[4] >> 6 ) & 0x3FF;
	    break;
	  case 0x20:
		ADC1Value[5] = ( LPC_ADC1->DR[5] >> 6 ) & 0x3FF;
	    break;
	  case 0x40:
		ADC1Value[6] = ( LPC_ADC1->DR[6] >> 6 ) & 0x3FF;
	    break;
	  case 0x80:
		ADC1Value[7] = ( LPC_ADC1->DR[7] >> 6 ) & 0x3FF;
	    break;		
	  default:
	    break;
	}
#if BURST_MODE
	channel1_flag |= (regVal & 0xFF);
	if ( (channel1_flag & 0xFF) == 0xFF )
	{
	  /* All the bits in have been set, it indicates all the ADC 
	  channels have been converted. */
	  LPC_ADC1->CR &= 0xF8FFFFFF;	/* stop ADC now */ 
	  ADC1IntDone = 1;
	}
#else
	LPC_ADC1->CR &= 0xF8FFFFFF;	/* stop ADC now */ 
	ADC1IntDone = 1;
#endif
  }
  return;
}
#endif

/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		None
** 
*****************************************************************************/
void ADCInit( uint32_t ADC_Clk )
{
  uint32_t i;

  for ( i = 0; i < ADC_NUM; i++ )
  {
	ADC0Value[i] = 0x0;
	ADC1Value[i] = 0x0;
  }
  /* Unlike some other pings, for ADC test, all the pins need
  to set to analog mode. Bit 7 needs to be cleared according 
  to design team. */

#if 0
  // Configure pinmux for ADC0
  scu_pinmux(0x3, 0, MD_PDN, FUNC2); 		// P3_0 func2=TX CLK
  scu_pinmux(0x6, 0, MD_PDN, FUNC0);		// P6_0 func0=RX CLK
  scu_pinmux(0x7, 2, MD_PDN, FUNC2);		// P7_2 func2=TX SDA
  scu_pinmux(0x6, 2, MD_PDN, FUNC0);		// P6_2 func0=RX SDA
  scu_pinmux(0x7, 1, MD_PDN, FUNC2);		// P7_1 func2=TX WS
  scu_pinmux(0x6, 1, MD_PDN, FUNC0);		// P6_1 func2=RX WS
  scu_pinmux(0x7, 1, MD_PDN, FUNC2);		// P7_1 func2=TX WS
  scu_pinmux(0x6, 1, MD_PDN, FUNC0);		// P6_1 func2=RX WS
#endif

  LPC_ADC0->CR = ( 0x01 << 0 ) |  /* SEL=1,select channel 0~7 on ADC0 */
	((M3Frequency/ADC_Clk-1)<<8) |  /* CLKDIV = Fpclk / 1000000 - 1 */ 
	( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
	( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
	( 1 << 21 ) |  		/* PDN = 1, normal operation */
	( 0 << 22 ) |  		/* TEST1:0 = 00 */
	( 0 << 24 ) |  		/* START = 0 A/D conversion stops */
	( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */ 

  LPC_ADC1->CR = ( 0x01 << 0 ) |  /* SEL=1,select channel 0~7 on ADC0 */
	((M3Frequency/ADC_Clk-1)<<8) |  /* CLKDIV = Fpclk / 1000000 - 1 */ 
	( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
	( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
	( 1 << 21 ) |  		/* PDN = 1, normal operation */
	( 0 << 22 ) |  		/* TEST1:0 = 00 */
	( 0 << 24 ) |  		/* START = 0 A/D conversion stops */
	( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */
  
  /* If POLLING, no need to do the following */
#if ADC_INTERRUPT_FLAG
  NVIC_EnableIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC1_IRQn);
  LPC_ADC0->INTEN = 0x1FF;		/* Enable all interrupts */
  LPC_ADC1->INTEN = 0x1FF;		/* Enable all interrupts */
#endif
  return;
}

/*****************************************************************************
** Function name:		ADCRead
**
** Descriptions:		Read ADC channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
** 
*****************************************************************************/
uint32_t ADCRead( uint32_t port_num, uint8_t channelNum )
{
#if !ADC_INTERRUPT_FLAG
  uint32_t regVal, ADC_Data;
#endif

  /* channel number is 0 through 7 */
  if ( channelNum >= ADC_NUM )
  {
	channelNum = 0;		/* reset channel number to 0 */
  }
  if ( port_num == 0 )
  {
    LPC_ADC0->CR &= 0xFFFFFF00;
    LPC_ADC0->CR |= (1 << 24) | (1 << channelNum);	
				/* switch channel,start A/D convert */
#if !ADC_INTERRUPT_FLAG
    while ( 1 )			/* wait until end of A/D convert */
    {
	  regVal = LPC_ADC0->DR[channelNum];
	  /* read result of A/D conversion */
	  if ( regVal & ADC_DONE )
	  {
	    break;
	  }
    }	
        
    LPC_ADC0->CR &= 0xF8FFFFFF;	/* stop ADC now */    
    if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
    {
	  return ( 0 );
    }
    ADC_Data = ( regVal >> 6 ) & 0x3FF;
    return ( ADC_Data );	/* return A/D conversion value */
#else
    return ( channelNum );	/* if it's interrupt driven, the ADC reading is 
							done inside the handler. so, return channel number */
#endif
  }
  else
  {
    LPC_ADC1->CR &= 0xFFFFFF00;
    LPC_ADC1->CR |= (1 << 24) | (1 << channelNum);	
				/* switch channel,start A/D convert */
#if !ADC_INTERRUPT_FLAG
    while ( 1 )			/* wait until end of A/D convert */
    {
	  regVal = LPC_ADC1->DR[channelNum];
	  /* read result of A/D conversion */
	  if ( regVal & ADC_DONE )
	  {
	    break;
	  }
    }	
        
    LPC_ADC1->CR &= 0xF8FFFFFF;	/* stop ADC now */    
    if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
    {
	  return ( 0 );
    }
    ADC_Data = ( regVal >> 6 ) & 0x3FF;
    return ( ADC_Data );	/* return A/D conversion value */
#else
    return ( channelNum );	/* if it's interrupt driven, the ADC reading is 
							done inside the handler. so, return channel number */
#endif
  }
}

/*****************************************************************************
** Function name:		ADC0BurstRead
**
** Descriptions:		Use burst mode to convert multiple channels once.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void ADCBurstRead( uint32_t port_num )
{
  if ( port_num == 0 )
  {
    /* Start bits need to be zero before BURST mode can be set. */
    if ( LPC_ADC0->CR & (0x7<<24) )
    {
	  LPC_ADC0->CR &= ~(0x7<<24);
    }
    LPC_ADC0->CR &= ~0xFF;
    /* Read all channels, 0 through 7. */
    LPC_ADC0->CR |= (0xFF);
    LPC_ADC0->CR |= (0x1<<16);		/* Set burst mode and start A/D convert */
  }
  else
  {
    /* Start bits need to be zero before BURST mode can be set. */
    if ( LPC_ADC1->CR & (0x7<<24) )
    {
	  LPC_ADC1->CR &= ~(0x7<<24);
    }
    LPC_ADC1->CR &= ~0xFF;
    /* Read all channels, 0 through 7. */
    LPC_ADC1->CR |= (0xFF);
    LPC_ADC1->CR |= (0x1<<16);		/* Set burst mode and start A/D convert */
  }
  return;						/* the ADC reading is done inside the 
								handler, return 0. */
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
