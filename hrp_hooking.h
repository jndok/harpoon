/* 
*
*	This code is purely experimental and still in super duper alpha. Don't expect great stuff.
*	The code is also awful, sloppy and badly written. It was written in around 4 hours.
*  	Big cleanup and update coming soon.
*
* 	This code is released under the Apache License 2.0 –– http://www.apache.org/licenses/LICENSE-2.0.txt
*
*	---
*
*	Note:This version only works for x86. Check other brnch for x64.
*
*/

#if defined(__i386__)

#define DBG_MODE
#define PUSH_RET_SIZE 0x6
#define JMP_SIZE 0x5

/* Thanks to rentzsch for these! */

typedef struct {
	unsigned int length;
	unsigned char mask[15];
	unsigned char constraint[15];
} AsmInstructionMatch;

#if defined(__i386__)
static AsmInstructionMatch possibleInstructions[] = {
	{ 0x1, {0xFF}, {0x90} },                                                        // nop
	{ 0x1, {0xFF}, {0x55} },                                                        // push %esp
	{ 0x2, {0xFF, 0xFF}, {0x89, 0xE5} },                                            // mov %esp,%ebp
	{ 0x1, {0xFF}, {0x53} },                                                        // push %ebx
	{ 0x3, {0xFF, 0xFF, 0x00}, {0x83, 0xEC, 0x00} },                                // sub 0x??, %esp
	{ 0x1, {0xFF}, {0x57} },                                                        // push %edi
	{ 0x1, {0xFF}, {0x56} },                                                        // push %esi
	{ 0x0 }
};
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach-o/dyld.h>

int __throw_hook_with_push_ret(void *function, void *replacement_function, void **orig_ref);
int __throw_hook_with_jmp(void *function, void *replacement_function, void **orig);
intptr_t resolve_symbol(char* symname, unsigned int library_identifier);	// thx @qwertyoruiop!
static int codeMatchesInstruction(unsigned char *code, AsmInstructionMatch* instruction);
