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

## Usage
*harpoon* is very easy to use. It currently has only one exposed function, `throw_hook`, which takes two parameters and one optional parameter.
+ `void throw_hook(void *orig, void *repl, void **orig_ptr);`: The first paramter, `orig`, is the original function aka the function to be hooked. The second one, `repl`, is the replacement function aka the function that will be called instead of the original. The third (and optional) parameter, `orig_ptr`, is a function pointer that can be used to call the original function without removing the hook.
A simple usage of this function can be seen in the `test.c` file. It's easy as that!

### Inject
*harpoon* does not provide an injecting routine, so you must use an external one. An example would be the common `dyld` environment variables to preload the `dylib`:
  
    DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=libTest.dylib ./prog

## Future updates
I'll just add i386 support and fix the code for now, when this is done I may be adding some cool features and other hooking methods.
