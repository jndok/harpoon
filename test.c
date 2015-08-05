#include "core/harpoon64.h"

int (*ptr)(const char *s);

void repl(const char *s)
{
  printf("hooked!\n");
  ptr(s); // call orig!
}

int main(void)
{	
  throw_hook(puts, repl, (void**)&ptr);
  puts("hey there! puts() here.");
  
  return 0;
}
