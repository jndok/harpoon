# harpoon (x86)
Very simple runtime hooking library for OS X. *for x64, check out the `harpoon64` branch.*

## Update notes (7/25/15)
A big update is coming soon. Harpoon will use another hooking method, bugfixes, plus huge code cleanup are planned. The two version will remain separate for now, but a merge will come, eventually.

## Warning
This library is still in early alpha, and I am still creating basically everything. You cannot even say it's a skeleton lib for now, as it contains only 4 functions. It is usable and it works (decently). Please be patient as I fix some bugs and commit the rest of the code. Thank you!

## How to implement
The library works only on i386 (aka Mach32) for now, so it's almost useless. There are two hooking functions:
* `__throw_hook_with_jmp(void *, void *, void**)` : Throws an hook to the function defined in the first param, replacing the implementation with the function defined in the 2nd. The third is the classical `branchIsland` from `mach_override`. Basically it's used to call the original function from the replacement or anywhere. This function uses an x86 `JMP` instruction to perform the jump.
* `__throw_hook_with_push_ret(void *, void *, void**)` : Throws an hook to the function defined in the first param, replacing the implementation with the function defined in the 2nd. The third is the classical `branchIsland` from `mach_override`. Basically it's used to call the original function from the replacement or anywhere. This function uses the x86 `PUSH` and `RET` instructions to perform the jump. **Note that this is half-broken for now. May not work sometimes.**

## How to inject
You can use every runtime injecting code to shoot the `dylib`. However, note that `harpoon` doesn't have an injecting routine, so it must be external. If you don't care about runtime injection, go for a classical `DYLD_INSERT_LIBRARIES` and launch the program. Remember to force the flat namespace! Example:
  
      DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=libTest.dylib ./test

## Known issues
These are the known issues with the library.

* **Cannot hook `return` only functions.** –– The library fails to hook functions that only contain a simple `return` statement, falling in an infinite loop. The hook is thrown but doesn't reach the function. Ex.:

  `int double_number(int n) { return n*2; } // this cannot be hooked`

## Updates
I will add more hooking methods and fix existing. x64 support should also be a thing, if I understand how to do it correctly. Also, debugging functions may come in the future.
