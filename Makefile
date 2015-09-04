all:
	make x64; make x86;

x64:
	gcc -m64 test.c hp.c lib/libcapstone.a -o test #-Wl,-no_pie

x86:
	gcc -m32 test.c hp.c lib/libcapstone.a -o test32 #-Wl,-no_pie

clean:
	rm -rf test test32
