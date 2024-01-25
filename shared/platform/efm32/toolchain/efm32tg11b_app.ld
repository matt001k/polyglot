/***************************************************************************//**
 * Linker script for Silicon Labs EFM32TG11B devices
 *******************************************************************************
 * # License
 * MIT
 *******************************************************************************
 *
 ******************************************************************************/
MEMORY
{
  BOOT (rx)     : ORIGIN = 0x00000000, LENGTH = 0x00008000
  APP (rx)      : ORIGIN = 0x00008000, LENGTH = 0x00018000
  RAM (rwx)     : ORIGIN = 0x20000000, LENGTH = 0x00007FF8
  SHARED (rwx)  : ORIGIN = 0x20007FF8, LENGTH = 0X00000008
}

/* Variables declared for use in application */
__bootrom_start__ = ORIGIN(BOOT);
__bootrom_size__ = LENGTH(BOOT);
__approm_start__ = ORIGIN(APP);
__approm_size__ = LENGTH(APP);
__flash_size__ = __bootrom_size__ + __approm_size__;
__flash_end__ = __bootrom_start__ + __flash_size__;

ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))
    __Vectors_End = .;
    __Vectors_Size = __Vectors_End - __Vectors;
    __end__ = .;

    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)

    KEEP(*(.eh_frame*))
  } > APP

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > APP

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > APP
  __exidx_end = .;

  /* To copy multiple ROM to RAM sections,
   * uncomment .copy.table section and,
   * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
  /*
  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;
    LONG (__etext)
    LONG (__data_start__)
    LONG (__data_end__ - __data_start__)
    LONG (__etext2)
    LONG (__data2_start__)
    LONG (__data2_end__ - __data2_start__)
    __copy_table_end__ = .;
  } > APP
  */

  /* To clear multiple BSS sections,
   * uncomment .zero.table section and,
   * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
  /*
  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    LONG (__bss_start__)
    LONG (__bss_end__ - __bss_start__)
    LONG (__bss2_start__)
    LONG (__bss2_end__ - __bss2_start__)
    __zero_table_end__ = .;
  } > APP
  */

  __etext = .;

  .data : AT (__etext)
  {
    __data_start__ = .;
    *(vtable)
    *(.data*)
    . = ALIGN (4);
    PROVIDE (__ram_func_section_start = .);
    *(.ram)
    PROVIDE (__ram_func_section_end = .);

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;

  } > RAM

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > RAM

  .heap (COPY):
  {
    __HeapBase = .;
    __end__ = .;
    end = __end__;
    _end = __end__;
    KEEP(*(.heap*))
    __HeapLimit = .;
  } > RAM

  /* .stack_dummy section doesn't contains any symbols. It is only
   * used for linker to calculate size of stack sections, and assign
   * values to stack symbols later */
  .stack_dummy (COPY):
  {
    KEEP(*(.stack*))
  } > RAM

  .shared (NOLOAD) :
  {
    . = ALIGN(2);
    __shared_start__ = .;
    KEEP(*(.shared*))
    . = ALIGN(2);
    __shared_end__ = .;
  } > SHARED

  /* Set stack top to end of RAM, and stack limit move down by
   * size of stack_dummy section */
  __StackTop = ORIGIN(RAM) + LENGTH(RAM);
  __StackLimit = __StackTop - SIZEOF(.stack_dummy);
  PROVIDE(__stack = __StackTop);

  /* Check if data + heap + stack exceeds RAM limit */
  ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

  /* Check if FLASH usage exceeds FLASH size */
  ASSERT( LENGTH(APP) >= (__etext - ORIGIN(APP) + SIZEOF(.data)), "APP memory overflowed !")
}
