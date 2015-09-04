#ifndef h64_h
#define h64_h

#include <stdio.h>
#include <string.h>
#include <mach/mach.h>
#include <capstone/capstone.h>

#define __DBG_ENABLED
#define OUTPUT_MSG(fmt, ...) fprintf(stdout, fmt " \n", ## __VA_ARGS__)

void hhook(void *target, void *repl, void **reentry);

#endif
