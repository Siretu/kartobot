/*****************************************************************************
* Product: Generic exceptions for ARM with GNU toolset / C++ version
* Date of the Last Update:  Jun 13, 2007
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2007 Quantum Leaps, LLC. All rights reserved.
*
* Contact information:
* Web site: http://www.quantum-leaps.com
* e-mail:   info@quantum-leaps.com
*****************************************************************************/
#ifndef arm_exc_h
#define arm_exc_h

                                                /* interrupt locking policy */
#define ARM_INT_KEY_TYPE         int

#ifdef __thumb__

    #define ARM_INT_LOCK(key_)   ((key_) = ARM_int_lock_SYS())
    #define ARM_INT_UNLOCK(key_) (ARM_int_unlock_SYS(key_))

    extern "C" ARM_INT_KEY_TYPE ARM_int_lock_SYS(void);
    extern "C" void ARM_int_unlock_SYS(ARM_INT_KEY_TYPE key);

#else

    #define ARM_INT_LOCK(key_)   do { \
        asm("MRS %0,cpsr" : "=r" (key_)); \
        asm("MSR cpsr_c,#(0x1F | 0x80 | 0x40)"); \
    } while (0)

    #define ARM_INT_UNLOCK(key_) asm("MSR cpsr_c,%0" : : "r" (key_))

#endif


extern "C" void ARM_reset(void);
extern "C" void ARM_undef(void);
extern "C" void ARM_swi(void);
extern "C" void ARM_pAbort(void);
extern "C" void ARM_dAbort(void);
extern "C" void ARM_reserved(void);
extern "C" void ARM_irq(void);
extern "C" void ARM_fiq(void);


#endif                                                         /* arm_exc_h */

