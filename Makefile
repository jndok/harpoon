all:	
	gcc core/harpoon.c ext/libcapstone.a -o libhook.dylib -dynamiclib -arch i386 -arch x86_64 -Iext/include
	gcc core/harpoon.c -o libhook.32.o -c -arch i386 -Iext/include
	gcc core/harpoon.c -o libhook.64.o -c -arch x86_64 -Iext/include
	libtool -static -o libhook.a libhook.32.o libhook.64.o ext/libcapstone.a
	cd tests; make
	rm libhook.32.o libhook.64.o
	@echo;echo;echo Compiled dymanic library: libhook.dylib;echo Compiled static library: libhook.a;echo Compiled 64-bit test: ./tests/test; echo Compiled 32-bit test: ./tests/test32
clean:
	rm -f tests/test tests/test32
	rm -f *.dylib *.o *.a
