// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>

#include <stack.h>

/* Initializes stack */
stackFrame init_stack(void * entryPoint, void * stackBase) {
    stackFrame ret;
    ret.r15 = 0x0;
	ret.r14 = 0x0;
	ret.r13 = 0x0;
	ret.r12 = 0x0;
	ret.r11 = 0x0;
	ret.r10 = 0x0;
	ret.r9 = 0x0;
	ret.r8 = 0x0;
	ret.rsi = 0x0;
	ret.rdi = 0x0;
	ret.rbp = 0x0;
	ret.rdx = 0x0;
	ret.rcx = 0x0;
	ret.rbx = 0x0;
	ret.rax = 0x0;
    ret.ip = (uint64_t) entryPoint;
    ret.cs = 0x8;
    ret.eflags = 0x202;
    ret.sp = (uint64_t) stackBase;
    ret.ss = 0x0;
    ret.returnAddress = 0x0 /*(uint64_t)kill_current*/; // TODO when scheduler

    return ret;
}