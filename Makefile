all:	
	gcc test.c core/harpoon64.c core/sc/sh64.s -lcapstone -o test #-Wl,-no_pie

inject:
	gcc lib.c core/harpoon64.c core/sc/sh64.s -lcapstone -dynamiclib -o libinject.dylib

clean:
	rm -f test
	rm -f *.dylib
