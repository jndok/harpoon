#import <stdio.h>
#import "libhook.h"
void (*put_orig)(char*);

void my_puts(char* x) {
	put_orig("hooked");
	put_orig(x);
}

int main() {
	throw_hook((void*)puts, (void*)my_puts, (void**) &put_orig);
	puts("hello");
}
