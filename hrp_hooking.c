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

#include "hrp_hooking.h"

/* Err.. this seems half-broken, probably I didn't implement it nicely. Use the JMP for now. Will fix ASAP. */
int __throw_hook_with_push_ret(void *function, void *replacement_function, void **orig)
{

	#ifdef DBG_MODE
	fprintf(stderr, "{DBG_MODE} [Warning] : [!] This function may be broken. Please use JMP for avoiding errors.\n");
	fprintf(stdout, "{DBG_MODE} : [+] Throwing hook @ 0x%lX...\n", (uintptr_t)function);
	#endif

	if (function == NULL || replacement_function == NULL) {
		fprintf(stderr, "[!] I cannot throw hooks to NULL.\n");
		return(-1);
	}

	/* thx to @qwertyoruiop for push/ret */
	char jmp_shellcode[] = {
		'\x68', '\x00', '\x00', '\x00', '\x00', '\xc3'
	}; 
	
	int stolen_bytes = 0;

	char *func_ptr = (char*)function;
	char *original_prologue = malloc(32);
	
	vm_protect(mach_task_self(), (vm_address_t)function, 32, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE); 					// set maximum privileges for function vm area
	vm_protect(mach_task_self(), (vm_address_t)original_prologue, 32, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	while(stolen_bytes < PUSH_RET_SIZE-1) {

		AsmInstructionMatch* curInstr = possibleInstructions;
		bool r;

		while(1) {
			
			if ((r = codeMatchesInstruction((unsigned char*)func_ptr, curInstr)) == true)
				break;

			curInstr++;
		}

		if(r == false)
			fprintf(stderr, "[!] I cannot throw an hook to %p, unable to find space in the function prologue.\n", function);

		stolen_bytes += curInstr->length;
		func_ptr += curInstr->length;
	}

	#ifdef DBG_MODE
	fprintf(stdout, "{DBG_MODE} : [+] Space found in prologue. Now performing the jump...\n\n");
	#endif

	memcpy(original_prologue, (const void*)function, stolen_bytes);
	memcpy(jmp_shellcode + 1, (const void*)&replacement_function, sizeof(replacement_function));
	memcpy(function, jmp_shellcode, 6);

	char *trampoline_address = (char*)((int)function + (int)stolen_bytes);
	
	memcpy(jmp_shellcode+1, &trampoline_address, sizeof(trampoline_address));

	memcpy(original_prologue+stolen_bytes, (const void*)jmp_shellcode, sizeof(jmp_shellcode));
		
	*orig = trampoline_address;
	return 0;
}

int __throw_hook_with_jmp(void *function, void *replacement_function, void **orig)
{

	#ifdef DBG_MODE
	fprintf(stdout, "{DBG_MODE} : [+] Throwing hook @ 0x%lX...\n", (uintptr_t)function);
	#endif

	if (function == NULL || replacement_function == NULL) {
		fprintf(stderr, "[!] I cannot throw hooks to NULL.\n");
		return(-1);
	}

	int r_offset = replacement_function - function - JMP_SIZE;
	char *func_ptr = (char*)function;
	char *original_prologue = malloc(32);

	vm_protect(mach_task_self(), (vm_address_t)function, 32, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);
	vm_protect(mach_task_self(), (vm_address_t)original_prologue, 32, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	char jmp[] = {
		'\xE9', '\x00', '\x00', '\x00', '\x00'
	};

	int stolen_bytes = 0;

	while(stolen_bytes < JMP_SIZE) {

		AsmInstructionMatch* curInstr = possibleInstructions;
		bool r;

		while(1) {
			
			if ((r = codeMatchesInstruction((unsigned char*)func_ptr, curInstr)) == true)
				break;

			curInstr++;
		}

		if(r == false)
			fprintf(stderr, "[!] I cannot throw an hook to %p, unable to find space in the function prologue.\n", function);

		stolen_bytes += curInstr->length;
		func_ptr += curInstr->length;
	}

	#ifdef DBG_MODE
	fprintf(stdout, "{DBG_MODE} : [+] Space found in prologue. Now performing the jump...\n\n");
	#endif

	int *p = malloc(sizeof(r_offset));
	*p = r_offset;
	memcpy(jmp+1, (const void*)p, sizeof(r_offset));

	memcpy(original_prologue, function, stolen_bytes);
	memcpy(function, jmp, stolen_bytes); // call

	if(orig == NULL)
		return 0;

	char *trampoline_address = (char*)((uintptr_t)function + (uintptr_t)stolen_bytes);

	r_offset = trampoline_address - (original_prologue+stolen_bytes) - JMP_SIZE;
	*p = r_offset;
	memcpy(jmp+1, (const void*)p, sizeof(r_offset));

	memcpy(original_prologue+stolen_bytes, jmp, sizeof(jmp));

	*orig = original_prologue;

	return 0;
}

/* This function has been stolen from @qwertyoruiop's Haema. Thanks m8. */
intptr_t resolve_symbol(char* symname, unsigned int library_identifier) // compile with -Wno-deprecated
{
	if (library_identifier != -1)
		return (intptr_t)NSAddressOfSymbol(NSLookupSymbolInImage(_dyld_get_image_header(library_identifier), symname, 0));
	library_identifier++;
	intptr_t ret = 0;
	while (!(ret=(intptr_t)  NSAddressOfSymbol(NSLookupSymbolInImage(_dyld_get_image_header(library_identifier), symname, 0))))
		library_identifier++;
	return ret;
}

/* This has been stolen from rentzsch's older version of mach_override. What an ingrateful thief I am. Anyway, thanks also to rentzsch! */
static int codeMatchesInstruction(unsigned char *code, AsmInstructionMatch* instruction)
{
	int match = 1;

	int i;
	for (i=0; i<instruction->length; i++) {
			unsigned char mask = instruction->mask[i];
			unsigned char constraint = instruction->constraint[i];
			unsigned char codeValue = code[i];
			match = ((codeValue & mask) == constraint);
			if (!match) break;
	}

	return match;
}
