all:	
	gcc core/harpoon.c ext/libcapstone.a -o libhook.dylib -dynamiclib -arch i386 -arch x86_64 -Iext/include
	cd tests; make
clean:
	rm -f tests/test
	rm -f *.dylib
