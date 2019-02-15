OUTPUT_FORMAT("elf32-vc4", "elf32-vc4",
              "elf32-vc4")
OUTPUT_ARCH(vc4)
ENTRY(_start)
GROUP(-lgloss -lc)

MEMORY {
  ram (rwx) : ORIGIN = 00, LENGTH = 0x08000000
}

SECTIONS {
  .text : {
     . = . + 0x200;
     PROVIDE(_start = .);
     . = ALIGN(4);
     *(.text .stub .text.* .gnu.linkonce.t.*)
     *(.gnu.warning)
     KEEP (*(.init))
     KEEP (*(.fini))
     /* Constructors and destructors */
     KEEP (*crtbegin*.o(.ctors))
     KEEP (*(EXCLUDE_FILE (*crtend*.o ) .ctors))
     KEEP (*(SORT(.ctors.*)))
     KEEP (*(.ctors))
     KEEP (*crtbegin*.o(.dtors))
     KEEP (*(EXCLUDE_FILE (*crtend*.o ) .dtors))
     KEEP (*(SORT(.dtors.*)))
     KEEP (*(.dtors))
     KEEP (*(.jcr))
     _etext = .;
   } >ram =0

   /* Exception handlers */
   .eh_frame_hdr : { *(.eh_frame_hdr) } > ram
   .eh_frame : { KEEP (*(.eh_frame)) } > ram
   .gcc_except_table : { *(.gcc_except_table) *(.gcc_except_table.*) } > ram
    
   .rodata : {
     . = ALIGN(4);
     _frodata = .;
     *(.rodata .rodata.* .gnu.linkonce.r.*) 
     *(.rodata1)
     _erodata = .;
   } >ram
   
   . = ALIGN(0x1000);
   data_seg = .;
   .data : {
     . = ALIGN(4);
      _fdata = .;
     *(.data .data.* .gnu.linkonce.d.*)
     *(.data1)
     SORT(CONSTRUCTORS)
     *(.sdata .sdata.* .gnu.linkonce.s.*)
     _edata = .;
   } >ram

   .bss : {
     . = ALIGN(4);
     _fbss = .;
     *(.dynsbss)
     *(.sbss .sbss.* .gnu.linkonce.sb.*)
     *(.scommon)
     *(.dynbss)
     *(.bss .bss.* .gnu.linkonce.b.*)
     *(COMMON)
     . = ALIGN(4);
     _ebss = .;
     _end = .;
     PROVIDE (end = .);
   } >ram

   stack_seg = ALIGN(0x10000);
   PROVIDE(_fstack = stack_seg);
   . = . + 0x20000;
   PROVIDE(irq_stack = ALIGN(4));
   
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
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

}
