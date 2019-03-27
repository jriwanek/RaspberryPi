#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "current-tests.h"
#include "lib/udelay.h"

void tests();

void main() {
  printf("\r\n\r\n@dshadowwolfs and @jriwaneks RPi reverse-engineering test harness\r\n"
	  "Vesion 1.0-alpha3, (c) 2019 Daniel \"DshadowWolf\" Hazelton and Jasmine \"jriwanek\" Iwanek\r\n"
	  "Open Source - Released under the MIT License, see https://opensource.org/licenses/MIT\r\n");
  tests();
}


void tests() {
  test_version();
  test_btest();
  dump_otp_regs();
  dump_otp_data();
  dump_bootrom();
  dump_pll_regs();
  range_match_test();
  malloc_free_test();
}

