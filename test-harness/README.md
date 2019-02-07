# Test Harness for Reverse Engineering
This code forms a basic test-harness for doing some reverse-engineering work of the Raspberry Pi. What is here, aside from the dump of the OTP contents, is to help further the creation of a firmware for the Raspberry Pi that is as open as possible. The contributors to this code agree that there are large portions of the VideoCore side of the platform that implement patent-encumbered algorithms which require specific licensing payments, so we are trying to be careful to step around those parts. Other parts - such as the dump of the OTP contents - are done for consistency checks and to provide some information for trying to have means of determining the different boards.

Some of the dumped data is also being used to help build an accurate (if not cycle-accurate) emulation of the hardware, with a goal of providing a test platform for the firmware work so that SD Cards are not stressed and any potentially fatal errors do not damage actual hardware, especially ones that might be hard to acquire.

## The Tests Currently Implemented
Currently this test harness tests two things:
1) The results of ``version r0`` (in some disassemblers as ``mov r0, cpuid``)
2) How the ``btest`` operation functions¹

There are also 3 dumps of data currently implemented:
1) The default contents of the 10 OTP registers²
2) The current contents of all 127 OTP storage registers²
3) Raspberry Pi bootrom³

## Test Environment
The code runs in supervisor mode on the bare metal of the VC4, with the global pointer (r24, pointing to the 'data segment', in this case) aligned to start 0x1000 after the end of the code and the stack pointer (r25) aligned to start 0x10000 after the end of the data segment. Thanks to the use of @itzsor's vc4 toolchain we have a decently complete standard C library available, with all output and input redirecting through the mini-uart.

## Test "Library"
In 'include/' you'll find some custom headers defining things - some borrowed from @christinaa's RPi-OpenFirmware - and the entire required chunk of headers from BroadCom's code-dump. This should cover all of the hardware and access needed.

## Adding Tests
For a test that is possible with just C, all you need to do is write the test, add a declaration to ``current-tests.h`` and call the test from ``vc4-runner.c``'s ``tests()`` function. If you added the test as a new source file, you'll need to add that to the Makefile as well. Any extra peripheral registers should be added to ``system_defs.h``.

If you have a test that needs assembly, then add it to ``vc4-tests.s`` and follow the shown calling conventions... Or try adding it as an ``__asm__`` block, possibly ``__volatile__``.

## Footnotes
¹ The standard comparison operation - that is, ``cmp`` in assembly language - sets the zero flag in the status register to signal "equals". The available documentation for ``btest`` states that the zero flag will be set when the bit is not set. Two of these tests actually check that - the third checks to see if the ``Negative`` flag is set if you test the ``sign`` bit of a negative number. While this information is no longer specifically needed, the tests remain as something of a historical relic.

² None of the available documentation covers what the default values of these registers are, unlike almost all the other registers. This is needed for accurate emulation. The contents of the OTP memory itself is also needed for accurate emulation. None of the registers that are masked-out by the ``vgencmd otp_dump`` are recorded anywhere, as the information is masked-out for reason, possibly Broadcom mandated.

³ The bootrom is one of the key parts of the Raspberry Pi starting and is completely undocumented, to my knowledge. It may have some effect on the runtime environment - or even be referenced - from the code in ``bootcode.bin``. For this reason a dump is needed - one that can be converted back to the binary form - for accurate emulation.
