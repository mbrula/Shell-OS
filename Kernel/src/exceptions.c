// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <exceptions.h>
#include <console.h>
#include <interrupts.h>
#include <timelib.h>

static void get_registers();
static void zero_division();
static void invalid_opcode();
static void not_imp();

RegisterSet regState;
ExceptionStackFrame excState;

extern void kill_current();

void (* excHandlers[]) () = {zero_division, not_imp, not_imp, not_imp, not_imp, not_imp, invalid_opcode};

/* Exception Dispatcher function */
void exceptionDispatcher(int exc) {
	_cli();
	
	/* Some exceptions load on stack Error Code, if they do, get from stack the IP */

	/* These exceptions don't have Error Code */
	if (exc == 8 || (exc >= 10 && exc <= 14) || exc == 17)
        __asm__ volatile ("mov 16(%%rsp), %0;" : "=a" (excState.rip));
	else
	    __asm__ volatile ("mov 24(%%rsp), %0;" : "=a" (excState.rip));

	get_registers();
	
	clear_console();
	print_char('\n');

	/* Excecute exception atention routine */
	excHandlers[exc]();
	
	/* Prints state of all registres */
	printError("RIP: %d\n", excState.rip);
    printError("RAX: %d\nRBX: %d\nRCX: %d\nRDX: %d\nRDI: %d\nRSI: %d\nRBP: %d\nRSP: %d\nR8: %d\nR9: %d\n", 
		regState.rax, regState.rbx, regState.rcx, regState.rdx, regState.rdi, regState.rsi, regState.rbp, regState.rsp, regState.r8, regState.r9);
	print("\n\nKilling current Process");

    _sti();
    sleep(2000);
	kill_current();
}

/* Gets all registers */
static void get_registers(){
	__asm__ volatile ( "mov %%rax, %0;" : "=a" (regState.rax));
    __asm__ volatile ( "mov %%rbx, %0;" : "=a" (regState.rbx));
    __asm__ volatile ( "mov %%rcx, %0;" : "=a" (regState.rcx));
    __asm__ volatile ( "mov %%rdx, %0;" : "=a" (regState.rdx));
    __asm__ volatile ( "mov %%rdi, %0;" : "=a" (regState.rdi));
    __asm__ volatile ( "mov %%rsi, %0;" : "=a" (regState.rsi));
    __asm__ volatile ( "mov %%rbp, %0;" : "=a" (regState.rbp));
    __asm__ volatile ( "mov %%rsp, %0;" : "=a" (regState.rsp));
    __asm__ volatile ( "mov %%r8, %0;"  : "=a" (regState.r8));
    __asm__ volatile ( "mov %%r9, %0;"  : "=a" (regState.r9));
}

/* Zero division exception */
static void zero_division() {
    printError("Exception 0: Division By Zero\n");
}

/* Invalid operation code exception */
static void invalid_opcode() {
    printError("Exception 6: Invalid Opcode\n");
}

/* Generic exception */
static void not_imp() {
    printError("Exception not implemented\n");
}