all:	
	gcc core/harpoon64.c core/sc/sh64.s core/sc/sh32.s ext/libcapstone.a -o libhook.dylib -dynamiclib

clean:
	rm -f test
	rm -f *.dylib
