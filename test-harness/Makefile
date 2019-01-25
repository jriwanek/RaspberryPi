CFLAGS=-ffreestanding -nostdlib -nodefaultlibs -nostartfiles -O -ffunction-sections
LDFLAGS=-nostdlib -nostartfiles -T linker.lds
ASFLAGS=
OBJCOPYFLAGS=-O binary

CROSS=vc4-elf
CC=${CROSS}-gcc
LD=${CROSS}-ld
AS=${CROSS}-as
OBJCOPY=${CROSS}-objcopy

FILES=crt0.o vc4-standard-lib.o vc4-hardware.o vc4-tests.o vc4-test-lib.o vc4-runner.o

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

.s.o:
	${AS} ${ASFLAGS} $< -o $@

all: ${FILES}
	${LD} ${LDFLAGS} ${FILES} -o test-bootcode.elf
	${OBJCOPY} ${OBJCOPYFLAGS} test-bootcode.elf test-bootcode.bin

clean:
	@rm *.o
	@rm test-bootcode.elf test-bootcode.bin
