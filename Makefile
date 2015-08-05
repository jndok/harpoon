all:	
	gcc test.c core/harpoon64.c core/sc/sh64.s ext/dbg/hdbg.c  -lcapstone -o test #-Wl,-no_pie
