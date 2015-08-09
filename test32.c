#include <string.h>
#include "core/harpoon64.h"

int (*ptr)(const char *s);

void repl(const char *s)
{
  printf("hooked!\n");
  ptr(s); // call orig!
}

int main(void)
{	
  //throw_hook_i386(puts, repl, (void**)&ptr);
  puts("test");
  
  return 0;
}
