all:	
	gcc test.c core/harpoon64.c core/sc/sh64.s core/sc/sh32.s -lcapstone -o test #-Wl,-no_pie

inject:
	gcc lib.c core/harpoon64.c core/sc/sh64.s core/sc/sh32.s -lcapstone -dynamiclib -o libinject.dylib

clean:
	rm -f test
	rm -f *.dylib
