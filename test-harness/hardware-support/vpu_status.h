#pragma once
#include <stdint.h>

typedef struct {
	uint32_t r23;
	uint32_t r22;
	uint32_t r21;
	uint32_t r20;
	uint32_t r19;
	uint32_t r18;
	uint32_t r17;
	uint32_t r16;
	uint32_t r15;
	uint32_t r14;
	uint32_t r13;
	uint32_t r12;
	uint32_t r11;
	uint32_t r10;
	uint32_t r9;
	uint32_t r8;
	uint32_t r7;
	uint32_t r6;

	uint32_t r5;
	uint32_t r4;
	uint32_t r3;
	uint32_t r2;
	uint32_t r1;
	uint32_t r0;

	uint32_t lr;

	uint32_t sr;
	uint32_t pc;
} vc4_saved_state_t;

void fatal_exception_handler(vc4_saved_state_t* pcb, uint32_t n);

