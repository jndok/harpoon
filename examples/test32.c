#include <string.h>
#include "../core/harpoon.h"

int (*ptr)(const char *s);

void repl(const char *s)
{
  printf("hooked!\n");
  ptr(s); // call orig!
  cleanup();
}

int main(void)
{
  //throw_hook_i386(puts, repl, (void**)&ptr);
  puts("test! puts() here.");

  return 0;
}
