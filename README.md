# harpoon 
#### *A very simple i386 / x86_64 hooking library for Mac OS X*

- very easy to use
- uses capstone engine for on-the-fly analysis
- written by [@jndok](http://twitter.com/jndok), refactored by @kpwn ([@qwertyoruiop](http://twitter.com/qwertyoruiop))

Usage
-

- Add the header: 
`#include "libhook.h"`

- Add a definition for the function pointer to the original function: 
`static void (*hooked_fcn_orig)(argument_types);`

- Hook the function by running (typically in a dynamic library constructor): 
`throw_hook((void*)function_to_hook, (void*)my_replacement, (void**) &hooked_fcn_orig);`

Tips for Library Injection
-

Compile your library so it either links dynamically or statically to harpoon's libhook, then load it into the task with:

- `DYLD_INSERT_LIBRARIES`to inject your library at runtime on any non-`RESTRICT` process
- [inj](http://github.com/kpwn/inj) by @kpwn  to load your library on-the-fly using `task_for_pid`
- [yololib](http://github.com/kpwn/inj) by Terence Tan to add your library as a run-time dependency to any Mach-O binary
- You can modify an existing library in your file system (dangerous, but useful in some rare cases) to load your library when loaded.

Issues
-

- It hasn't been tested throughly, although it's currently in use on various private (public too?) projects successfully.
- The trampoline jump code requires 12 bytes on x86_64. Some functions may be too small to be hooked.
- If you notice anything else, please shoot an Issue
