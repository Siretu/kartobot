//*****************************************************************************
//
// cpu.S - Instruction wrappers for special CPU instructions needed by the
//         drivers.
//
// Copyright (c) 2006 Luminary Micro, Inc.  All rights reserved.
//
// Software License Agreement
//
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's Stellaris Family of microcontroller products.
//
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  Any use in violation
// of the foregoing restrictions may subject the user to criminal sanctions
// under applicable laws, as well as to civil liability for the breach of the
// terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 852 of the Stellaris Driver Library.
//
//*****************************************************************************

#include "../asmdefs.h"

//*****************************************************************************
//
// The entire contents of this file go into the code section.
//
//*****************************************************************************
    __TEXT__

//*****************************************************************************
//
// Wrapper function for the CPSID instruction.
//
//*****************************************************************************
#if defined(GROUP_cpsid) || defined(BUILD_ALL)
    __EXPORT__ CPUcpsid
    __THUMB_LABEL__
CPUcpsid __LABEL__
    cpsid   i
    bx      lr
#endif

//*****************************************************************************
//
// Wrapper function for the CPSIE instruction.
//
//*****************************************************************************
#if defined(GROUP_cpsie) || defined(BUILD_ALL)
    __EXPORT__ CPUcpsie
    __THUMB_LABEL__
CPUcpsie __LABEL__
    cpsie   i
    bx      lr
#endif

//*****************************************************************************
//
// Wrapper function for the WFI instruction.
//
//*****************************************************************************
#if defined(GROUP_wfi) || defined(BUILD_ALL)
    __EXPORT__ CPUwfi
    __THUMB_LABEL__
CPUwfi __LABEL__
    wfi
    bx      lr
#endif

    __END__
