/*
    ██╗  ██╗ █████╗ ██████╗ ██████╗  ██████╗  ██████╗ ███╗   ██╗
    ██║  ██║██╔══██╗██╔══██╗██╔══██╗██╔═══██╗██╔═══██╗████╗  ██║
    ███████║███████║██████╔╝██████╔╝██║   ██║██║   ██║██╔██╗ ██║
    ██╔══██║██╔══██║██╔══██╗██╔═══╝ ██║   ██║██║   ██║██║╚██╗██║
    ██║  ██║██║  ██║██║  ██║██║     ╚██████╔╝╚██████╔╝██║ ╚████║
    ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝      ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝ =(1.0 reborn)=

                              ---

  == Notes ==
  harpoon is still under development. Expect bugs, strange behavior and
  general problems. This version still lacks error-checking, proper memory
  management and other things.

  harpoon will be continuously updated in future, to guarantee functionality
  with newer OSX versions, and for bug fixes.

  I hope you can find it useful and suitable to your needs.

  ----

  also, feel free to contribute by forking harpoon and submitting updates,
  or simply by opening issues in GitHub.

*/

#ifndef h64_h
#define h64_h

//#pragma GCC poison printf
//#pragma message("this version is still unstable and may behave strangely, so be cautious.")

#define JUMP_ZONE_SIZE 0x20
#define JMP64_LONG_SIZE 0xC
#define JMP32_SHORT_SIZE 0x5

#define NOP_INSN 0x90

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <malloc/malloc.h>
#include <assert.h>
#include <mach/mach.h>
#include <capstone/capstone.h>

#include "../ext/dbg/hdbg.h"

static void *allocate_jump_zone(void *orig);
static int deallocate_jump_zone(void *z_ptr);

void cleanup(); 

void throw_hook(void *orig, void *repl, void **orig_ptr); //exposed
void throw_hook_i386(void *orig, void *repl, void **orig_ptr); //exposed

#endif
