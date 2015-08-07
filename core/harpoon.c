#include "harpoon.h"
#include <pthread.h>

#define ZONE_SIZE 0x20

static uint64_t* zone_free_list = NULL;
static pthread_mutex_t zone_lck = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t hook_lck = PTHREAD_MUTEX_INITIALIZER;
static void zfree(void* alloc);
static void* zalloc() {
    void* ret = NULL;
    pthread_mutex_lock(&zone_lck);
    if (!zone_free_list) {
        uint64_t* szfl = 0;
        
        vm_allocate(mach_task_self_, (vm_address_t*)&szfl, PAGE_SIZE, 1);
        if (!szfl) {
            goto out;
        }
        vm_protect(mach_task_self_, (vm_address_t)szfl, PAGE_SIZE, 0, VM_PROT_ALL);
        for (int i = 0; i < (PAGE_SIZE/ZONE_SIZE); i++) {
            zfree((void*)(1ULL | (uint64_t)&szfl[i*(ZONE_SIZE/sizeof(uint64_t))]));
        }
    }
    if (!zone_free_list) {
        goto out;
    }
    ret = zone_free_list;
    zone_free_list = (uint64_t*) zone_free_list[0];
    ((uint64_t*) ret)[0] = 0xbbadbeefbbadbeef;
out:
    pthread_mutex_unlock(&zone_lck);
    return ret;
}
static void zfree(void* alloc) {
    char lock = !(((uint64_t)alloc) & 1);
    
    alloc = (void*) (((uint64_t) alloc) & (~1));
    
    if (lock) {
        pthread_mutex_lock(&zone_lck);
    }
    bzero(alloc, ZONE_SIZE);
    ((uint64_t*) alloc)[0] = (uint64_t)zone_free_list;
    zone_free_list = (uint64_t*)alloc;
    if (lock) {
        pthread_mutex_unlock(&zone_lck);
    }
}

size_t eat_instructions(void *func, size_t target)
{
  csh handle;
  cs_insn *insn;
  size_t cnt;
  size_t len_cnt = 0;

  if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
    return 0;

  cnt = cs_disasm(handle, func, 0xF, 0x0, 0, &insn);
  for (int k = 0; k < 0xF || !(len_cnt >= target); k++) {
    if (len_cnt < target) {
      len_cnt+=insn[k].size;
    }
  }
    
    if (len_cnt < target) {
        return 0;
    }

  return len_cnt;
}

typedef struct __attribute__((__packed__)) opst {
    uint16_t a; uint64_t b; uint16_t c;
} opst;

void throw_hook(void *orig, void *repl, void **orig_ptr)
{
  //__DBG("throw_hook: (%p)\n", orig);

    pthread_mutex_lock(&hook_lck);

    void *tramp = zalloc();
    
    // orig_ptr
    size_t eaten = eat_instructions(orig, 12);
    if (!eaten) {
        goto out;
    }
    opst x;
    x.a = 0xb848;
    x.b = (uint64_t)repl;
    x.c = 0xc350;
    
    vm_protect(mach_task_self_, (vm_address_t)orig, PAGE_SIZE, 0, VM_PROT_ALL);
    vm_protect(mach_task_self_, (vm_address_t)orig+12, PAGE_SIZE, 0, VM_PROT_ALL);
    
    memcpy(tramp, orig, eaten);
    memset(orig, 0x90, eaten);
    memcpy(orig, &x, 12);
    
    x.b = (uint64_t) orig + eaten;
    
    memcpy(tramp+eaten, &x, 12);
    
    vm_protect(mach_task_self_, (vm_address_t)orig, PAGE_SIZE, 0, VM_PROT_READ|VM_PROT_EXECUTE);
    vm_protect(mach_task_self_, (vm_address_t)orig+12, PAGE_SIZE, 0, VM_PROT_READ|VM_PROT_EXECUTE);

    if (orig_ptr) {
        *orig_ptr = tramp;
    }
out:
    pthread_mutex_unlock(&hook_lck);
}
