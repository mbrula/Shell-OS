#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stdint.h>

/* Exception Dispatcher function */
void exceptionDispatcher(int exc);

/* Struct on stack after exception is launched (rip is the only one used) */
typedef struct {
    uint64_t stack_segment;
    uint64_t stack_ptr;
    uint64_t rflags;
    uint64_t cs;
    uint64_t rip;
    uint64_t errCode;
} ExceptionStackFrame;

/* Struct to complete al values from all registres */
typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
} RegisterSet;

#endif /* _EXCEPTIONS_H_ */