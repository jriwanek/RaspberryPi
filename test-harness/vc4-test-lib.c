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

static const char *a2w_pll_reg_names_d4[] = {
"A2W_PLLA_DIG0",   "A2W_PLLA_DIG1",   "A2W_PLLA_DIG2",   "A2W_PLLA_DIG3",   "A2W_PLLA_ANA0",   "A2W_PLLA_ANA1",   "A2W_PLLA_ANA2",   "A2W_PLLA_ANA3",
"A2W_PLLC_DIG0",   "A2W_PLLC_DIG1",   "A2W_PLLC_DIG2",   "A2W_PLLC_DIG3",   "A2W_PLLC_ANA0",   "A2W_PLLC_ANA1",   "A2W_PLLC_ANA2",   "A2W_PLLC_ANA3", 
"A2W_PLLD_DIG0",   "A2W_PLLD_DIG1",   "A2W_PLLD_DIG2",   "A2W_PLLD_DIG3",   "A2W_PLLD_ANA0",   "A2W_PLLD_ANA1",   "A2W_PLLD_ANA2",   "A2W_PLLD_ANA3",
"A2W_PLLH_DIG0",   "A2W_PLLH_DIG1",   "A2W_PLLH_DIG2",   "A2W_PLLH_DIG3",   "A2W_PLLH_ANA0",   "A2W_PLLH_ANA1",   "A2W_PLLH_ANA2",   "A2W_PLLH_ANA3",
"A2W_HDMI_CTL0",   "A2W_HDMI_CTL1",   "A2W_HDMI_CTL2",   "A2W_HDMI_CTL3",   "A2W_XOSC0",       "A2W_XOSC1",       "A2W_SMPS_CTLA0",  "A2W_SMPS_CTLA1",
"A2W_SMPS_CTLA2",  "A2W_SMPS_CTLB0",  "A2W_SMPS_CTLB1",  "A2W_SMPS_CTLB2",  "A2W_SMPS_CTLC0",  "A2W_SMPS_CTLC1",  "A2W_SMPS_CTLC2",  "A2W_SMPS_CTLC3",
"A2W_SMPS_LDO0",   "A2W_SMPS_LDO1",   "A2W_PLLA_CTRL",   "A2W_PLLA_FRAC",   "A2W_PLLA_DSI0",   "A2W_PLLA_CORE",   "A2W_PLLA_PER",    "A2W_PLLA_CCP2",
"A2W_PLLC_CTRL",   "A2W_PLLC_FRAC",   "A2W_PLLC_CORE2",  "A2W_PLLC_CORE1",  "A2W_PLLC_PER",    "A2W_PLLC_CORE0",  "A2W_PLLD_CTRL",   "A2W_PLLD_FRAC",
"A2W_PLLD_DSI0",   "A2W_PLLD_CORE",   "A2W_PLLD_PER",    "A2W_PLLD_DSI1",   "A2W_PLLH_CTRL",   "A2W_PLLH_FRAC",   "A2W_PLLH_AUX",    "A2W_PLLH_RCAL",
"A2W_PLLH_PIX",    "A2W_PLLH_STS",    "A2W_XOSC_CTRL",   "A2W_PLLA_DIG0R",  "A2W_PLLA_DIG1R",  "A2W_PLLA_DIG2R",  "A2W_PLLA_DIG3R",  "A2W_PLLA_ANA0R",
"A2W_PLLA_ANA1R",  "A2W_PLLA_ANA2R",  "A2W_PLLA_ANA3R",  "A2W_PLLC_DIG0R",  "A2W_PLLC_DIG1R",  "A2W_PLLC_DIG2R",  "A2W_PLLC_DIG3R",  "A2W_PLLC_ANA0R",
"A2W_PLLC_ANA1R",  "A2W_PLLC_ANA2R",  "A2W_PLLC_ANA3R",  "A2W_PLLD_DIG0R",  "A2W_PLLD_DIG1R",  "A2W_PLLD_DIG2R",  "A2W_PLLD_DIG3R",  "A2W_PLLD_ANA0R",
"A2W_PLLD_ANA1R",  "A2W_PLLD_ANA2R",  "A2W_PLLD_ANA3R",  "A2W_PLLH_DIG0R",  "A2W_PLLH_DIG1R",  "A2W_PLLH_DIG2R",  "A2W_PLLH_DIG3R",  "A2W_PLLH_ANA0R",
"A2W_PLLH_ANA1R",  "A2W_PLLH_ANA2R",  "A2W_PLLH_ANA3R",  "A2W_HDMI_CTL0R",  "A2W_HDMI_CTL1R",  "A2W_HDMI_CTL2R",  "A2W_HDMI_CTL3R",  "A2W_XOSC0R",
"A2W_XOSC1R",      "A2W_SMPS_CTLA0R", "A2W_SMPS_CTLA1R", "A2W_SMPS_CTLA2R", "A2W_SMPS_CTLB0R", "A2W_SMPS_CTLB1R", "A2W_SMPS_CTLB2R", "A2W_SMPS_CTLC0R",
"A2W_SMPS_CTLC1R", "A2W_SMPS_CTLC2R", "A2W_SMPS_CTLC3R", "A2W_SMPS_LDO0R",  "A2W_SMPS_LDO1R" };

#define REG_OFFSET(base, regnum, size) (*(volatile unsigned int *)(base+(regnum*size)))

void dump_pll_regs() {
  xprintf("\nA2W PLL DEFAULT REGISTER VALUES DUMP:\n");
  for(unsigned int x = 0; x < 125; x++)
    xprintf("\t%016s: 0x%08x\n", a2w_pll_reg_names_d4[x], REG_OFFSET(A2W_BASE, x, 4) & 0x00ffffff);
  xprintf("\n");
}
