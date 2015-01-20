/*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
*         LUFA Library
* Copyright (C) Dean Camera, 2011.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* 
* Permission to use, copy, modify, and distribute this software
* and its documentation for any purpose is hereby granted without
* fee, provided that it is used in conjunction with NXP Semiconductors
* microcontrollers.  This copyright, permission, and disclaimer notice
* must appear in all copies of this code.
*/

#ifndef __ENDPOINT_LPC_H__
#define __ENDPOINT_LPC_H__

#if defined(__LPC18XX__)
	#include "DCD/LPC18XX/Endpoint_LPC18xx.h"
#elif defined(__LPC17XX__)
	#include "DCD/LPC17XX/Endpoint_LPC17xx.h"
#elif defined(__LPC11UXX__)
	#include "DCD/LPC11UXX/Endpoint_LPC11Uxx.h"
#endif

#include "../../../../Common/Common.h"
#include "../USBTask.h"
#include "../USBInterrupt.h"

/* Public Interface - May be used in end-application: */
/* Macros: */
/** \name Endpoint Bank Mode Masks */
//@{
/** Mask for the bank mode selection for the \ref Endpoint_ConfigureEndpoint() macro. This indicates
 *  that the endpoint should have one single bank, which requires less USB FIFO memory but results
 *  in slower transfers as only one USB device (the AVR or the host) can access the endpoint's
 *  bank at the one time.
 */
#define ENDPOINT_BANK_SINGLE                    (0 << 1)

/** Mask for the bank mode selection for the \ref Endpoint_ConfigureEndpoint() macro. This indicates
 *  that the endpoint should have two banks, which requires more USB FIFO memory but results
 *  in faster transfers as one USB device (the AVR or the host) can access one bank while the other
 *  accesses the second bank.
 */
#define ENDPOINT_BANK_DOUBLE                    (1 << 1)
//@}

#if (!defined(FIXED_CONTROL_ENDPOINT_SIZE) || defined(__DOXYGEN__))
/** Default size of the default control endpoint's bank, until altered by the control endpoint bank size
 *  value in the device descriptor. Not available if the \c FIXED_CONTROL_ENDPOINT_SIZE token is defined.
 */
#define ENDPOINT_CONTROLEP_DEFAULT_SIZE     64
#endif

/** Retrieves the maximum bank size in bytes of a given endpoint.
 *
 *  \note This macro will only work correctly on endpoint indexes that are compile-time constants
 *        defined by the preprocessor.
 *
 *  \param[in] EPIndex  Endpoint number, a value between 0 and (\ref ENDPOINT_TOTAL_ENDPOINTS - 1)
 */
#define ENDPOINT_MAX_SIZE(EPIndex)              512

#if !defined(CONTROL_ONLY_DEVICE) || defined(__DOXYGEN__)
/** Total number of endpoints (including the default control endpoint at address 0) which may
 *  be used in the device. Different USB AVR models support different amounts of endpoints,
 *  this value reflects the maximum number of endpoints for the currently selected AVR model.
 */
#define ENDPOINT_TOTAL_ENDPOINTS            ENDPOINT_DETAILS_MAXEP
#else
#define ENDPOINT_TOTAL_ENDPOINTS            1
#endif

/* Enums: */
/** Enum for the possible error return codes of the \ref Endpoint_WaitUntilReady() function.
 *
 *  \ingroup Group_EndpointRW_AVR8
 */
enum Endpoint_WaitUntilReady_ErrorCodes_t
{
	ENDPOINT_READYWAIT_NoError                 = 0, /**< Endpoint is ready for next packet, no error. */
	ENDPOINT_READYWAIT_EndpointStalled         = 1, /**< The endpoint was stalled during the stream
	 *   transfer by the host or device.
	 */
	ENDPOINT_READYWAIT_DeviceDisconnected      = 2,	/**< Device was disconnected from the host while
	 *   waiting for the endpoint to become ready.
	 */
	ENDPOINT_READYWAIT_BusSuspended            = 3, /**< The USB bus has been suspended by the host and
	 *   no USB endpoint traffic can occur until the bus
	 *   has resumed.
	 */
	ENDPOINT_READYWAIT_Timeout                 = 4, /**< The host failed to accept or send the next packet
	 *   within the software timeout period set by the
	 *   \ref USB_STREAM_TIMEOUT_MS macro.
	 */
};

