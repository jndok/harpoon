#import <stdio.h>
#import "libhook.h"
void (*put_orig)(char*);

void my_puts(char* x) {
put_orig("hooked");
put_orig(x);
}

int main() {
throw_hook(puts, my_puts, &put_orig);
puts("hello");

}
