/* Linker script for Cortex-M3
 *
 * Version:CodeSourcery Sourcery G++ Lite 2007q3-53
 * BugURL:https://support.codesourcery.com/GNUToolchain/
 *
 *  Copyright 2007 CodeSourcery.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply. */

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)
SEARCH_DIR(.)
/* --------------------------------------------------------
 * Libraries  
 * --------------------------------------------------------
 */
/*GROUP(-lgcc -lc)*/

MEMORY
{
  rom (rx)  : ORIGIN = 0x00002000, LENGTH = 504K
  ram (rwx) : ORIGIN = 0x10000000, LENGTH = 32K
  
  ram1(rwx) : ORIGIN = 0x2007C000, LENGTH = 16K
  ram2(rwx) : ORIGIN = 0x20080000, LENGTH = 16K
}

/* These force the linker to search for particular symbols from
 * the start of the link process and thus ensure the user's
 * overrides are picked up
 */
EXTERN(__cs3_reset_cortex_m)
EXTERN(__cs3_interrupt_vector_cortex_m)
EXTERN(__cs3_start_c main __cs3_stack __cs3_stack_size __cs3_heap_end)

/* Stack grows downwards from the end of the first (main) RAM block.
	 * Keep 32 bytes free for calls to the IAP flash programming functions.
	 * Keep another 16 bytes free to work around a GDB issue where it reads
	 * beyond the end of the stack.
	 * __stack_end__ = ORIGIN( ram ) + LENGTH( ram ) - 32 - 16;
	 */
PROVIDE(__cs3_stack = __cs3_region_start_ram + __cs3_region_size_ram - 32);
PROVIDE(__cs3_stack_size = __cs3_region_start_ram + __cs3_region_size_ram - _end);
/* PROVIDE(__cs3_heap_start = __heap_end__); */
PROVIDE(__cs3_heap_end = __ram1_start__ + __cs3_region_size_ram1);


SECTIONS
{
  .text :
  {
    CREATE_OBJECT_SYMBOLS
    __cs3_region_start_rom = .;
    *(.cs3.region-head.rom)
    
    __cs3_reset_vector = .;
    KEEP(*(.cs3.reset_vector))
    /* Make sure we pulled in an reset vector.  */
    ASSERT (. != __cs3_reset_vector, "No reset vector");
    
    *(.rom)
    *(.rom.b)

    __cs3_reset = .;
    KEEP(*(.cs3.reset))
    /* Make sure we pulled in some reset code.  */
    ASSERT (. != __cs3_reset, "No reset code");
    
    *(.text .text.* .gnu.linkonce.t.*)
    *(.plt)
    *(.gnu.warning)
    *(.glue_7t) *(.glue_7) *(.vfp11_veneer)

    *(.rodata .rodata.* .gnu.linkonce.r.*)

    *(.ARM.extab* .gnu.linkonce.armextab.*)
    *(.gcc_except_table)
    *(.eh_frame_hdr)
    *(.eh_frame)

    . = ALIGN(4);
    KEEP(*(.init))

    . = ALIGN(4);
    __preinit_array_start = .;
    KEEP (*(.preinit_array))
    __preinit_array_end = .;

    . = ALIGN(4);
    __init_array_start = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    __init_array_end = .;

    . = ALIGN(4);
    KEEP(*(.fini))

    . = ALIGN(4);
    __fini_array_start = .;
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    __fini_array_end = .;
    
    . = ALIGN(0x4);
    __ctors_start__ = .;
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))
    __ctors_end__ = .;
    
	. = ALIGN(0x4);
	__dtors_start__ = .; 
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))
    __dtors_end__ = .;
    
    *(.init .init.*)
	*(.fini .fini.*)

    . = ALIGN(4);
    __cs3_regions = .;
    LONG (0)
    LONG (__cs3_region_init_ram)
    LONG (__cs3_region_start_ram)
    LONG (__cs3_region_init_size_ram)
    LONG (__cs3_region_zero_size_ram)
  }

  /* .ARM.exidx is sorted, so has to go in its own output section.  */
  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } >rom
  __exidx_end = .;
  
  .text.align :
  {
    . = ALIGN(8);
    _etext = .;
  } >rom
  __cs3_region_size_rom = LENGTH(rom);
  __cs3_region_num = 1;

  .data :
  {
    __cs3_region_start_ram = .;
    *(.cs3.region-head.ram)
    
    __cs3_interrupt_vector = .;
    PROVIDE(__cs3_interrupt_vector = .);
    KEEP(*(.cs3.interrupt_vector))
    /* Make sure we pulled in an interrupt vector.  */
    ASSERT (. != __cs3_interrupt_vector, "No interrupt vector");
    
    KEEP(*(.jcr))
    *(.got.plt) *(.got)
    
    *(.shdata)
    *(.data .data.* .gnu.linkonce.d.*)
    *(.ram)
    . = ALIGN (8);
    _edata = .;
  } >ram AT>rom
  
  .bss :
  {
    __bss_start__ = .;
    *(.shbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
    *(.ram.b)
    . = ALIGN (8);
    __bss_end__ = .;
    _end = .;
    __end = .;
  } >ram AT>rom
  
  .stack (__cs3_stack - __cs3_stack_size) (NOLOAD):
  {
    *(.stack)
    _estack = .;
    PROVIDE(estack = .);
  } >ram
  
  .ram1 (NOLOAD):
  {
 	__ram1_start__ = .;
  	*(.ram1)
  	__ram1_end__ = .;
  } >ram1
  
  .heap (NOLOAD) :
  {
    *(.heap)
    . = ALIGN (8);
    __cs3_heap_start = .;
  } >ram1
  
  .ram2 (NOLOAD):
  {
  	__ram2_start__ = .;
  	/* . += 1; /* malloc may use 1 byte from here.. */
  	*(.ram2)
  	__ram2_end__ = .;
  } >ram2

  __cs3_region_init_ram = LOADADDR (.data);
  __cs3_region_init_size_ram = _edata - __cs3_region_start_ram;
  __cs3_region_zero_size_ram = _end - _edata;
  __cs3_region_size_ram = LENGTH(ram);
  __cs3_region_size_ram1 = LENGTH(ram1);
  __cs3_region_num = 1;

  .stab 0 (NOLOAD) : { *(.stab) }
  .stabstr 0 (NOLOAD) : { *(.stabstr) }
  /* DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the beginning
   * of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) }
}
