#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>
#include <lib.h>

/* Dispatcher for software interrupts */
uint64_t handleSyscall(uint64_t sirq, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

void read_handler(uint64_t fd, char * buff, uint64_t count);
void write_handler(uint64_t fd, const char * buff, uint64_t count);
uint64_t time_handler();
void clear_handler();
uint64_t rtc_handler(uint8_t register);
void sleep_handler(uint64_t millis);
void beep_handler(uint16_t frequency, uint64_t time);
void exit_handler();
void pixel_handler(uint64_t x, uint64_t y, uint64_t rgb);

void * malloc_handler(uint64_t bytes);
void free_handler(void * ptr);
void mm_print_status_handler();
void mm_get_status_handler(uint64_t * total, uint64_t * occupied, uint64_t * free);

#endif /* _SYSCALLS_H_ */