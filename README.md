# harpoon (reborn)
Lightweight runtime hooking library for OS X.

## Current status
*harpoon* was totally rewritten recently, the code has been changed completely, and you should use the new version.
I will keep updating and improving it, so remember to check back here for any update/bug fix.
*harpoon* currently supports x64 only, as I am rewriting the x86 part as well. Some of the i386 hooking code is already present in the current version, but it's not implemented yet.
x86 support will come very soon.

### Warning
*harpoon* is a just for fun project, meaning I'm not putting all my effort into it (for now, at least :P). This being said,
beware. Runtime function hooking is sneaky stuff, and my *harpoon* itself is certainly not perfect.
The new version aims to be usable in a real world application, and in serious projects, but it still has to be improved **a lot**.
You can help by submitting issues or code fixes, and I'd be absolutely happy if you do!

## Installation
*harpoon* has not an install script for now. Simply copy the `core/` and the `ext/` folders in your project directory and access them locally:

    #include "core/harpoon64.h"

I will add an installation system in the future. 

## Usage
*harpoon* is very easy to use. It currently has only one exposed function, `throw_hook`, which takes two parameters and one optional parameter.

### Throwing an hook
When I say *throw an hook*, I mean dynamically replace a generic `a()` function implementation with a generic `b()` function implementation. As you can see below harpoon uses the `throw_hook` function, to do so.

+ `void throw_hook(void *orig, void *repl, void **orig_ptr)`: Replace function `*orig` implementation with `*repl` implementation. Optionally return a pointer to the original function in `**orig_ptr`.

The first parameter, `*orig`, is basically the original function, aka the one that will be hooked. The second parameter, `*repl`, is the replacement function, aka the one that will be called instead of `*orig`.
The third parameter is entirely optional, if not desired pass `NULL`. It is a function pointer, and will hold the original function prologue (see technical explanation below), which can be used to call the `*orig` function.

Look in the `test.c` file to see an usage of this function.

### Inject
The real usage of *harpoon* comes in when combined with `dylib` injecting. Basically you inject a compiled `dylib` into an executable, and hook into the executable functions.

Simply use the `throw_hook` function, by specifying an executable function as the `*orig` paramter. The `*repl` should be a function in your `dylib`, so should be the `**orig_ptr` parameter. **IMPORTANT: Remember to call the function pointer at the end of your replacement function! Failing to do so will result in the original function never going to be called.**

Once your `dylib` is ready, compile it with something like:

    gcc code.c -dynamiclib -o libinject.dylib

You're now ready to inject it into a program! Look into the `lib.c` file to see an injectable `dylib` example.

*harpoon* does not provide an injecting routine, so you must use an external one. An example would be the common `dyld` environment variables to preload the `dylib`:

    DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=libTest.dylib ./prog

## Future updates
I'll just add i386 support and fix the code for now, when this is done I may be adding some cool features and other hooking methods.
