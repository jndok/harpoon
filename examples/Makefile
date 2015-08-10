all:
	make x86_64; make i386; make inject_x86_64; make inject_i386;

x86_64:
	gcc test.c ../core/harpoon.c ../ext/libcapstone.a -o test #-Wl,-no_pie

i386:
	gcc test32.c -m32 ../core/harpoon.c ../ext/libcapstone.a -o test32 #-Wl,-no_pie

inject_x86_64:
	gcc lib.c ../core/harpoon.c ../ext/libcapstone.a -dynamiclib -o libinject.dylib

inject_i386:
	 gcc lib32.c -m32 ../core/harpoon.c ../ext/libcapstone.a -dynamiclib -o libinject32.dylib
