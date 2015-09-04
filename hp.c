#include "hp.h"

typedef struct __attribute__((packed)) jmp64 {
  uint16_t mov;
  uint64_t addr;
  uint16_t jmp;
} jlong;

typedef struct __attribute__((packed)) jmp32 {
  uint8_t jmp;
  uint32_t off;
} jshort;

size_t eat(void *target, size_t bytes_to_eat)
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

  cnt = cs_disasm(handle, target, 0xF, 0x0, 0, &insn);
  for (size_t k = 0;; k++) {
    if (!(len_cnt >= bytes_to_eat)) {
      len_cnt+=insn[k].size;
    } else {
      break;
    }
  }
  return len_cnt;
}

void hhook(void *target, void *repl, void **reentry)
{
#ifdef __x86_64__
#ifdef __DBG_ENABLED
  OUTPUT_MSG("[+] Detected x64 binary.");
#endif
  size_t eaten = eat(target, sizeof(jlong));
  if (!eaten) {
    OUTPUT_MSG("[!] Cannot eat function.");
    exit(-1);
  }
  void *trampoline=(void*)malloc(eaten+sizeof(jlong));
  vm_protect(mach_task_self(), (vm_address_t)target, eaten, 0, VM_PROT_ALL);
  vm_protect(mach_task_self(), (vm_address_t)trampoline, eaten+sizeof(jlong), 0, VM_PROT_ALL);
  jlong jl;
  jl.mov = 0xb848;
  jl.addr = (uint64_t)repl;
  jl.jmp = 0xe0ff;
  memcpy(trampoline, (const void*)target, eaten);
  *(jlong*)target = jl;
  jl.addr = (uint64_t)target+eaten;
  memcpy(trampoline+eaten, (const void*)&jl, sizeof(jlong));
  if (reentry)
    *reentry=trampoline;
  vm_protect(mach_task_self(), (vm_address_t)trampoline, eaten+sizeof(jlong), 0, VM_PROT_READ|VM_PROT_EXECUTE);
#ifdef __DBG_ENABLED
  OUTPUT_MSG("hook @ %p --> %p | eaten: %lu | trampoline @ %p", target, repl, eaten, trampoline);
#endif
#endif
#ifdef __i386__
  OUTPUT_MSG("[+] Detected i386 binary.");
  size_t eaten = eat(target, sizeof(jshort));
  if (!eaten) {
    OUTPUT_MSG("[!] Cannot eat function.");
    exit(-1);
  }
  void *trampoline=(void*)malloc(eaten+sizeof(jshort));
  vm_protect(mach_task_self(), (vm_address_t)target, eaten, 0, VM_PROT_ALL);
  vm_protect(mach_task_self(), (vm_address_t)trampoline, eaten+sizeof(jshort), 0, VM_PROT_ALL);
  jshort js;
  js.jmp = 0xe9;
  js.off = (uint32_t)repl - (uint32_t)target - 0x5;
  memcpy(trampoline, (const void*)target, eaten);
  *(jshort*)target = js;
  js.off = (uint32_t)(target+eaten) - (uint32_t)(trampoline+eaten) - 0x5;
  memcpy(trampoline+eaten, (const void*)&js, sizeof(jshort));
  if (reentry)
    *reentry=trampoline;
  vm_protect(mach_task_self(), (vm_address_t)trampoline, eaten+sizeof(jshort), 0, VM_PROT_READ|VM_PROT_EXECUTE);
#ifdef __DBG_ENABLED
  OUTPUT_MSG("hook @ %p --> %p | eaten: %lu | trampoline @ %p", target, repl, eaten, trampoline);
#endif
#endif
}
