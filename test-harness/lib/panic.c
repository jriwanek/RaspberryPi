#include <stdio.h>
#include <vc4lib.h>
#include <stdarg.h>

void panic(const char* fmt,  ...) {
  printf("panic(): ");

  va_list arp;

  va_start(arp, fmt);
  vprintf(fmt, arp);
  va_end(arp);

  putchar('\n');

  hang_cpu();
}
