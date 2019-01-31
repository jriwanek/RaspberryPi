#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include "current-tests.h"

void tests();

void main() {
  printf("\r\n\r\n@dshadowwolfs and @jriwaneks RPi reverse-engineering test harness\r\n"
	  "Vesion 1.0-alpha1, (c) 2019 Daniel \"DshadowWolf\" Hazelton and Jasmine \"jriwanek\" Iwanek\r\n"
	  "Open Source - Released under the MIT License, see https://opensource.org/licenses/MIT\r\n");
  unsigned char *c = malloc(1);
  free(c);
  tests();
}


void tests() {
  test_version();
  test_btest();
  dump_otp_regs();
  dump_otp_data();
  dump_bootrom();
  dump_pll_regs();
}

