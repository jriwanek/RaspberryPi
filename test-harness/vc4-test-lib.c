#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <test-helpers.h>
#include <hardware.h>
#include <bcm2708_chip/otp.h>
#include "test-support.h"
#include "lib/udelay.h"

void test_version() {
  printf(">> test of \"version r0\" (or, alternatively, \"mov r0, cpuid\") operation\r\n");
  unsigned int version = get_version();
  printf("version:                 0x%08x\r\n", version);
}

void test_btest() {
  printf(">> test of btest operation and results in various spots\r\n");
  volatile unsigned int no_match = btest_run(8,2), match = btest_run(8,3), ztest = btest_run(-127,31);
  printf("btest non-match sr:      0x%08x\r\n"
	  "btest match sr:          0x%08x\r\n"
	  "btest match sign-bit sr: 0x%08x\r\n", no_match, match, ztest);
}

void wake_otp() {
  OTP_CONFIG_REG = 3;
  udelay(0x14);
  OTP_CTRL_LO_REG = 0;
  OTP_CTRL_HI_REG = 0;
  udelay(0x14);
  OTP_CONFIG_REG = 2;
  udelay(0x14);
}

void sleep_otp() {
  OTP_CTRL_LO_REG = 0;
  OTP_CTRL_HI_REG = 0;
  udelay(0x14);
  OTP_CONFIG_REG = 0;
}

void dump_otp_data() {
  wake_otp();
  printf("OTP DATA DUMP:\r\n");
  for(int i = 0; i < 0x80; i++) {
    int reg;
    OTP_ADDR_REG = i;
    udelay(0x14);
    OTP_CTRL_HI_REG = 0;
    OTP_CTRL_LO_REG = 0;
    udelay(0x28);
    int r = OTP_CTRL_LO_REG;
    OTP_CTRL_LO_REG = 1;
    r = OTP_CTRL_LO_REG;
    while((OTP_STATUS_REG &1)==0) ;
    reg = OTP_DATA_REG;
    printf("%3u: 0x%08x\r\n", i, reg);
  }
  sleep_otp();
}

struct data_reg {
  const char *name;
  unsigned int addr;
  unsigned int mask;
};

struct data_reg otp_registers[] = {
  { "OTP_BOOTMODE_REG", 0x7e20f000, 0xffffffff }, { "OTP_CONFIG_REG",          0x7e20f004, 0x00000007 }, { "OTP_CTRL_LO_REG",     0x7e20f008, 0xffffffff },
  { "OTP_CTRL_HI_REG",  0x7e20f00c, 0x0000ffff }, { "OTP_BITSEL_REG",          0x7e20f010, 0xffffffff }, { "OTP_DATA_REG",        0x7e20f014, 0x0000001f },
  { "OTP_ADDR_REG",     0x7e20f018, 0x0000001f }, { "OTP_WRITE_DATA_READ_REG", 0x7e20f01c, 0xffffffff }, { "OTP_INIT_STATUS_REG", 0x7e20f020, 0xffffffff }
};

void dump_otp_regs() {
  printf("OTP REGISTER DUMP:\r\n");
  for( int i = 0; i < 9; i++ ) {
    struct data_reg reg = otp_registers[i];
    unsigned int raw_val = HW_REGISTER_RW(reg.addr);
    unsigned int mask = reg.mask;
    unsigned int actual = raw_val & mask;
    printf("\t%023s:\t0x%08x\r\n", reg.name, actual);
  }
}

#define MEM_AT_OFFSET(addr, base, offset) (*(volatile unsigned int*)((addr)+(base)+(offset)))
void dump_bootrom() {
  unsigned int base = 0x60000000;
  unsigned int max = 0x10000;
  printf("\r\nBOOTROM DUMP: (%08x to %08x)\r\n", base, (base+max));
  for(int i = 0; i < max; i += 16) {
    int a0 = MEM_AT_OFFSET(base, i, 0);
    int a4 = MEM_AT_OFFSET(base, i, 4);
    int a8 = MEM_AT_OFFSET(base, i, 8);
    int ac =  MEM_AT_OFFSET(base, i, 0xc);

    printf("0x%08x: %08X %08X %08X %08X\r\n", base + i, a0, a4, a8, ac);
  }
}

