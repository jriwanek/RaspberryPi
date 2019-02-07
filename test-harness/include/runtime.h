#pragma once

#define ALWAYS_INLINE __attribute__((always_inline)) inline

void udelay(unsigned int);
void panic(const char *fmt, ...);
