#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>
#include <lib.h>
#include <process.h>
#include <mutex.h>

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

uint64_t create_handler(char * name, uint64_t argc, char ** argv, level context, char * fds);
uint64_t kill_handler(uint64_t pid);
uint64_t get_pid_handler();
void list_all_process_handler();
uint64_t set_priority_handler(uint64_t pid, uint8_t prio);
uint64_t change_state_handler(uint64_t pid);
void halt_handler();

int new_pipe_handler(char * name);
int pipe_open_handler(char * name);
void pipe_close_handler(int fd);
void pipe_status_handler();
uint64_t new_sem_handler(char * name, uint64_t init);
uint64_t sem_open_handler(char * name);
void sem_close_handler(mutNode * sem);
void sem_wait_handler(mutNode * sem);
void sem_post_handler(mutNode * sem);
void sem_status_handler();

#endif /* _SYSCALLS_H_ */