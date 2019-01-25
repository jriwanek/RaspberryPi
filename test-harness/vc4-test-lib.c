#include "test-support.h"
#include "vc4-stdlib.h"

void test_version() {
  xprintf("test of \"version r0\" (or, alternatively, \"mov r0, cpuid\") operation\n");
  unsigned int version = get_version();
  xprintf("version: %x\n", version);
}

void test_btest() {
  xprintf("test of btest operation and results in various spots\n");
  volatile unsigned int no_match = btest_run(8,2), match = btest_run(8,3), ztest = btest_run(-127,31);
  xprintf("btest non-match sr: %x\nbtest match sr: %x\nbtest match sign-bit sr: %x\n", no_match, match, ztest);
}

void get_otp_reg() {
}

void dump_otp_data() {
}

void wake_otp() {
}

void dump_otp_regs() {
}

void sleep_otp() {
}

