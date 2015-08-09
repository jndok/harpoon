all:	
	gcc test.c core/harpoon64.c core/sc/sh64.s ext/libcapstone.a -o test #-Wl,-no_pie

i386:
	gcc test32.c -m32 core/harpoon64.c ext/libcapstone.a -o test32 #-Wl,-no_pie

inject:
	gcc lib.c core/harpoon64.c core/sc/sh64.s ext/libcapstone.a -dynamiclib -o libinject.dylib

inject_i386:
	gcc lib32.c -m32 core/harpoon64.c ext/libcapstone.a -dynamiclib -o libinject32.dylib

clean:
	rm -f test
	rm -f test32
	rm -f *.dylib
