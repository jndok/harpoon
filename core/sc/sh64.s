.text
.globl _JUMP_ZONE
.globl _SH_JMP64_LONG
.globl _SH_JMP32_SHORT

_SH_JMP64_LONG:
	movq $0xdeadbeef, %rax
	jmp *%rax

_SH_JMP32_SHORT:
	jmp 0xfacefeed

_JUMP_ZONE:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