struct data_reg pll_registers[] ={
    { "A2W_PLLA_DIG0",   0x7e102000, 0x00ffffff }, { "A2W_PLLA_DIG1",   0x7e102004, 0x00ffffff }, { "A2W_PLLA_DIG2",   0x7e102008, 0x00ffffff },
    { "A2W_PLLA_DIG3",   0x7e10200c, 0x00ffffff }, { "A2W_PLLA_ANA0",   0x7e102010, 0x00ffffff }, { "A2W_PLLA_ANA1",   0x7e102014, 0x00ffffff }, 
    { "A2W_PLLA_ANA2",   0x7e102018, 0x00ffffff }, { "A2W_PLLA_ANA3",   0x7e10201c, 0x00ffffff }, { "A2W_PLLC_DIG0",   0x7e102020, 0x00ffffff }, 
    { "A2W_PLLC_DIG1",   0x7e102024, 0x00ffffff }, { "A2W_PLLC_DIG2",   0x7e102028, 0x00ffffff }, { "A2W_PLLC_DIG3",   0x7e10202c, 0x00ffffff },
    { "A2W_PLLC_ANA0",   0x7e102030, 0x00ffffff }, { "A2W_PLLC_ANA1",   0x7e102034, 0x00ffffff }, { "A2W_PLLC_ANA2",   0x7e102038, 0x00ffffff },
    { "A2W_PLLC_ANA3",   0x7e10203c, 0x00ffffff }, { "A2W_PLLD_DIG0",   0x7e102040, 0x00ffffff }, { "A2W_PLLD_DIG1",   0x7e102044, 0x00ffffff },
    { "A2W_PLLD_DIG2",   0x7e102048, 0x00ffffff }, { "A2W_PLLD_DIG3",   0x7e10204c, 0x00ffffff }, { "A2W_PLLD_ANA0",   0x7e102050, 0x00ffffff },
    { "A2W_PLLD_ANA1",   0x7e102054, 0x00ffffff }, { "A2W_PLLD_ANA2",   0x7e102058, 0x00ffffff }, { "A2W_PLLD_ANA3",   0x7e10205c, 0x00ffffff },
    { "A2W_PLLH_DIG0",   0x7e102060, 0x00ffffff }, { "A2W_PLLH_DIG1",   0x7e102064, 0x00ffffff }, { "A2W_PLLH_DIG2",   0x7e102068, 0x00ffffff },
    { "A2W_PLLH_DIG3",   0x7e10206c, 0x00ffffff }, { "A2W_PLLH_ANA0",   0x7e102070, 0x00ffffff }, { "A2W_PLLH_ANA1",   0x7e102074, 0x00ffffff },
    { "A2W_PLLH_ANA2",   0x7e102078, 0x00ffffff }, { "A2W_PLLH_ANA3",   0x7e10207c, 0x00ffffff }, { "A2W_HDMI_CTL0",   0x7e102080, 0x00ffffff },
    { "A2W_HDMI_CTL1",   0x7e102084, 0x00ffffff }, { "A2W_HDMI_CTL2",   0x7e102088, 0x00ffffff }, { "A2W_HDMI_CTL3",   0x7e10208c, 0x00ffffff },
    { "A2W_XOSC0",       0x7e102090, 0x00ffffff }, { "A2W_XOSC1",       0x7e102094, 0x00ffffff }, { "A2W_SMPS_CTLA0",  0x7e1020a0, 0x00ffffff },
    { "A2W_SMPS_CTLA1",  0x7e1020a4, 0x00ffffff }, { "A2W_SMPS_CTLA2",  0x7e1020a8, 0x00ffffff }, { "A2W_SMPS_CTLB0",  0x7e1020b0, 0x00ffffff },
    { "A2W_SMPS_CTLB1",  0x7e1020b4, 0x00ffffff }, { "A2W_SMPS_CTLB2",  0x7e1020b8, 0x00ffffff }, { "A2W_SMPS_CTLC0",  0x7e1020c0, 0x00ffffff }, 
    { "A2W_SMPS_CTLC1",  0x7e1020c4, 0x00ffffff }, { "A2W_SMPS_CTLC2",  0x7e1020c8, 0x00ffffff }, { "A2W_SMPS_CTLC3",  0x7e1020cc, 0x00ffffff },
    { "A2W_SMPS_LDO0",   0x7e1020d0, 0x00ffffff }, { "A2W_SMPS_LDO1",   0x7e1020d4, 0x00ffffff }, { "A2W_PLLA_CTRL",   0x7e102100, 0x000373ff },
    { "A2W_PLLA_FRAC",   0x7e102200, 0x000fffff }, { "A2W_PLLA_DSI0",   0x7e102300, 0x000003ff }, { "A2W_PLLA_CORE",   0x7e102400, 0x000003ff },
    { "A2W_PLLA_PER",    0x7e102500, 0x000003ff }, { "A2W_PLLA_CCP2",   0x7e102600, 0x000003ff }, { "A2W_PLLC_CTRL",   0x7e102120, 0x000373ff },
    { "A2W_PLLC_FRAC",   0x7e102220, 0x000fffff }, { "A2W_PLLC_CORE2",  0x7e102320, 0x000003ff }, { "A2W_PLLC_CORE1",  0x7e102420, 0x000003ff },
    { "A2W_PLLC_PER",    0x7e102520, 0x000003ff }, { "A2W_PLLC_CORE0",  0x7e102620, 0x000003ff }, { "A2W_PLLD_CTRL",   0x7e102140, 0x000373ff },
    { "A2W_PLLD_FRAC",   0x7e102240, 0x000fffff }, { "A2W_PLLD_DSI0",   0x7e102340, 0x000003ff }, { "A2W_PLLD_CORE",   0x7e102440, 0x000003ff },
    { "A2W_PLLD_PER",    0x7e102540, 0x000003ff }, { "A2W_PLLD_DSI1",   0x7e102640, 0x000003ff }, { "A2W_PLLH_CTRL",   0x7e102160, 0x000370ff },
    { "A2W_PLLH_FRAC",   0x7e102260, 0x000fffff }, { "A2W_PLLH_AUX",    0x7e102360, 0x000003ff }, { "A2W_PLLH_RCAL",   0x7e102460, 0x000003ff },
    { "A2W_PLLH_PIX",    0x7e102560, 0x000003ff }, { "A2W_PLLH_STS",    0x7e102660, 0xffffffff }, { "A2W_XOSC_CTRL",   0x7e102190, 0x0000037f },
    { "A2W_PLLA_DIG0R",  0x7e102800, 0x00ffffff }, { "A2W_PLLA_DIG1R",  0x7e102804, 0x00ffffff }, { "A2W_PLLA_DIG2R",  0x7e102808, 0x00ffffff },
    { "A2W_PLLA_DIG3R",  0x7e10280c, 0x00ffffff }, { "A2W_PLLA_ANA0R",  0x7e102810, 0x00ffffff }, { "A2W_PLLA_ANA1R",  0x7e102814, 0x00ffffff },
    { "A2W_PLLA_ANA2R",  0x7e102818, 0x00ffffff }, { "A2W_PLLA_ANA3R",  0x7e10281c, 0x00ffffff }, { "A2W_PLLC_DIG0R",  0x7e102820, 0x00ffffff },
    { "A2W_PLLC_DIG1R",  0x7e102824, 0x00ffffff }, { "A2W_PLLC_DIG2R",  0x7e102828, 0x00ffffff }, { "A2W_PLLC_DIG3R",  0x7e10282c, 0x00ffffff },
    { "A2W_PLLC_ANA0R",  0x7e102830, 0x00ffffff }, { "A2W_PLLC_ANA1R",  0x7e102834, 0x00ffffff }, { "A2W_PLLC_ANA2R",  0x7e102838, 0x00ffffff },
    { "A2W_PLLC_ANA3R",  0x7e10283c, 0x00ffffff }, { "A2W_PLLD_DIG0R",  0x7e102840, 0x00ffffff }, { "A2W_PLLD_DIG1R",  0x7e102844, 0x00ffffff },
    { "A2W_PLLD_DIG2R",  0x7e102848, 0x00ffffff }, { "A2W_PLLD_DIG3R",  0x7e10284c, 0x00ffffff }, { "A2W_PLLD_ANA0R",  0x7e102850, 0x00ffffff },
    { "A2W_PLLD_ANA1R",  0x7e102854, 0x00ffffff }, { "A2W_PLLD_ANA2R",  0x7e102858, 0x00ffffff }, { "A2W_PLLD_ANA3R",  0x7e10285c, 0x00ffffff },
    { "A2W_PLLH_DIG0R",  0x7e102860, 0x00ffffff }, { "A2W_PLLH_DIG1R",  0x7e102864, 0x00ffffff }, { "A2W_PLLH_DIG2R",  0x7e102868, 0x00ffffff },
    { "A2W_PLLH_DIG3R",  0x7e10286c, 0x00ffffff }, { "A2W_PLLH_ANA0R",  0x7e102870, 0x00ffffff }, { "A2W_PLLH_ANA1R",  0x7e102874, 0x00ffffff },
    { "A2W_PLLH_ANA2R",  0x7e102878, 0x00ffffff }, { "A2W_PLLH_ANA3R",  0x7e10287c, 0x00ffffff }, { "A2W_HDMI_CTL0R",  0x7e102880, 0x00ffffff },
    { "A2W_HDMI_CTL1R",  0x7e102884, 0x00ffffff }, { "A2W_HDMI_CTL2R",  0x7e102888, 0x00ffffff }, { "A2W_HDMI_CTL3R",  0x7e10288c, 0x00ffffff },
    { "A2W_XOSC0R",      0x7e102890, 0x00ffffff }, { "A2W_XOSC1R",      0x7e102894, 0x00ffffff }, { "A2W_SMPS_CTLA0R", 0x7e1028a0, 0x00ffffff },
    { "A2W_SMPS_CTLA1R", 0x7e1028a4, 0x00ffffff }, { "A2W_SMPS_CTLA2R", 0x7e1028a8, 0x00ffffff }, { "A2W_SMPS_CTLB0R", 0x7e1028b0, 0x00ffffff },
    { "A2W_SMPS_CTLB1R", 0x7e1028b4, 0x00ffffff }, { "A2W_SMPS_CTLB2R", 0x7e1028b8, 0x00ffffff }, { "A2W_SMPS_CTLC0R", 0x7e1028c0, 0x00ffffff },
    { "A2W_SMPS_CTLC1R", 0x7e1028c4, 0x00ffffff }, { "A2W_SMPS_CTLC2R", 0x7e1028c8, 0x00ffffff }, { "A2W_SMPS_CTLC3R", 0x7e1028cc, 0x00ffffff },
    { "A2W_SMPS_LDO0R",  0x7e1028d0, 0x00ffffff }, { "A2W_SMPS_LDO1R",  0x7e1028d4, 0x00ffffff }, { "A2W_PLLA_CTRLR",  0x7e102900, 0x000373ff },
    { "A2W_PLLA_FRACR",  0x7e102a00, 0x000fffff }, { "A2W_PLLA_DSI0R",  0x7e102b00, 0x000003ff }, { "A2W_PLLA_CORER",  0x7e102c00, 0x000003ff },
    { "A2W_PLLA_PERR",   0x7e102d00, 0x000003ff }, { "A2W_PLLA_CCP2R",  0x7e102e00, 0x000003ff }, { "A2W_PLLA_MULTI",  0x7e102f00, 0xffffffff },
    { "A2W_PLLC_CTRLR",  0x7e102920, 0x000373ff }, { "A2W_PLLC_FRACR",  0x7e102a20, 0x000fffff }, { "A2W_PLLC_CORE2R", 0x7e102b20, 0x000003ff },
    { "A2W_PLLC_CORE1R", 0x7e102c20, 0x000003ff }, { "A2W_PLLC_PERR",   0x7e102d20, 0x000003ff }, { "A2W_PLLC_CORE0R", 0x7e102e20, 0x000003ff },
    { "A2W_PLLC_MULTI",  0x7e102f20, 0xffffffff }, { "A2W_PLLD_CTRLR",  0x7e102940, 0x000373ff }, { "A2W_PLLD_FRACR",  0x7e102a40, 0x000fffff },
    { "A2W_PLLD_DSI0R",  0x7e102b40, 0x000003ff }, { "A2W_PLLD_CORER",  0x7e102c40, 0x000003ff }, { "A2W_PLLD_PERR",   0x7e102d40, 0x000003ff },
    { "A2W_PLLD_DSI1R",  0x7e102e40, 0x000003ff }, { "A2W_PLLD_MULTI",  0x7e102f40, 0xffffffff }, { "A2W_PLLH_CTRLR",  0x7e102960, 0x000370ff },
    { "A2W_PLLH_FRACR",  0x7e102a60, 0x000fffff }, { "A2W_PLLH_AUXR",   0x7e102b60, 0x000003ff }, { "A2W_PLLH_RCALR",  0x7e102c60, 0x000003ff },
    { "A2W_PLLH_PIXR",   0x7e102d60, 0x000003ff }, { "A2W_PLLH_STSR",   0x7e102e60, 0x000003ff }, { "A2W_XOSC_CTRLR",  0x7e102990, 0x0000037f },
    { "A2W_PLLH_MULTI",  0x7e102f60, 0xffffffff }
};