/** Indicates the number of bytes currently stored in the current endpoint's selected bank.
 *
 *  \note The return width of this function may differ, depending on the maximum endpoint bank size
 *        of the selected AVR model.
 *
 *  \ingroup Group_EndpointRW_AVR8
 *
 *  \return Total number of bytes in the currently selected Endpoint's FIFO buffer.
 */
static inline uint16_t Endpoint_BytesInEndpoint(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint16_t Endpoint_BytesInEndpoint(void)
{
	return 0;
	// TODO not implemented yet
}

/** Get the endpoint address of the currently selected endpoint. This is typically used to save
 *  the currently selected endpoint number so that it can be restored after another endpoint has
 *  been manipulated.
 *
 *  \return Index of the currently selected endpoint.
 */
static inline uint8_t Endpoint_GetCurrentEndpoint(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint8_t Endpoint_GetCurrentEndpoint(void)
{
	return endpointselected;
}

/** Selects the given endpoint number. If the address from the device descriptors is used, the
 *  value should be masked with the \ref ENDPOINT_EPNUM_MASK constant to extract only the endpoint
 *  number (and discarding the endpoint direction bit).
 *
 *  Any endpoint operations which do not require the endpoint number to be indicated will operate on
 *  the currently selected endpoint.
 *
 *  \param[in] EndpointNumber Endpoint number to select.
 */
static inline void Endpoint_SelectEndpoint(const uint8_t EndpointNumber) ATTR_ALWAYS_INLINE;
static inline void Endpoint_SelectEndpoint(const uint8_t EndpointNumber)
{
	endpointselected = EndpointNumber;
	usb_data_buffer_index = 0;
}

/** Reads one byte from the currently selected endpoint's bank, for OUT direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \return Next byte in the currently selected endpoint's FIFO buffer.
 */
static inline uint8_t Endpoint_Read_8(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint8_t Endpoint_Read_8(void)
{
	uint8_t tem = usb_data_buffer[usb_data_buffer_index];
	usb_data_buffer_index++;

	return tem;
}

/** Determines the currently selected endpoint's direction.
 *
 *  \return The currently selected endpoint's direction, as a \c ENDPOINT_DIR_* mask.
 */
static inline uint8_t Endpoint_GetEndpointDirection(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint8_t Endpoint_GetEndpointDirection(void)
{
	return (endpointhandle[endpointselected]%2) ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
}

/** Determines if the currently selected endpoint may be read from (if data is waiting in the endpoint
 *  bank and the endpoint is an OUT direction, or if the bank is not yet full if the endpoint is an IN
 *  direction). This function will return false if an error has occurred in the endpoint, if the endpoint
 *  is an OUT direction and no packet (or an empty packet) has been received, or if the endpoint is an IN
 *  direction and the endpoint bank is full.
 *
 *  \ingroup Group_EndpointPacketManagement_AVR8
 *
 *  \return Boolean \c true if the currently selected endpoint may be read from or written to, depending
 *          on its direction.
 */
static inline bool Endpoint_IsReadWriteAllowed(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsReadWriteAllowed(void)
{
	return (Endpoint_GetEndpointDirection() == ENDPOINT_DIR_OUT) ? Endpoint_IsOUTReceived() : Endpoint_IsINReady() ;
}
/** Writes one byte to the currently selected endpoint's bank, for IN direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \param[in] Data  Data to write into the the currently selected endpoint's FIFO buffer.
 */
static inline void Endpoint_Write_8(const uint8_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_8(const uint8_t Data)
{
	usb_data_buffer[usb_data_buffer_index] = Data;
	usb_data_buffer_index++;
}

/** Discards one byte from the currently selected endpoint's bank, for OUT direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 */
static inline void Endpoint_Discard_8(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Discard_8(void)
{
	uint8_t dummy;
	dummy = Endpoint_Read_8();
}

/** Reads two bytes from the currently selected endpoint's bank in little endian format, for OUT
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \return Next two bytes in the currently selected endpoint's FIFO buffer.
 */
static inline uint16_t Endpoint_Read_16_LE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint16_t Endpoint_Read_16_LE(void)
{
	uint16_t tem = 0;
	uint8_t tem1,tem2;

	tem1 = Endpoint_Read_8();
	tem2 = Endpoint_Read_8();
	tem = (tem2<<8) | tem1;
	return tem;
}

/** Reads two bytes from the currently selected endpoint's bank in big endian format, for OUT
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \return Next two bytes in the currently selected endpoint's FIFO buffer.
 */
static inline uint16_t Endpoint_Read_16_BE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint16_t Endpoint_Read_16_BE(void)
{
	uint16_t tem = 0;
	uint8_t tem1,tem2;

	tem1 = Endpoint_Read_8();
	tem2 = Endpoint_Read_8();
	tem = (tem1<<8) | tem2;
	return tem;
}

/** Writes two bytes to the currently selected endpoint's bank in little endian format, for IN
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
 */
static inline void Endpoint_Write_16_LE(const uint16_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_16_LE(const uint16_t Data)
{
	Endpoint_Write_8(Data&0xFF);
	Endpoint_Write_8((Data>>8)&0xFF);
}

/** Writes two bytes to the currently selected endpoint's bank in big endian format, for IN
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
 */
static inline void Endpoint_Write_16_BE(const uint16_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_16_BE(const uint16_t Data)
{
	Endpoint_Write_8((Data>>8)&0xFF);
	Endpoint_Write_8(Data&0xFF);
}

/** Discards two bytes from the currently selected endpoint's bank, for OUT direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 */
static inline void Endpoint_Discard_16(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Discard_16(void)
{
	Endpoint_Read_8();
	Endpoint_Read_8();
}

/** Reads four bytes from the currently selected endpoint's bank in little endian format, for OUT
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \return Next four bytes in the currently selected endpoint's FIFO buffer.
 */
static inline uint32_t Endpoint_Read_32_LE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint32_t Endpoint_Read_32_LE(void)
{
	uint32_t tem = 0;
	uint8_t tem1,tem2,tem3,tem4;

	tem1 = Endpoint_Read_8();
	tem2 = Endpoint_Read_8();
	tem3 = Endpoint_Read_8();
	tem4 = Endpoint_Read_8();
	tem = (tem4<<24) |(tem3<<16) |(tem2<<8) | tem1;
	return tem;
}

/** Reads four bytes from the currently selected endpoint's bank in big endian format, for OUT
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \return Next four bytes in the currently selected endpoint's FIFO buffer.
 */
static inline uint32_t Endpoint_Read_32_BE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint32_t Endpoint_Read_32_BE(void)
{
	uint32_t tem = 0;
	uint8_t tem1,tem2,tem3,tem4;

	tem1 = Endpoint_Read_8();
	tem2 = Endpoint_Read_8();
	tem3 = Endpoint_Read_8();
	tem4 = Endpoint_Read_8();
	tem = (tem1<<24) |(tem2<<16) |(tem3<<8) | tem4;
	return tem;
}

/** Writes four bytes to the currently selected endpoint's bank in little endian format, for IN
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
 */
static inline void Endpoint_Write_32_LE(const uint32_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_32_LE(const uint32_t Data)
{
	Endpoint_Write_8(Data&0xFF);
	Endpoint_Write_8((Data>>8)&0xFF);
	Endpoint_Write_8((Data>>16)&0xFF);
	Endpoint_Write_8((Data>>24)&0xFF);
}

/** Writes four bytes to the currently selected endpoint's bank in big endian format, for IN
 *  direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 *
 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
 */
static inline void Endpoint_Write_32_BE(const uint32_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_32_BE(const uint32_t Data)
{
	Endpoint_Write_8((Data>>24)&0xFF);
	Endpoint_Write_8((Data>>16)&0xFF);
	Endpoint_Write_8((Data>>8)&0xFF);
	Endpoint_Write_8(Data&0xFF);
}

/** Discards four bytes from the currently selected endpoint's bank, for OUT direction endpoints.
 *
 *  \ingroup Group_EndpointPrimitiveRW_AVR8
 */
static inline void Endpoint_Discard_32(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Discard_32(void)
{
	Endpoint_Read_8();
	Endpoint_Read_8();
	Endpoint_Read_8();
	Endpoint_Read_8();
}

void Endpoint_GetSetupPackage(uint8_t* pData);

#endif
