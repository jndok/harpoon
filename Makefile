all:	
	cd examples; make all;

x86_64:
	cd examples; make x86_64;

i386:
	cd examples; make i386;

inject_x86_64:
	cd examples; make inject_x86_64;

inject_i386:
	cd examples; make inject_i386;

clean:
	cd examples; rm -f test; rm -f test32; rm -f *.dylib
