#include <stdint.h>
#include <math.h>
#include <hardware.h>
#include <bcm2708_chip/aux_io.h>
#include <test-helpers.h>
#include <system-helpers.h>

static uint32_t system_clock = 19200000;
static uint32_t current_baud = 115200;

void uart_set_baud(uint32_t);

void uart_init(uint32_t clock, uint32_t baud) {
  system_clock = clock;
  MASK(GP_FSEL1, GP_FSEL1_FSEL14_CLR);
  MASK(GP_FSEL1, GP_FSEL1_FSEL15_CLR);
  SET(GP_FSEL1, 2, GP_FSEL1_FSEL14_LSB); // ALT Function 5, Pin 14
  SET(GP_FSEL1, 2, GP_FSEL1_FSEL15_LSB); // ditto, Pin 15 
  
  GP_PUD = 0;

  udelay(150);
  GP_PUDCLK0 = ((1<<14)|(1<<15)); // Disable Pull-up/Pull-down on pins 14 and 15
  udelay(150);
  GP_PUDCLK0 = 0;
  
  HW_REGISTER_RW(AUX_ENABLES    ) = 1;
  HW_REGISTER_RW(AUX_MU_IER_REG ) = 0;
  HW_REGISTER_RW(AUX_MU_CNTL_REG) = 0;
  HW_REGISTER_RW(AUX_MU_LCR_REG ) = 3;
  HW_REGISTER_RW(AUX_MU_MCR_REG ) = 0;
  HW_REGISTER_RW(AUX_MU_IER_REG ) = 0;
  HW_REGISTER_RW(AUX_MU_IIR_REG ) = 0xC6;
  HW_REGISTER_RW(AUX_MU_BAUD_REG) = baud;
  HW_REGISTER_RW(AUX_MU_LCR_REG ) = 3;
  HW_REGISTER_RW(AUX_MU_CNTL_REG) = 3;
}

void uart_set_baud(uint32_t baud) {
  current_baud = baud;
  uint32_t reg = ((uint32_t)ceil(((float)system_clock)/(((float)baud)*8)))-1;
  HW_REGISTER_RW(AUX_MU_BAUD_REG) = reg;
}

void uart_clock_reference(uint32_t sysclock) {
  system_clock = sysclock;
  uart_set_baud(current_baud);
}
