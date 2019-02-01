.macro SaveRegsLower 
	stm lr, (--sp)
	stm r0-r5, (--sp)
.endm

.macro SaveRegsUpper
	stm r6-r15, (--sp)
	stm r16-r23, (--sp)
.endm

.macro SaveRegsAll
	SaveRegsLower
	SaveRegsUpper
.endm

.macro RegExceptionHandler label, exception_number
	lea r2, except_\label
	st r2, (r1)
	add r1, #4
.endm

.macro ExceptionHandler label, exception_number
except_\label:
	SaveRegsLower
	mov r1, \exception_number
	b fatal_exception
.endm

	
	.section .text.fatal_exception,"ax",@progbits
	.p2align 1
	.globl fatal_exception
	.type fatal_exception, @function
fatal_exception:
	SaveRegsUpper
	mov r0, sp
	b fatal_exception_handler
	
	.section .text.setup_exception_table,"ax",@progbits
	.p2align 1
	.globl setup_exception_table
	.type setup_exception_table, @function
setup_exception_table:
	stm r0-r6,lr,(--sp)
	mov r3, #0x1B000
	mov r1, r3
	RegExceptionHandler zero, #0
	RegExceptionHandler misaligned, #1
	RegExceptionHandler dividebyzero, #2
	RegExceptionHandler undefinedinstruction, #3
	RegExceptionHandler forbiddeninstruction, #4
	RegExceptionHandler illegalmemory, #5
	RegExceptionHandler buserror, #6
	RegExceptionHandler floatingpoint, #7
	RegExceptionHandler isp, #8
	RegExceptionHandler dummy, #9
	RegExceptionHandler icache, #10
	RegExceptionHandler veccore, #11
	RegExceptionHandler badl2alias, #12
	RegExceptionHandler breakpoint, #13
	RegExceptionHandler unknown, #14
	RegExceptionHandler unknown, #15
	RegExceptionHandler unknown, #16
	RegExceptionHandler unknown, #17
	RegExceptionHandler unknown, #18
	RegExceptionHandler unknown, #19
	RegExceptionHandler unknown, #20
	RegExceptionHandler unknown, #21
	RegExceptionHandler unknown, #22
	RegExceptionHandler unknown, #23
	RegExceptionHandler unknown, #24
	RegExceptionHandler unknown, #25
	RegExceptionHandler unknown, #26
	RegExceptionHandler unknown, #27
	RegExceptionHandler unknown, #28
	RegExceptionHandler unknown, #29
	RegExceptionHandler unknown, #30
	RegExceptionHandler unknown, #31
	add r1, r3, #128
	lea r2, handle_irq_base
	add r4, r3, #572
	
L_setup_hw_irq:
	st r2, (r1)
	add r1, #4
	ble r1, r4, L_setup_hw_irq

	/*
	 * load the interrupt and normal stack pointers. these
	 * are chosen to be near the top of the available cache memory
	 */

	mov r28, irq_stack 
	ldm r0-r6,pc,(sp++)

handle_irq_base:
	SaveRegsAll

	/* top of savearea */
	mov r0, sp
	mov r1, r29
	bl handle_irq

return_from_exception:
	ldm r16-r23, (sp++)
	ldm r6-r15, (sp++)
	ldm r0-r5, (sp++)
	ld lr, (sp++)
	rti

	ExceptionHandler zero, #0
	ExceptionHandler misaligned, #1
	ExceptionHandler dividebyzero, #2
	ExceptionHandler undefinedinstruction, #3
	ExceptionHandler forbiddeninstruction, #4
	ExceptionHandler illegalmemory, #5
	ExceptionHandler buserror, #6
	ExceptionHandler floatingpoint, #7
	ExceptionHandler isp, #8
	ExceptionHandler dummy, #9
	ExceptionHandler icache, #10
	ExceptionHandler veccore, #11
	ExceptionHandler badl2alias, #12
	ExceptionHandler breakpoint, #13
	ExceptionHandler unknown, #14
	
