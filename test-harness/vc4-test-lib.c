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
  "A2W_SMPS_LDO0",   "A2W_SMPS_LDO1"
};

const char *a2w_pll_reg_names_94[] = {
  "A2W_PLLA_DIG0R", "A2W_PLLA_DIG1R", "A2W_PLLA_DIG2R", "A2W_PLLA_DIG3R", "A2W_PLLA_ANA0R", "A2W_PLLA_ANA1R", "A2W_PLLA_ANA2R", "A2W_PLLA_ANA3R",
  "A2W_PLLC_DIG0R", "A2W_PLLC_DIG1R", "A2W_PLLC_DIG2R", "A2W_PLLC_DIG3R", "A2W_PLLC_ANA0R", "A2W_PLLC_ANA1R", "A2W_PLLC_ANA2R", "A2W_PLLC_ANA3R",
  "A2W_PLLD_DIG0R", "A2W_PLLD_DIG1R", "A2W_PLLD_DIG2R", "A2W_PLLD_DIG3R", "A2W_PLLD_ANA0R", "A2W_PLLD_ANA1R", "A2W_PLLD_ANA2R", "A2W_PLLD_ANA3R",
  "A2W_PLLH_DIG0R", "A2W_PLLH_DIG1R", "A2W_PLLH_DIG2R", "A2W_PLLH_DIG3R", "A2W_PLLH_ANA0R", "A2W_PLLH_ANA1R", "A2W_PLLH_ANA2R", "A2W_PLLH_ANA3R",
  "A2W_HDMI_CTL0R", "A2W_HDMI_CTL1R", "A2W_HDMI_CTL2R", "A2W_HDMI_CTL3R", "A2W_XOSC0R",     "A2W_XOSC1R"
};

#define REG_OFFSET(base, regnum, size) (*(volatile unsigned int *)(base+(regnum*size)))
#define PLL_EX_REG(base, pll, reg) (*(volatile unsigned int *)(base+((reg*0x100)+(pll*0x20))))

void dump_pll_regs() {
  xprintf("\nA2W PLL DEFAULT REGISTER VALUES DUMP:\n");
  for(unsigned int x = 0; x < 50; x++)
    xprintf("\t%016s: 0x%08x\n", a2w_pll_reg_names_d4[x], REG_OFFSET(A2W_BASE, x, 4) & 0x00ffffff);

  xprintf("\n");

  const char *pll_regs_ex[][6] = {
      { "CTRL", "FRAC", "DSI0", "CORE", "PER", "CCP2" },
      { "CTRL", "FRAC", "CORE2", "CORE1", "PER", "CORE0" },
      { "CTRL", "FRAC", "DSI0", "CORE", "PER", "DSI1"},
      { "CTRL", "FRAC", "AUX", "RCAL", "PIX", "STS" }
  };

  for(int pll_reg = 0; pll_reg < 4; pll_reg++) {
    const char which_pll[] = { 'A', 'C', 'D', 'H' };
    const unsigned int mask[] = { 0x000373ffU, 0x000fffffU, 0x000003ffU, 0x000003ffU, 0x000003ffU, 0x000003ffU };
    for(int w_reg = 0; w_reg < 6; w_reg++) {
      xprintf("\tA2W_PLL%c_%s:\t0x%08x\n",  which_pll[pll_reg], pll_regs_ex[pll_reg][w_reg], PLL_EX_REG(0x7e102100, pll_reg, w_reg) & mask[w_reg]);
    }
  }
  
  xprintf("\n");
  xprintf("\tA2W_XOSC_CTRL:\t0x%08x\n", REG_OFFSET(0x7e102190, 0, 0));
  xprintf("\n");

  
  for(unsigned int x = 0; x < 38; x++)
    xprintf("\t%016sR: 0x%08x\n", a2w_pll_reg_names_94[x], REG_OFFSET(0x7e102800, x, 4) & 0x00ffffff);

  xprintf("\n");
  
}
