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

#include <stdio.h>
#include <string.h>
#include <malloc/malloc.h>
#include <assert.h>
#include <mach/mach.h>
#include <mach/mach_types.h>
#include <capstone/capstone.h>

void throw_hook(void *orig, void *repl, void **orig_ptr); //exposed

#endif
