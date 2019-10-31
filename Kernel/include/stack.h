#ifndef _STACK_H_
#define _STACK_H_

#include <stdint.h>

typedef struct stack_frame {
    uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
    uint64_t ip;
    uint64_t cs;
    uint64_t eflags;
    uint64_t sp;
    uint64_t ss;
    uint64_t returnAddress;
} stackFrame;

/* Initializes stack */
stackFrame init_stack(void * entryPoint, void * stackBase);

#endif /* _STACK_H_ */


