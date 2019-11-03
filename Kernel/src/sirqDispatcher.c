// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <stdint.h>
#include <mutex.h>

#define SYSCALL_COUNT	29

/* Software handlers functions */
static uint64_t syscall_00 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_01 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_02 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_03 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_04 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_05 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_06 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_07 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_08 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static uint64_t syscall_09 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_10 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_11 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_12 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static uint64_t syscall_13 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_14 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_15 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_16 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_17 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_18 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_19 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

static uint64_t syscall_20 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_21 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_22 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_23 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_24 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_25 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_26 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_27 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_28 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static uint64_t syscall_29 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

uint64_t (* syscalls[]) (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) = 
																{syscall_00, syscall_01, syscall_02, syscall_03, 
																	syscall_04, syscall_05, syscall_06, syscall_07,
																	syscall_08,	syscall_09, syscall_10, syscall_11,
																	syscall_12, syscall_13, syscall_14, syscall_15,
																	syscall_16, syscall_17, syscall_18, syscall_19,
																	syscall_20, syscall_21, syscall_22, syscall_23,
																	syscall_24, syscall_25, syscall_26, syscall_27,
																	syscall_28, syscall_29};

/* Dispatcher for software interrupts */
uint64_t handleSyscall(uint64_t sirq, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	if (sirq <= SYSCALL_COUNT) {
		return syscalls[sirq](rdi, rsi, rdx, rcx, r8, r9); 
	}
	return 1;
}

uint64_t syscall_00 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	read_handler(rdi, (char *)rsi, rdx);
	return 0;
}

uint64_t syscall_01 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	write_handler(rdi, (char *)rsi, rdx);
	return 0;
}

uint64_t syscall_02 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return time_handler();
}

uint64_t syscall_03 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	clear_handler();
	return 0;
}

uint64_t syscall_04 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return rtc_handler((char) rdi);
}

uint64_t syscall_05 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	sleep_handler(rdi);
	return 0;
}

uint64_t syscall_06 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	beep_handler(rdi, rsi);
	return 0;
}

uint64_t syscall_07 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	exit_handler();
	return 0;
}

uint64_t syscall_08 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	pixel_handler(rdi, rsi, rdx);
	return 0;
}

/* -----------------------------------------------------------------*/
uint64_t syscall_09 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return (uint64_t) malloc_handler(rdi);
}

uint64_t syscall_10 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	free_handler((void *) rdi);
	return 0;
}

uint64_t syscall_11 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	mm_print_status_handler();
	return 0;
}

uint64_t syscall_12 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	mm_get_status_handler((uint64_t *)rdi, (uint64_t *)rsi, (uint64_t *)rdx);
	return 0;
}

/* -----------------------------------------------------------------*/

uint64_t syscall_13 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return create_handler((char *)rdi, rsi, (char **) rdx, rcx, (char *) r8);
}

uint64_t syscall_14(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return kill_handler(rdi);
}

uint64_t syscall_15 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return get_pid_handler();
}

uint64_t syscall_16 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	list_all_process_handler();
	return 0;
}

uint64_t syscall_17 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return set_priority_handler(rdi, rsi);
}

uint64_t syscall_18 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return change_state_handler(rdi);
}

uint64_t syscall_19 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	halt_handler();
	return 0;
}
/* -----------------------------------------------------------------*/
uint64_t syscall_20 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return new_pipe_handler((char *) rdi);
}

uint64_t syscall_21 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return pipe_open_handler((char *) rdi);
}

uint64_t syscall_22 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	pipe_close_handler(rdi);
	return 0;
}

uint64_t syscall_23 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	pipe_status_handler();
	return 0;
}

uint64_t syscall_24 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return new_sem_handler((char *) rdi, rsi);
}

uint64_t syscall_25 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	return sem_open_handler((char *) rdi);
}

uint64_t syscall_26 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	sem_close_handler((mutNode *) rdi);
	return 0;
}

uint64_t syscall_27 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	sem_wait_handler((mutNode *) rdi);
	return 0;
}

uint64_t syscall_28 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	sem_post_handler((mutNode *) rdi);
	return 0;
}

uint64_t syscall_29 (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	sem_status_handler();
	return 0;
}