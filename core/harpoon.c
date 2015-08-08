#include "harpoon.h"
#include <pthread.h>


#ifdef __x86_64__
typedef struct __attribute__((__packed__)) opst {
    uint16_t a; uint64_t b; uint16_t c;
} opst;
#elif __i386__
typedef struct __attribute__((__packed__)) opst {
    uint8_t a; uint32_t b;
} opst;
#endif

#ifdef __x86_64__
#define ABSJUMP_SUB(x) 0
#define ZONE_SIZE ((1 + ((sizeof(opst)) >> 8)) << 8)
#define native_word_t uint64_t
#define ZONE_ALLOCATOR_BEEF 0xbbadbeefbbadbeef
#elif __i386__
#define ABSJUMP_SUB(x) (5+(uint32_t)(x))
#define ZONE_SIZE ((1 + ((sizeof(opst)) >> 8)) << 8)
#define native_word_t uint32_t
#define ZONE_ALLOCATOR_BEEF 0xbbadbeef
#endif

static native_word_t* zone_free_list = NULL;
static pthread_mutex_t zone_lck = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t hook_lck = PTHREAD_MUTEX_INITIALIZER;
static void zfree(void* alloc);
static void* zalloc() {
    void* ret = NULL;
    pthread_mutex_lock(&zone_lck);
    if (!zone_free_list) {
        
        if (ZONE_SIZE % 2 || ZONE_SIZE < sizeof(native_word_t)) {
            puts("zalloc error: zone size must be a multiple of 2 and bigger than sizeof(native_word_t)");
            exit(-1);
        }

        native_word_t* szfl = 0;
        
        vm_allocate(mach_task_self_, (vm_address_t*)&szfl, PAGE_SIZE, 1);
        if (!szfl) {
            goto out;
        }
        vm_protect(mach_task_self_, (vm_address_t)szfl, PAGE_SIZE, 0, VM_PROT_ALL);
        for (int i = 0; i < (PAGE_SIZE/ZONE_SIZE); i++) {
            zfree((void*)(1ULL | (native_word_t)&szfl[i*(ZONE_SIZE/sizeof(native_word_t))]));
        }
    }
    if (!zone_free_list) {
        goto out;
    }
    ret = zone_free_list;
    zone_free_list = (native_word_t*) zone_free_list[0];
    ((native_word_t*) ret)[0] = ZONE_ALLOCATOR_BEEF;
out:
    pthread_mutex_unlock(&zone_lck);
    return ret;
}
static void zfree(void* alloc) {
    char lock = !(((native_word_t)alloc) & 1);
    
    alloc = (void*) (((native_word_t) alloc) & (~1));
    
    if (lock) {
        pthread_mutex_lock(&zone_lck);
    }
    bzero(alloc, ZONE_SIZE);
    ((native_word_t*) alloc)[0] = (native_word_t)zone_free_list;
    zone_free_list = (native_word_t*)alloc;
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
#ifdef __x86_64__
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return 0;
#elif __i386__
    if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK)
        return 0;
#endif
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

void throw_hook(void *orig, void *repl, void **orig_ptr)
{
  //__DBG("throw_hook: (%p)\n", orig);

    pthread_mutex_lock(&hook_lck);

    void *tramp = zalloc();

    // orig_ptr
    size_t eaten = eat_instructions(orig, sizeof(opst));
    if (!eaten) {
        puts("eaten = 0");
        goto out;
    }
    opst x;
    
#ifdef __x86_64__
    x.a = 0xb848; // mov rax, target
    x.c = 0xc350; // pop rax; ret
#elif __i386__
    x.a = 0xE9;   // abs jump
#endif
    
    x.b = (native_word_t) repl - ABSJUMP_SUB(orig);

    vm_protect(mach_task_self_, (vm_address_t)orig, PAGE_SIZE, 0, VM_PROT_ALL);
    vm_protect(mach_task_self_, (vm_address_t)orig+sizeof(opst), PAGE_SIZE, 0, VM_PROT_ALL);
    
    memcpy(tramp, orig, eaten);
    memset(orig, 0x90, eaten);
    memcpy(orig, &x, sizeof(opst));

    x.b = (native_word_t) (orig + eaten) - ABSJUMP_SUB(tramp+eaten);
    
    memcpy(tramp+eaten, &x, sizeof(opst));
    
    vm_protect(mach_task_self_, (vm_address_t)orig, PAGE_SIZE, 0, VM_PROT_READ|VM_PROT_EXECUTE);
    vm_protect(mach_task_self_, (vm_address_t)orig+sizeof(opst), PAGE_SIZE, 0, VM_PROT_READ|VM_PROT_EXECUTE);

    if (orig_ptr) {
        *orig_ptr = tramp;
    }
out:
    pthread_mutex_unlock(&hook_lck);
}
