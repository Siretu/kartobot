/*****************************************************************************
* Product: BLINKY example application
* Date of the Last Update:  Jun 29, 2007
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2007 Quantum Leaps, LLC. All rights reserved.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_vectors)

MEMORY {                                       /* memory map of AT91SAM7S64 */
    ROM (rx)  : ORIGIN = 0x00100000, LENGTH = 64k
    RAM (rwx) : ORIGIN = 0x00200000, LENGTH = 16k
}

/* Sizes of the stacks used by the application. NOTE: you need to adjust */
C_STACK_SIZE   = 512;
IRQ_STACK_SIZE = 0;
FIQ_STACK_SIZE = 0;
SVC_STACK_SIZE = 0;
ABT_STACK_SIZE = 0;
UND_STACK_SIZE = 0;

/* The size of the heap used by the application. NOTE: you need to adjust   */
HEAP_SIZE = 0;

SECTIONS {

    .reset : {
        *startup.o (.text)  /* startup code (ARM vectors and reset handler) */
        . = ALIGN(0x4);
     } >ROM

    .ramvect : {                        /* used for vectors remapped to RAM */
        __ram_start = .;
        . = 0x40;
    } >RAM

    .fastcode : {
        __fastcode_load = LOADADDR (.fastcode);
        __fastcode_start = .;

        *(.glue_7t) *(.glue_7)
        *isr.o(.text.*)
        *(.text.fastcode)
        *(.text._ZN6Blinky8dispatchEv)                /* Blinky::dispatch() */
        /* add other modules here ... */

        . = ALIGN (4);
        __fastcode_end = .;
    } >RAM AT>ROM

    .text : {
        . = ALIGN(4);
        *(.text)                                   /* .text sections (code) */
        *(.text*)                                 /* .text* sections (code) */
        *(.rodata)           /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)         /* .rodata* sections (constants, strings, etc.) */
        *(.glue_7) /* glue arm to thumb (NOTE: placed already in .fastcode) */
        *(.glue_7t)/* glue thumb to arm (NOTE: placed already in .fastcode) */

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;                         /* global symbol at end of code */
    } >ROM

    .preinit_array : {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(SORT(.preinit_array.*)))
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >ROM

    .init_array : {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } >ROM

    .fini_array : {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(.fini_array*))
        KEEP (*(SORT(.fini_array.*)))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >ROM

    .data : {
        __data_load = LOADADDR (.data);
        __data_start = .;
        *(.data)                                          /* .data sections */
        *(.data*)                                        /* .data* sections */
        . = ALIGN(4);
        _edata = .;
    } >RAM AT>ROM

    .bss : {
        __bss_start__ = . ;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;                     /* define a global symbol at bss end */
        __bss_end__ = .;
    } >RAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );
    PROVIDE ( __end__ = _ebss );

    .heap : {
        __heap_start__ = . ;
        . = . + HEAP_SIZE;
        . = ALIGN(4);
        __heap_end__ = . ;
    } >RAM

    .stack : {
        __stack_start__ = . ;

        . += IRQ_STACK_SIZE;
        . = ALIGN (4);
        __irq_stack_top__ = . ;

        . += FIQ_STACK_SIZE;
        . = ALIGN (4);
        __fiq_stack_top__ = . ;

        . += SVC_STACK_SIZE;
        . = ALIGN (4);
        __svc_stack_top__ = . ;

        . += ABT_STACK_SIZE;
        . = ALIGN (4);
        __abt_stack_top__ = . ;

        . += UND_STACK_SIZE;
        . = ALIGN (4);
        __und_stack_top__ = . ;

        . += C_STACK_SIZE;
        . = ALIGN (4);
        __c_stack_top__ = . ;

        __stack_end__ = .;
    } >RAM

    /* Remove information from the standard libraries */
    /DISCARD/ : {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}
