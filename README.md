# harpoon (reborn)
Lightweight runtime hooking library for OS X.

## Current status
This version has been reorganized, code is cleaner, hooking time is halved (~0.050s on x64 and ~0.005 on x86) and everything works better.
<br>
Other updates are planned in the near future.

### Warning
*harpoon* is a just for fun project, meaning I'm not putting all my effort into it (for now, at least :P). This being said,
beware. Runtime function hooking is sneaky stuff, and my *harpoon* itself is certainly not perfect.
The new version aims to be usable in a real world application, and in serious projects, but it still has to be improved **a lot**.
You can help by submitting issues or code fixes, and I'd be absolutely happy if you do!

## Usage
*harpoon* is very easy to use. It currently has only two exposed functions, `throw_hook` and `throw_hook_i386`, both take two parameters and one optional parameter.

### Throwing an hook
Throwing an hook is extremely easy. `#include` the `core/` and the `ext` dirs in your code, and just call one of the functions below (depends if you are hooking on x64 or x86):
+ `void throw_hook[_i386](void *orig, void *repl, void **orig_ptr)`: `*orig` is the original function. `*repl` is the replacement function. `**orig_ptr` is an optional function pointer to call the original function, without de-hooking it.
<br>
*Note that the `[_i386]` in the function name just means that you have to use `throw_hook_i386` if on x86, and `throw_hook` if on x64.*

### Inject
The real usage of *harpoon* comes in when combined with `dylib` injecting. Basically you inject a compiled `dylib` into an executable, and hook into the executable functions.

Simply use the `throw_hook` function, by specifying an executable function as the `*orig` paramter. The `*repl` should be a function in your `dylib`, so should be the `**orig_ptr` parameter. **IMPORTANT: Remember to call the function pointer at the end of your replacement function! Failing to do so will result in the original function never going to be called.**

Once your `dylib` is ready, compile it with something like:

    gcc code.c -dynamiclib -o libinject.dylib

You're now ready to inject it into a program! Look into the `lib.c`/`lib32.c` file to see an injectable `dylib` example.

*harpoon* does not provide an injecting routine, so you must use an external one. An example would be the common `dyld` environment variables to preload the `dylib`:

    DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=libTest.dylib ./prog

## Side notes
#### Hooking time
The hooking time is much longer on x64 (~0.050s) because of the x64 hooking method. We have to choose an appropriate location in the address space where to map the jump zone, or the relative jump from the original function will fail.
<br>
Jump zones are required to hook certain functions without problems. I will add an option to avoid jump zones if your function doesn't need them and speed up the hooking time.

## Future updates
I'll just add i386 support and fix the code for now, when this is done I may be adding some cool features and other hooking methods.
