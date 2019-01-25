	.section .text.start
	.global _start
_start:
	mov gp, 0x8000c000
	mov sp, 0x8000d5ac
	b main
	
