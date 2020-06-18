
/* The entry point of the kernel (see boot.S) */
ENTRY(Start)

SECTIONS
{
  .text ALIGN (CONSTANT (COMMONPAGESIZE)) :
  {
    KEEP (*(.text.boot))
    *(.text .text.*)
  }

  .rodata ALIGN (CONSTANT (COMMONPAGESIZE)) :
  {
    *(.rodata .rodata.*)
  }

  .preinit_array ALIGN (CONSTANT (COMMONPAGESIZE)) :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  }
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
  }
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);
  }

  .data ALIGN (CONSTANT (COMMONPAGESIZE)) :
  {
    *(.data)
  }

  .bss ALIGN (CONSTANT (COMMONPAGESIZE)) :
  {
    PROVIDE_HIDDEN (__bss_start = .);
    *(.bss .bss.*)
    PROVIDE_HIDDEN (__bss_end = .);
  }

  . = ALIGN (CONSTANT (COMMONPAGESIZE));
  PROVIDE_HIDDEN (__stack_start = .);

  . = ALIGN (CONSTANT (COMMONPAGESIZE));
  __end = .;
}
