/*
	
	This is a sample user program, which is going
	to be injected with a maliciously crafted dylib.

	With our example, puts() is going to be hijacked.
	- Check `testlib.c` for details -

*/

#include <stdio.h>

int main(void)
{

   	puts("hey im puts() from main()!");

	return(0);
}