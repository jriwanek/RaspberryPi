OUTPUT_FORMAT("elf32-vc4")
OUTPUT_ARCH(vc4)
ENTRY(_start)

SECTIONS {
  . = 0x80000000;
  .text : {
    FILL(0x00);
    . = 0x200;
     *(.text.start)
     *(.text.main)
     *(.text.out_char)
     *(.text.out_string)
     *(.text.xprintf)
     *(.text.va_xprintf)
   }
   . = 0x8000c000;
   .data : {
     *(.bss)
     *(.rodata)
   }
}
