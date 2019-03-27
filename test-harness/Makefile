CFLAGS=-O2 -I./include -I. -I./lib -I./hardware-support -static
LDFLAGS=-T ./linker.ld
ASFLAGS=
OBJCOPYFLAGS=-O binary

CROSS=vc4-elf
CC=${CROSS}-gcc
LD=${CROSS}-ld
AS=${CROSS}-as
OBJCOPY=${CROSS}-objcopy

FILES=lib/vc4lib.o lib/panic.o lib/udelay.o hardware-support/pll.o hardware-support/uart.o \
	hardware-support/vpustatus.o hardware-support/setup-functions.o hardware-support/sdram.o \
	vc4-hardware.o vc4-tests.o vc4-test-lib.o vc4-runner.o

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

.s.o:
	${AS} ${ASFLAGS} $< -o $@

all: ${FILES}
	${CC} ${CFLAGS} ${LDFLAGS} ${FILES} -o test-bootcode.elf -lm
	${OBJCOPY} ${OBJCOPYFLAGS} test-bootcode.elf test-bootcode.bin

clean:
	@find . -depth -type f -iname '*.o' -delete
	@if [ -e test-bootcode.elf ]; then rm test-bootcode.elf; fi;
	@if [ -e test-bootcode.bin ]; then rm test-bootcode.bin; fi;
