all:	
	gcc test.c core/harpoon64.c core/sc/sh64.s  -lcapstone -o test #-Wl,-no_pie
