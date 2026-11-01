ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (rwx): ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
  .text : {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    *(.eh_frame)
    *(.ARM.exidx*)
    KEEP(*(.init))
    KEEP(*(.fini))
  } > FLASH

  .data : AT(ADDR(.text) + SIZEOF(.text)) {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  .bss : {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

 _estack = ORIGIN(RAM) + LENGTH(RAM);

 _sidata = LOADADDR(.data);

 _end = .;
}

/* Ensure startup symbols are provided for the reset code. */
PROVIDE(_sidata = LOADADDR(.data));
PROVIDE(_sdata  = ADDR(.data));
PROVIDE(_edata  = ADDR(.data) + SIZEOF(.data));
PROVIDE(_sbss   = ADDR(.bss));
PROVIDE(_ebss   = ADDR(.bss) + SIZEOF(.bss));
PROVIDE(_estack = ORIGIN(RAM) + LENGTH(RAM));
