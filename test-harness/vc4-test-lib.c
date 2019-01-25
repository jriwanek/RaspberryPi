#include "system_defs.h"
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

void otp_wait(unsigned int cyc) {
  int c = 0;
  while( c < cyc ) c++;
}

void wake_otp() {
  OTP_CONFIG_REG = 3;
  otp_wait(0x14);
  OTP_CTRL_LO_REG = 0;
  OTP_CTRL_HI_REG = 0;
  otp_wait(0x14);
  OTP_CONFIG_REG = 2;
  otp_wait(0x14);
}

void sleep_otp() {
  OTP_CTRL_LO_REG = 0;
  OTP_CTRL_HI_REG = 0;
  otp_wait(0x14);
  OTP_CONFIG_REG = 0;
}

void dump_otp_data() {
  wake_otp();
  xprintf("OTP DATA DUMP:\n");
  for(int i = 0; i < 0x80; i++) {
    int reg;
    OTP_ADDR_REG = i;
    otp_wait(0x14);
    OTP_CTRL_HI_REG = 0;
    OTP_CTRL_LO_REG = 0;
    otp_wait(0x28);
    int r = OTP_CTRL_LO_REG;
    OTP_CTRL_LO_REG = 1;
    r = OTP_CTRL_LO_REG;
    while((OTP_STATUS_REG &1)==0) ;
    reg = OTP_DATA_REG;
    xprintf("%u: %x\n", i, reg);
  }
  sleep_otp();
}

void dump_otp_regs() {
  xprintf("OTP REGISTER DUMP:\n");
  xprintf("\tBOOTMODE: %x\n\tCONFIG: %x\n", OTP_BOOTMODE_REG, OTP_CONFIG_REG);
  xprintf("\tCTRL_LO: %x\n\tCNTRL_HI: %x\n", OTP_CTRL_LO_REG, OTP_CTRL_HI_REG);
  xprintf("\tBITSEL: %x\n\tDATA: %x\n", OTP_BITSEL_REG, OTP_DATA_REG);
  xprintf("\tADDR: %x\n\tWRITE_DATA_READ: %x\n", OTP_ADDR_REG, OTP_WRITE_DATA_READ_REG);
  xprintf("\tINIT_STATUS: %x\n", OTP_INIT_STATUS_REG);
}

