#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <hardware.h>
#include <bcm2708_chip/aux_io.h>
#include <test-helpers.h>

extern unsigned int _miniuart_settings;

__attribute__((noreturn)) void
_exit (int status)
{
  printf("exited: %d\n", status);
  register int r0 __asm__("r0") = status;
  __asm__ __volatile__ ("sleep" : : "r" (r0));
}

void setup_uart(unsigned int baud_reg) {
  MASK(GP_FSEL1, GP_FSEL1_FSEL14_CLR);
  MASK(GP_FSEL1, GP_FSEL1_FSEL15_CLR);
  SET(GP_FSEL1, 2, GP_FSEL1_FSEL14_LSB); // ALT Function 5, Pin 14
  SET(GP_FSEL1, 2, GP_FSEL1_FSEL15_LSB); // ditto, Pin 15 
  
  GP_PUD = 0;

  int a = 0;
  while(a++ < 151);
  GP_PUDCLK0 = 0x4000; //GP_CLK_PIN14
  while(a-- > 0);
  GP_PUDCLK0 = 0;
  
  HW_REGISTER_RW(AUX_ENABLES    ) = 1;
  HW_REGISTER_RW(AUX_MU_IER_REG ) = 0;
  HW_REGISTER_RW(AUX_MU_CNTL_REG) = 0;
  HW_REGISTER_RW(AUX_MU_LCR_REG ) = 3;
  HW_REGISTER_RW(AUX_MU_MCR_REG ) = 0;
  HW_REGISTER_RW(AUX_MU_IER_REG ) = 0;
  HW_REGISTER_RW(AUX_MU_IIR_REG ) = 0xC6;
  HW_REGISTER_RW(AUX_MU_BAUD_REG) = baud_reg;
  HW_REGISTER_RW(AUX_MU_LCR_REG ) = 3;
  HW_REGISTER_RW(AUX_MU_CNTL_REG) = 3;
}

void
newlines_on(void)
{
  struct termios t;
  tcgetattr(0, &t);
  t.c_oflag |= (ONLCR | OPOST);
  tcsetattr(0, TCSADRAIN, &t);
}

void __vc4_init(void) {
  setup_uart(20);
  newlines_on();
  _miniuart_settings = 7;
}

