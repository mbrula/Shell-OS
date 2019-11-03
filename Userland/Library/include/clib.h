#ifndef CLIB_H_
#define CLIB_H_

#include <stdint.h>
#include <stdarg.h>

#define DAY_REG     0X07
#define MONTH_REG   0X08
#define YEAR_REG    0X09
#define HOUR_REG    0X04
#define MIN_REG     0X02
#define SEC_REG     0X00

#define DATE_SEP    '-'
#define TIME_SEP    ':'

#define STDIN       0
#define STDOUT      1
#define STDERR      2

#define FOREGROUND      0
#define BACKGROUND      1

#define MAX_LENGTH  200

void puts(const char * string);
void putsFd(const char * string, uint64_t fd);
void perror(const char * string);
void putchar(uint8_t character);
void putcharFd(uint8_t character, uint64_t fd);
void printf(char * str, ...);
void printfFd(uint64_t fd, char * str, ...);
char getchar(void);
int gets(char * string, uint64_t size);
char getcharFd(uint64_t fd);
int getsFd(char * string, uint64_t size, uint64_t fd);
void clearScreen(void);
char * getDate(char * date);
char * getTime(char * time);
void sleep(uint64_t millis);
void beep(uint32_t frequency, uint64_t millis);
void exit();
void halt();
void finish();
uint64_t getTicks();
void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

/* Memory functions */
void * malloc(uint32_t size);
void free(void * ptr);
void memStatus();
void memGetStatus(uint64_t * total, uint64_t * free, uint64_t * occupied);

/* Processes functions */
uint64_t fork(void * entryPoint, char * name);
uint64_t newProcess(const char * name, uint64_t argc, char * argv[], uint64_t ground, uint64_t inFd, uint64_t outFd);
uint64_t kill(uint64_t pid);
uint64_t getPid();
void ps();
uint64_t setPriority(uint64_t pid, uint64_t n);
uint64_t changeState(uint64_t pid);

/* Create a new named pipe */
int newPipe(char * name);

/* Open an existing named pipe */
int pipeOpen(char * name);

/* Close an existing named pipe */
void pipeClose(int fd);

/* List all pipes */
void pipeStatus();

/* Create a new named semaphore */
uint64_t newSem(char * name, uint64_t init);

/* Open an existing named semaphore */
uint64_t semOpen(char * name);

/* Close an existing named semaphore */
void semClose(uint64_t sem);

/* Wait for a named semaphore */
void semWait(uint64_t sem);

/* Post on a named semaphore */
void semPost(uint64_t sem);

/* List all semaphores */
void semStatus();

extern uint64_t syscall(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

#endif /* CLIB_H_ */