#include "vc4-stdlib.h"
#include "vc4-stdargs.h"
#include "vc4-hardware.h"
#include "current-tests.h"

#define BAUD_RATE 20

static const char *mess_first = "This is a test, this is only a test\nIf this had been something important, you'd know already\n";
static const char *mess_last = "Now turn off the machine, dummy!\n";

void tests();

void main() {
  volatile unsigned int vers;

  setup_uart(BAUD_RATE);
  xprintf("\n\n@dshadowwolfs and @jriwaneks RPi reverse-engineering test harness\n"
	  "Vesion 1.0-alpha1, (c) 2019 Daniel \"DshadowWolf\" Hazelton and Jasmine \"jriwanek\" Iwanek\n"
	  "Open Source - Released under the MIT License, see https://opensource.org/licenses/MIT\n");
  tests();
}


void tests() {
  test_version();
  test_btest();
  dump_otp_regs();
  dump_otp_data();
  dump_bootrom();
}

