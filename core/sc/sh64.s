.text
.globl _JUMP_ZONE
.globl _SH_JMP64_LONG

_SH_JMP64_LONG:
	movq $0xdeadbeef, %rax
	jmp *%rax

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
