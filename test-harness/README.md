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
The code runs in supervisor mode on the bare metal of the VC4, with the global pointer (r24, pointing to the 'data segment', in this case) pointing to 0x8000c000 and the stack pointer (r25) given a starting value of 0x8000f000. All features of the base C language are available, but there is no "standard library" as might otherwise be expected. The mini-uart is active at 115200 baud and unless the PLL is initialized, the system runs at 19.2MHz.⁴ A variadic function is provided for formatted output, implementing all the expected backslash escapes⁵ and providing a small range of printf style formatters.⁶ There is no ``malloc()`` or any similar functions - at this point the project team is very small and focused on other tasks. There are some headers available, each specifically named and we do not set any ``-I`` flags, so they are not available for the "system include" style.

## Test "Library"
As mentioned in the "Test Environment" section there is a printf-like function provided, it has the following format/transposition options:
* ``c`` single character
* ``s`` string - can be space-padded (right aligned) by using a form similar to the classic C style - ie: ``%032s`` will left-pad the string with spaces for a field width of at least 32 characters - larger than that and the string will overflow the field.
* ``u`` or ``i`` unsigned integer - can be zero-padded following the same format as string output with the same caveat
* ``d`` signed integer (same as ``u`` or ``i``)
* ``x`` unsigned integer, as hexidecimal, leading ``0x``, zero-padding with all caveats
* ``X`` same as ``x`` but lacking the leading ``0x```, zero-padding with all caveats
* ``o`` like ``x`` but in octal, leading ``o```, zero-padding with all caveats
* ``O`` this is ``o`` but lacking the leading ``o```, zero-padding with all caveats
* ``b`` output as binary, leading ``0b```, zero-padding with all caveats
* ``B`` same as ``b`` but no leading ``0b```, zero-padding with all caveats
* ``%`` output a literal ``%``

Definition/description to be found in ``vc4-stdlib.h``

System varargs - the classic C varargs setup - are available (sans va_copy) in ``vc4-stdargs.h``.

Hardware register constants and access information currently in ``system_defs.h`` - this will change.

## Adding Tests
For a test that is possible with just C, all you need to do is write the test, add a declaration to ``current-tests.h`` and call the test from ``vc4-runner.c``'s ``tests()`` function. If you added the test as a new source file, you'll need to add that to the Makefile as well. Any extra peripheral registers should be added to ``system_defs.h``.

If you have a test that needs assembly, then add it to ``vc4-tests.s`` and follow the shown calling conventions... Or try adding it as an ``__asm__`` block, possibly ``__volatile__``.

## Footnotes
¹ The standard comparison operation - that is, ``cmp`` in assembly language - sets the zero flag in the status register to signal "equals". The available documentation for ``btest`` states that the zero flag will be set when the bit is not set. Two of these tests actually check that - the third checks to see if the ``Negative`` flag is set if you test the ``sign`` bit of a negative number. While this information is no longer specifically needed, the tests remain as something of a historical relic.
² None of the available documentation covers what the default values of these registers are, unlike almost all the other registers. This is needed for accurate emulation. The contents of the OTP memory itself is also needed for accurate emulation. None of the registers that are masked-out by the ``vgencmd otp_dump`` are recorded anywhere, as the information is masked-out for reason, possibly Broadcom mandated.
³ The bootrom is one of the key parts of the Raspberry Pi starting and is completely undocumented, to my knowledge. It may have some effect on the runtime environment - or even be referenced - from the code in ``bootcode.bin``. For this reason a dump is needed - one that can be converted back to the binary form - for accurate emulation.
⁴ Until the PLL is configured the system runs on the clock provided by a 19.2MHz crystal on the board.
⁵ As the code runs through the GNU build system, the "backslash escapes" are actually part of that system - even the GNU Assembler has automatic conversion of them.
⁶ These are somewhat compatible with classic printf formatters, but the provided xprintf is not printf and you should not expect them to always work the same or have the same features. See the "Test Library" section for details.
