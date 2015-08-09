#include <stdio.h>
#include <malloc/malloc.h>
#include "core/harpoon64.h"

int (*orig_ptr)(const char *s);

int hijacked_puts(const char *s)
{
  malloc_printf("hey puts()! you have been hooked!\n");
  return orig_ptr(s); // call original
}

__attribute__((constructor)) void load()
{
  throw_hook(puts, hijacked_puts, (void**)&orig_ptr);
}
