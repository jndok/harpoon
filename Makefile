all: test.c
	gcc testlib.c hrp_hooking.c -dynamiclib -o libTest.dylib -arch i386 -Wno-deprecated-declarations -DDEBUG
	gcc -m32 test.c -o test