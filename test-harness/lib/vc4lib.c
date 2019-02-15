#include <stdio.h>

void __attribute__((noreturn)) hang_cpu() {
	/* disable interrupts  */
	__asm__ __volatile__ ("di");

	/* loop */
	for (;;) {
		__asm__ __volatile__ ("nop");
	}
}

void __attribute__((noreturn)) _exit (int status)
{
  printf("exited: %d\n", status);
  register int r0 __asm__("r0") = status;
  __asm__ __volatile__ ("sleep" : : "r" (r0));
  for(;;) ; // no, we don't return.
}
