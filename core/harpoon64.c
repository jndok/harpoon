#include "harpoon64.h"

//temporary
char i386_shellcode[] = {
  '\xE9','\x00','\x00','\x00','\x00'
};

static void copy_bytes(char *old, char *new, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    *(new+i) = *(old+i);
  }
}

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

/*
static void load_shellcode32(char *shellcode, void *to)
{
  copy_bytes(i386_shellcode, shellcode, JMP32_SHORT_SIZE);
  memcpy(shellcode+1, (const void*)&to, sizeof(to));
}
*/

void throw_hook_i386(void *orig, void *repl, void **orig_ptr)
{
  int r_offset = repl - orig - JMP32_SHORT_SIZE;
  int stolen_bytes = 0;
  char *func_ptr = (char*)orig;
  char *original_prologue = malloc(32);
  memset(original_prologue, 0x90, 32);

  vm_protect(mach_task_self(), (vm_address_t)orig, 32, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);
  vm_protect(mach_task_self(), (vm_address_t)original_prologue, 32, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

  stolen_bytes = eat_instructions(orig);

  memcpy(i386_shellcode+1, (const void*)&r_offset, sizeof(r_offset));

  memcpy(original_prologue, orig, stolen_bytes);
  memcpy(orig, i386_shellcode, stolen_bytes); // call

  char *trampoline_address = (char*)((uintptr_t)orig + (uintptr_t)stolen_bytes);

  r_offset = trampoline_address - (original_prologue+stolen_bytes) - JMP32_SHORT_SIZE;
  memcpy(i386_shellcode+1, (const void*)&r_offset, sizeof(r_offset));

  memcpy(original_prologue+stolen_bytes, i386_shellcode, sizeof(i386_shellcode));
  
  *orig_ptr = original_prologue;
}

#endif

#if defined (__x86_64__)

static void load_shellcode64(char *shellcode, void *to)
{
  copy_bytes(SH_JMP64_LONG, shellcode, JMP64_LONG_SIZE);
  memcpy(shellcode+2, (const void*)&to, sizeof(to));
}

static int make_zone_executable(void *z_ptr, size_t sz)
{
  int ret = 0;
  if ((ret = vm_protect(mach_task_self(), (vm_address_t)z_ptr, sz, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE)) == 0)
    return ret;

    return -1;
}

static int allocate_jump_zone(void **z_ptr, void *orig)
{
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
    *z_ptr = (void*) page;
  else if(!allocated && !ret)
    ret = KERN_NO_SPACE;

  return ret;
}

static int deallocate_jump_zone(void *z_ptr)
{
  assert(z_ptr);

  int ret = 0;
  if((ret = vm_deallocate(mach_task_self(), (vm_address_t)z_ptr, PAGE_SIZE)) == 0)
    return 0;

  return -1;
}

static int populate_jump_zone(void *z_ptr, char *jmp_shellcode, char *cooked, size_t ck_sz)
{
  assert(z_ptr);
  assert(jmp_shellcode);

  copy_bytes(JUMP_ZONE, z_ptr, JUMP_ZONE_SIZE); // copy memory -> memory

  if (cooked && (ck_sz != 0)) {
    memcpy(z_ptr, (const void *)cooked, ck_sz);
  } else if(cooked && (ck_sz == 0)) {
    return -1;
  }

  memcpy(z_ptr+(JUMP_ZONE_SIZE-JMP64_LONG_SIZE), (const void*)jmp_shellcode, JMP64_LONG_SIZE);
  make_zone_executable(z_ptr, PAGE_SIZE);

  return 0;
}

void mov(uint64_t *target, uint64_t value) //we need casting
{
    *target = value;
}

static void set_jump_to_jump_zone(void *z_ptr, void *target)
{
  uint64_t s_jump = 0x0;
  uint32_t off = ((char*)z_ptr - (char*)target - JMP32_SHORT_SIZE);
  off = OSSwapInt32(off); // reverse byte order

  s_jump |= 0xE900000000000000LL;
  s_jump |= ((uint64_t)off & 0xFFFFFFFF) << 24;
  s_jump = OSSwapInt64(s_jump);

  make_zone_executable(target, JMP32_SHORT_SIZE);
  mov((uint64_t *)target, s_jump);
}

void throw_hook(void *orig, void *repl, void **orig_ptr)
{
  //__DBG("throw_hook: (%p)\n", orig);

  char zone_jump[JMP64_LONG_SIZE];
  char return_jmp[JMP64_LONG_SIZE];
  void *jzone_ptr = NULL, *trampoline_ptr = NULL;

  //allocate and fill jump zone (near original function) with long jump to replacement
  allocate_jump_zone(&jzone_ptr, orig);
  load_shellcode64(zone_jump, repl);
  populate_jump_zone(jzone_ptr, zone_jump, NULL, 0);

  size_t stolen_bytes = eat_instructions(orig);
  char prologue[stolen_bytes];

  memcpy(prologue, (const void *)orig, stolen_bytes);

  allocate_jump_zone(&trampoline_ptr, jzone_ptr);
  load_shellcode64(return_jmp, orig+stolen_bytes);
  populate_jump_zone(trampoline_ptr, return_jmp, prologue, stolen_bytes);

  make_zone_executable(orig, stolen_bytes);
  make_zone_executable(trampoline_ptr, PAGE_SIZE);
  //memset(orig, NOP_INSN, stolen_bytes);
  set_jump_to_jump_zone(jzone_ptr, orig);

  if(orig_ptr != NULL)
    *orig_ptr = trampoline_ptr;
}

#endif
