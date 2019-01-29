	.section .text.start
	.global _start
_start:
	mov gp, data_seg
	mov sp, stack_seg
	b main
	
