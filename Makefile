all:	
	gcc core/harpoon.c ext/libcapstone.a -o libhook.dylib -dynamiclib
	cd tests; make
clean:
	rm -f tests/test
	rm -f *.dylib
