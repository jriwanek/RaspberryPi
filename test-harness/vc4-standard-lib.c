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

typedef unsigned int size_t;

void out_char(const unsigned char ch) {  
  if(ch == 0x0A) out_char(0x0D);
  
  // wait for the buffer to have space
  while(!(AUX_MU_LSR_REG & 0x20)) ;
    
  AUX_MU_IO_REG = ch==0?'0':ch;
}

void out_string(const char *str) {
  while(*str)
    out_char(*str++);
}

// liberally borrowed and renamed from elsewhere
void va_xprintf (
	     const char*	fmt,	/* Pointer to the format string */
	     va_list arp			/* Pointer to arguments */
	     )
{
  unsigned int r, i, j, w, f;
  unsigned long v;
  char s[16], c, d, *p;


  for (;;) {
    c = *fmt++;					/* Get a char */
    if (!c) break;				/* End of format? */
    if (c != '%') {				/* Pass through it if not a % sequense */
      out_char(c); continue;
    }
    f = 0;
    c = *fmt++;					/* Get first char of the sequense */
    if (c == '0') {				/* Flag: '0' padded */
      f = 1; c = *fmt++;
    } else {
      if (c == '-') {			/* Flag: left justified */
	f = 2; c = *fmt++;
      }
    }
    for (w = 0; c >= '0' && c <= '9'; c = *fmt++)	/* Minimum width */
      w = w * 10 + c - '0';
    if (c == 'l' || c == 'L') {	/* Prefix: Size is long int */
      f |= 4; c = *fmt++;
    }
    if (!c) break;				/* End of format? */
    d = c;
    if (d >= 'a') d -= 0x20;
    switch (d) {				/* Type is... */
    case 'S' :					/* String */
      p = va_arg(arp, char*);
      for (j = 0; p[j]; j++) ;
      while (!(f & 2) && j++ < w) out_char(' ');
      out_string(p);
      while (j++ < w) out_char(' ');
      continue;
    case 'C' :					/* Character */
      out_char((char)va_arg(arp, int)); continue;
    case 'B' :					/* Binary */
      r = 2; break;
    case 'O' :					/* Octal */
      r = 8; break;
    case 'D' :					/* Signed decimal */
    case 'U' :					/* Unsigned decimal */
      r = 10; break;
    case 'X' :					/* Hexdecimal */
      r = 16; break;
    default:					/* Unknown type (passthrough) */
      out_char(c); continue;
    }

    /* Get an argument and put it in numeral */
    v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
    if (d == 'D' && (v & 0x80000000)) {
      v = 0 - v;
      f |= 8;
    }
    i = 0;
    do {
      d = (char)(v % r); v /= r;
      if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
      s[i++] = d + '0';
    } while (v && i < sizeof(s));
    if (f & 8) s[i++] = '-';
    j = i; d = (f & 1) ? '0' : ' ';
    while (!(f & 2) && j++ < w) out_char(d);
    do out_char(s[--i]); while(i);
    while (j++ < w) out_char(' ');
  }
}

void xprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  va_xprintf(fmt, args);
  va_end(args);
}

void *memcpy(void *dest, const void *src, size_t n) {
  char *d = dest;
  const char *s = src;
  
  for(int x = 0; x < n; x++) d[x] = s[x];

  return d;
}
