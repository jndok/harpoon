/*
	
	This is a sample malicious (or not maybe :P) dylib.
	Ready to be shoot into a running program.

	With our example, puts() is going to be hijacked.
	- Check `test.c` for details -

*/

#include "hrp_hooking.h"
#include <stdio.h>

static int (*orig)(void *p) = NULL; // prototype for puts()
void sample_hook(void *p)
{
	printf("I have hooked your puts() function! >:D\nNow calling original...\n\n");

	orig(p);
}

__attribute__((constructor)) void load()
{

	__throw_hook_with_jmp((void*)resolve_symbol("_puts", -1), sample_hook, (void**)&orig);

}