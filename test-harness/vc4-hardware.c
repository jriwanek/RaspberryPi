#include <stdbool.h>
#include <hardware.h>
#include <bcm2708_chip/aux_io.h>
#include <test-helpers.h>
#include <vc4-exception.h>
#include <uart.h>
#include <pll.h>
#include <sdram.h>
#include <init-funcs.h>
#include <stdio.h>
#include "lib/udelay.h"

uint32_t g_CPUID;
extern uint32_t irq_stack;

extern void setup_exception_table();
void setup_interrupts();

void __vc4_init(void) {
  setup_exception_table();
  setup_interrupts();
  __asm__ __volatile__("version r0\nmov r1, %0\nst r0, (r1)\n" : "=m" (g_CPUID) : : "r0", "r1");
  setup_pll();
  uart_init(250000000, 270);
  
  IC0_VADDR = 0x1B000;
  IC1_VADDR = 0x1B000;
  
  __asm__ __volatile__("ei");
  
  sdram_init();
}

__attribute__((noreturn)) void
_exit (int status)
{
  printf("exited: %d\n", status);
  register int r0 __asm__("r0") = status;
  __asm__ __volatile__ ("sleep" : : "r" (r0));
  for(;;); // make sure we don't return
}


#define mmio_read32(addr) HW_REGISTER_RW(addr)
#define mmio_write32(addr, value) (HW_REGISTER_RW(addr) = value)

void set_interrupt(int intno, bool enable, int core) {
  int base = (core == 0) ? IC0_BASE : IC1_BASE;

  int offset = 0x10 + ((intno >> 3) << 2);
  uint32_t slot = 0xF << ((intno & 7) << 2);

  uint32_t v = mmio_read32(base + offset) & ~slot;
  mmio_write32(base + offset, enable ? v | slot : v);
}

void setup_interrupts() {
  for(int i = 0; i < 64; ++i) {
    set_interrupt(i, (i != (125 - 64)) && (i != (121 - 64)) && (i != (120 - 64)) && (i != (73 - 64)) && (i != (96 - 64)), 0);
    set_interrupt(i, 0, 1);
  }
}
