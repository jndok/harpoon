#include "harpoon.h"

typedef struct __attribute__((packed)) shellcode64 {
  uint16_t mov;
  uint64_t addr;
  uint16_t jmp;
} sh64;

typedef struct __attribute__((packed)) shellcode32 {
  uint8_t jmp;
  uint32_t off;
} sh32;

size_t eat_instructions(void *func)
{
  csh handle;
  cs_insn *insn;
  size_t cnt;
  size_t len_cnt = 0;

#if defined (__x86_64__)
  if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
    return 0;
#elif defined (__i386__)
  if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK)
    return 0;
#endif

  cnt = cs_disasm(handle, func, 0xF, 0x0, 0, &insn);
#if defined (__x86_64__)
  for (size_t k = 0;; k++) {
    if (len_cnt < JMP32_SHORT_SIZE + 0x3) {
      len_cnt+=insn[k].size;
    } else if (len_cnt >= JMP32_SHORT_SIZE + 0x3) {
      break;
    }
  }
#elif defined (__i386__)
  for (size_t k = 0;; k++) {
    if (len_cnt < JMP32_SHORT_SIZE) {
      len_cnt+=insn[k].size;
    } else if (len_cnt >= JMP32_SHORT_SIZE) {
      break;
    }
  }
#endif

  return len_cnt;
}

#if defined (__i386__)

void *trampoline = NULL;

void cleanup()
{
    if (trampoline != NULL) {
      free(trampoline);
    }
}

void throw_hook_i386(void *orig, void *repl, void **orig_ptr)
{
  size_t eaten = eat_instructions(orig);

  trampoline = (void*)malloc(0x20);

  vm_protect(mach_task_self(), (vm_address_t)orig, eaten, 0, VM_PROT_ALL);
  vm_protect(mach_task_self(), (vm_address_t)trampoline, 0x20, 0, VM_PROT_ALL);

  memcpy(trampoline, (const void*)orig, eaten);

  sh32 jshort;
  jshort.jmp = 0xe9;
  jshort.off = (uint32_t)repl - (uint32_t)orig - 0x5;

  memcpy(orig, (const void*)&jshort, eaten);

  jshort.off = (uint32_t)(orig+eaten) - (uint32_t)(trampoline+eaten) - 0x5;

  memcpy(trampoline+eaten, (const void*)&jshort, sizeof(sh32));

  if (orig_ptr != NULL)
    *orig_ptr = trampoline;

  vm_protect(mach_task_self(), (vm_address_t)orig, eaten, 0, VM_PROT_READ|VM_PROT_EXECUTE);
  vm_protect(mach_task_self(), (vm_address_t)trampoline, 0x20, 0, VM_PROT_READ|VM_PROT_EXECUTE);
}

#endif

#if defined (__x86_64__)

void *jzone = NULL, *trampoline = NULL;

static void *allocate_jump_zone(void *orig)
{
  void *z_ptr;
  int ret = 0;
  vm_address_t page = 0;

  vm_address_t f = ((uint64_t)orig & ~((0xFUL << 28) | (PAGE_SIZE - 1))) | (0x1UL << 31); /* first address in 32-bit address space */
  vm_address_t l = (uint64_t)orig & ~((0x1UL << 32) - 1);                                 /* last address in 32-bit address space */

  page = f;
  int allocated = 0;
  vm_map_t task_self = mach_task_self();

  /* allocating loop */
  while(!ret && !allocated && page != l) {
    ret = vm_allocate(task_self, &page, PAGE_SIZE, 0);
    if(ret == 0)
      allocated = 1;
    else if(ret == KERN_NO_SPACE) {
      page -= PAGE_SIZE;
      ret = 0;
    }
  }

  if(allocated)
    z_ptr = (void*) page;
  else if(!allocated && !ret)
    ret = KERN_NO_SPACE;

  return z_ptr;
}

static int deallocate_jump_zone(void *z_ptr)
{
  assert(z_ptr);

  int ret = 0;
  if((ret = vm_deallocate(mach_task_self(), (vm_address_t)z_ptr, PAGE_SIZE)) == 0)
    return 0;

  return -1;
}

void cleanup()
{
    deallocate_jump_zone(jzone);
    if (trampoline != NULL) {
      free(trampoline);
    }
}

void throw_hook(void *orig, void *repl, void **orig_ptr)
{
  size_t eaten = eat_instructions(orig);

  jzone = allocate_jump_zone(orig);
  trampoline = (void*)malloc(0x20);

  vm_protect(mach_task_self(), (vm_address_t)orig, eaten, 0, VM_PROT_ALL);
  vm_protect(mach_task_self(), (vm_address_t)jzone, 32, 0, VM_PROT_ALL);
  vm_protect(mach_task_self(), (vm_address_t)trampoline, 0x20, 0, VM_PROT_ALL);

  memcpy(trampoline, (const void*)orig, eaten);
  sh32 jshort;
  jshort.jmp = 0xe9;
  jshort.off = (uint32_t)jzone - (uint32_t)orig - 0x5;

  sh64 jlong;
  jlong.mov = 0xb848;
  jlong.addr = (uint64_t)repl;
  jlong.jmp = 0xe0ff;

  memcpy(orig, (const void*)&jshort, eaten);
  memcpy(jzone, (const void*)&jlong, sizeof(sh64));

  jlong.addr = (uint64_t)orig + eaten;

  memcpy(trampoline+eaten, (const void*)&jlong, sizeof(sh64));

  if (orig_ptr != NULL)
    *orig_ptr = trampoline;

  vm_protect(mach_task_self(), (vm_address_t)orig, eaten, 0, VM_PROT_READ|VM_PROT_EXECUTE);
  /* these cause trouble in dylib injecting */
  //vm_protect(mach_task_self(), (vm_address_t)jzone, 32, 0, VM_PROT_READ|VM_PROT_EXECUTE);
  //vm_protect(mach_task_self(), (vm_address_t)trampoline, 0x20, 0, VM_PROT_READ|VM_PROT_EXECUTE);
}

#endif
