#include "system_defs.h"

void setup_uart(unsigned int baud_reg) {
  MASK(GPFSEL1, GP_FSEL14_CLEAR);
  MASK(GPFSEL1, GP_FSEL15_CLEAR);
  SET(GPFSEL1, ALT_FUNCTION_5, GP_PIN14);
  SET(GPFSEL1, ALT_FUNCTION_5, GP_PIN15);
  
  GPPUD = 0;

  int a = 0;
  while(a++ < 151);
  GPPUDCLK0 = 0x4000; //GP_CLK_PIN14
  while(a-- > 0);
  GPPUDCLK0 = 0;
  
  AUX_ENABLES = 1;
  AUX_MU_IER_REG = 0;
  AUX_MU_CNTL_REG = 0;
  AUX_MU_LCR_REG = 3;
  AUX_MU_MCR_REG = 0;
  AUX_MU_IER_REG = 0;
  AUX_MU_IIR_REG = 0xC6;
  AUX_MU_BAUD_REG = baud_reg;
  AUX_MU_LCR_REG = 3;
  AUX_MU_CNTL_REG = 3;
}
