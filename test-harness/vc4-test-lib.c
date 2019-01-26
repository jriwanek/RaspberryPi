#include "system_defs.h"
#include "test-support.h"
#include "vc4-stdlib.h"

void test_version() {
  xprintf(">> test of \"version r0\" (or, alternatively, \"mov r0, cpuid\") operation\n");
  unsigned int version = get_version();
  xprintf("version:                 0x%08x\n", version);
}

void test_btest() {
  xprintf(">> test of btest operation and results in various spots\n");
  volatile unsigned int no_match = btest_run(8,2), match = btest_run(8,3), ztest = btest_run(-127,31);
  xprintf("btest non-match sr:      0x%08x\n"
	  "btest match sr:          0x%08x\n"
	  "btest match sign-bit sr: 0x%08x\n", no_match, match, ztest);
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
    xprintf("%3u: 0x%08x\n", i, reg);
  }
  sleep_otp();
}

void dump_otp_regs() {
  xprintf("OTP REGISTER DUMP:\n");
  xprintf("\tBOOTMODE:        0x%08x\n"
	  "\tCONFIG:          0x%08x\n", OTP_BOOTMODE_REG, OTP_CONFIG_REG);
  xprintf("\tCTRL_LO:         0x%08x\n"
	  "\tCNTRL_HI:        0x%08x\n", OTP_CTRL_LO_REG, OTP_CTRL_HI_REG);
  xprintf("\tBITSEL:          0x%08x\n"
	  "\tDATA:            0x%08x\n", OTP_BITSEL_REG, OTP_DATA_REG);
  xprintf("\tADDR:            0x%08x\n"
	  "\tWRITE_DATA_READ: 0x%08x\n", OTP_ADDR_REG, OTP_WRITE_DATA_READ_REG);
  xprintf("\tINIT_STATUS:     0x%08x\n", OTP_INIT_STATUS_REG);
}

#define MEM_AT_OFFSET(addr, base, offset) (*(volatile unsigned int*)((addr)+(base)+(offset)))
void dump_bootrom() {
  unsigned int base = 0x60000000;
  unsigned int max = 0x8000;
  xprintf("\nBOOTROM DUMP: (%08x to %08x)\n", base, (base+max));
  for(int i = 0; i < max; i += 16) {
    int a0 = MEM_AT_OFFSET(base, i, 0);
    int a4 = MEM_AT_OFFSET(base, i, 4);
    int a8 = MEM_AT_OFFSET(base, i, 8);
    int ac =  MEM_AT_OFFSET(base, i, 0xc);

    xprintf("0x%08x: %08X %08X %08X %08X\n", base + i, a0, a4, a8, ac);
  }
}
