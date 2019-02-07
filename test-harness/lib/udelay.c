#include <stdint.h>
#include <hardware.h>
#include <hardware_vc4.h>

void udelay(uint32_t t) {
  uint32_t tv = ST_CLO;
  for (;;) {
    /* nop still takes a cycle i think? */
    __asm__ __volatile__ ("nop" :::);
    if ((ST_CLO - tv) > t)
      return;
  }
}
