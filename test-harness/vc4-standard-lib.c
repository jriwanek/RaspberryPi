// at some point we need to change to having something closer
// to the broadcom dump, if not using the broadcom dump. For now, though...
#include "system_defs.h"
#include "vc4-stdargs.h"

#define is_lower(x) ((x) >= 'a' && (x) <= 'z')
#define is_upper(x) ((x) >= 'A' && (x) <= 'Z')
#define to_lower(x) (is_upper((x))?(x) + 32:(x))
#define is_digit(x) ((x) >= '0' && (x) <= '9')
#define is_hexdigit(x) ((to_lower((x)) >= 'a' && to_lower((x)) <= 'f') || is_digit((x)))
#define is_octal_digit(x) ((x) >= '0' && (x) <= '7')
#define conv_octdigit(x) (is_octal_digit((x))?(x) - '0':(x))
#define conv_hexdigit(x) (is_hexdigit((x))?is_digit((x))?(x) - 0:to_lower((x)) - 'W':0)

void out_char(const unsigned char ch) {
  if(ch == 0) return;
  if(ch == 0x0A) out_char(0x0D);
  // wait for the buffer to have space
  while(!(AUX_MU_LSR_REG & 0x20)) ;
    
  AUX_MU_IO_REG = ch;
}

static const char digits[16] = "0123456789ABCDEF";

void out_string(const char *str) {
  while(*str)
    out_char(*str++);
}

static void va_xprintf(const char *fmt, va_list args) {
  unsigned char c;
  char *p, buff[16];
  int j, r, i, s;
  
  for(;;) {
    c=*fmt++;
    s = 0;
    
    if(!c) break;
    
    if(c != '%') {
      out_char(c); continue;
    }

    c = *fmt++;
    switch(c) {
    case 's':
      p = va_arg(args, char *);
      out_string(p);
      continue;
    case 'c':
      out_char((const char)va_arg(args,int));
      continue;
    case 'd':
      s = 1;
    case 'u':
    case 'i':
      r = 10;
      break;
    case 'x':
      r = 16;
      break;
    case 'o':
      r = 8;
      break;
    case 'b':
      r = 2;
      break;
    default:
      out_char((const char)c);
      continue;
    }

    j = (unsigned int)va_arg(args, unsigned int);

    switch(r) {
    case 2:
      out_string("0b");
      break;
    case 8:
      out_char('o');
      break;
    case 16:
      out_string("0x");
      break;
    }

    i = 0;
    for(int k=0;k<16;k++) buff[k]=0;
    if(s && j & 0x8000000) { s = 2; j = 0 - j; }
    do {
      int w = j%r;
      j /=r;
      buff[i++] = digits[w];
    } while(j>0);
    if(s==2) buff[i++] = '-';
    j = i;
    while(j >= 0) out_char(buff[j--]);
  }
}

void xprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  va_xprintf(fmt, args);
  va_end(args);
}
