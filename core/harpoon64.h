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
#include <malloc/malloc.h>
#include <assert.h>
#include <mach/mach.h>
#include <capstone/capstone.h>

#include "../ext/dbg/hdbg.h"

extern char SH_JMP64_LONG[JMP64_LONG_SIZE];
extern char SH_JMP32_SHORT[JMP32_SHORT_SIZE];
extern char JUMP_ZONE[JUMP_ZONE_SIZE];

static void copy_bytes(char *old, char *new, size_t size);                                    

static int make_zone_executable(void *z_ptr, size_t sz);                                      

static int allocate_jump_zone(void **z_ptr, void *orig);                                      
static int deallocate_jump_zone(void *z_ptr);                                                 
static int populate_jump_zone(void *z_ptr, char *jmp_shellcode, char *cooked, size_t ck_sz);  
static void set_jump_to_jump_zone(void *z_ptr, void *target);                                 

static void load_shellcode64(char *shellcode, void *to);                                      
static void load_shellcode32(char *shellcode, void *to);                                      

void throw_hook(void *orig, void *repl, void **orig_ptr); //exposed

#endif
