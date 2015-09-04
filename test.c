#include <string.h>
#include "hp.h"

int (*ptr)(const char *s);

void repl(const char *s)
{
  printf("hooked!\n");
  ptr(s); // call orig!
}

int main(void)
{
  hhook(puts, repl, (void**)&ptr);
  puts("test! puts() here.");

  return 0;
}