void dump_pll_regs() {
  printf("\r\nA2W PLL DEFAULT REGISTER VALUES DUMP:\r\n");
  for(unsigned int x = 0; x < 150; x++) {
    struct data_reg reg = pll_registers[x];
    unsigned int raw_reg_val = HW_REGISTER_RW(reg.addr);
    unsigned int reg_val_masked = raw_reg_val & reg.mask;
    printf("\t%016s: 0x%08x\r\n", reg.name, reg_val_masked);
  }
}

struct paired_reg {
  const char *name;
  unsigned int addr[2];
  unsigned int mask;
};

struct paired_reg match_regs[] = {
  { "OTP_BOOTMODE_REG",    { 0x7e20f000, 0xfe20f000 }, 0xffffffff }, { "OTP_CONFIG_REG",          { 0x7e20f004, 0xfe20f004 }, 0x00000007 },
  { "OTP_CTRL_LO_REG",     { 0x7e20f008, 0xfe20f008 }, 0xffffffff }, { "OTP_CTRL_HI_REG",         { 0x7e20f00c, 0xfe20f00c }, 0x0000ffff },
  { "OTP_BITSEL_REG",      { 0x7e20f010, 0xfe20f010 }, 0xffffffff }, { "OTP_DATA_REG",            { 0x7e20f014, 0xfe20f014 }, 0x0000001f },
  { "OTP_ADDR_REG",        { 0x7e20f018, 0xfe20f018 }, 0x0000001f }, { "OTP_WRITE_DATA_READ_REG", { 0x7e20f01c, 0xfe20f01c }, 0xffffffff },
  { "OTP_INIT_STATUS_REG", { 0x7e20f020, 0xf220f020 }, 0xffffffff }
};

void range_match_test() {
  printf("\r\nDoes 0xfe000000 - 0xffffffff match 0x7e000000 to 0x7fffffff ?\r\n");
  bool all_match = true;
  for(int i = 0; i < 9; i++) {
    struct paired_reg work = match_regs[i];
    unsigned int val_a = HW_REGISTER_RW(work.addr[0]) & work.mask;
    unsigned int val_b = HW_REGISTER_RW(work.addr[1]) & work.mask;
    bool match = (val_a == val_b);
    if(!match) all_match = false;
    printf("\t% 24s: 0x%08x == 0x%08x ? > %s\r\n", work.name, val_a, val_b, match?"TRUE":"FALSE");
  }
  printf("All Matched? %s\r\n", all_match?"TRUE":"FALSE");
}

void malloc_free_test() {
  char *buff = malloc(256);
  if(buff == NULL) {
    printf("malloc failed: %s\r\n", strerror(errno));
    return;
  }

  printf("buffer at %p\r\n", buff);
  free(buff);
}
