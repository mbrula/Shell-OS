// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>

#define READ_ID     0
#define WRITE_ID    1
#define TICKS_ID    2
#define CLEAR_ID    3
#define RTC_ID      4
#define SLEEP_ID    5
#define BEEP_ID     6
#define EXIT_ID     7
#define PIXEL_ID    8

#define MALLOC_ID   9
#define FREE_ID     10
#define STATUS_ID   11
#define GET_STATUS_ID   12

#define NEW_PROC_ID     13
#define KILL_ID         14
#define PID_ID          15
#define PS_ID           16
#define SET_PRIO_ID     17
#define SET_STATE_ID    18

#define HALT_ID     19

#define MAX_BUFFER 100

void putchar(uint8_t character) {
    syscall(WRITE_ID, STDOUT, (uint64_t) &character, 1, 0, 0, 0);
}

void puts(const char * string) {
    syscall(WRITE_ID, STDOUT, (uint64_t) string, strlen(string), 0, 0, 0);
}

void perror(const char * string) {
    syscall(WRITE_ID, STDERR, (uint64_t) string, strlen(string), 0, 0, 0);
}

void printf(char * str, ...){
    va_list list;
    va_start(list, str);
    int i = 0;

    char newStr[MAX_BUFFER] = {0};
    int len = 0;

    while(str[i] != 0){
    	if(str[i] == '%' && (i == 0 || str[i-1] != '\\')){
            char buffer[MAX_BUFFER] = {0};
            switch (str[i+1]) {
                case 'd':
                    itoa((int) va_arg(list,int), buffer, 10);
                    len += concat((newStr + len), buffer);
                    i += 2;
                    break;
                case 's':
                    len += concat((newStr + len), va_arg(list,char*));
                    i += 2;
                    break;
                default:
                    i += 2;
                    break;
            }
        }
        else {
            newStr[len] = str[i];
            len++;
            i++;
        }
    }
    va_end(list);
    newStr[len] = 0;
    len++;
    syscall(WRITE_ID, STDOUT, (uint64_t) newStr, len, 0, 0, 0);
}

void putsFd(const char * string, uint64_t fd) {
    syscall(WRITE_ID, fd, (uint64_t) string, strlen(string), 0, 0, 0);
}

void putcharFd(uint8_t character, uint64_t fd) {
    syscall(WRITE_ID, fd, (uint64_t) &character, 1, 0, 0, 0);
}

void printfFd(uint64_t fd, char * str, ...) {
    va_list list;
    va_start(list, str);
    int i = 0;

    char newStr[MAX_BUFFER] = {0};
    int len = 0;

    while(str[i] != 0){
    	if(str[i] == '%' && (i == 0 || str[i-1] != '\\')){
            char buffer[MAX_BUFFER] = {0};
            switch (str[i+1]) {
                case 'd':
                    itoa((int) va_arg(list,int), buffer, 10);
                    len += concat((newStr + len), buffer);
                    i += 2;
                    break;
                case 's':
                    len += concat((newStr + len), va_arg(list,char*));
                    i += 2;
                    break;
                default:
                    i += 2;
                    break;
            }
        }
        else {
            newStr[len] = str[i];
            len++;
            i++;
        }
    }
    va_end(list);
    newStr[len] = 0;
    len++;
    syscall(WRITE_ID, fd, (uint64_t) newStr, len, 0, 0, 0);
}

char getchar() {
    char character = 0;
    while( character == 0) syscall(READ_ID, STDIN, (uint64_t) &character, 1, 0, 0, 0);
    return character;
}

/* Reads size bytes (or until Enter) from STDIN and saves them in string. PRINTS CHARACTERS WHILE READING */
/* Return # characters read if pressed Enter. Returns -1 if EOF recieved */
int gets(char * string, uint64_t size) {
    uint64_t index = 0;
    int car;
    while (index < size - 1 && (car = getcharFd(STDIN)) != '\n' && car >= 0) {
        if (car == '\b') {
            if (index > 0) {
                index--;
                putchar(car);
            }
        } else {
            string[index++] = car;
            putchar(car);
        }
    }
    string[index++] = 0;
    if (car >= 0)
        return index;
    return -1;
}

char getcharFd(uint64_t fd) {
    char character = 0; // TODO change dis
    while(character == 0) syscall(READ_ID, fd, (uint64_t) &character, 1, 0, 0, 0);
    return character;
}

/* Reads size bytes (or until Enter) from fileDescriptor and saves them in string. WONT PRINT WHILE READING */
int getsFd(char * string, uint64_t size, uint64_t fd) {
    uint64_t index = 0;
    uint8_t car;
    while (index < size - 1 && (car = getcharFd(fd)) != '\n') {
        if (car == '\b') {
            if (index > 0) {
                index--;
            }
        } else {
            string[index++] = car;
        }
    }
    string[index++] = 0;
    return index;
}

void clearScreen() {
    syscall(CLEAR_ID, 0, 0, 0, 0, 0, 0);
}

// Formato de fecha: dd-mm-yyyy 
// date debe ser un vector de al menos 11 posiciones
char * getDate(char * date) {
    uint64_t day = syscall(RTC_ID, DAY_REG, 0, 0, 0, 0, 0);
    uint64_t month = syscall(RTC_ID, MONTH_REG, 0, 0, 0, 0, 0);
    uint64_t year = 2000 + syscall(RTC_ID, YEAR_REG, 0, 0, 0, 0, 0); // Devuelve numero desde 2000

    saveTwoDigit(day, date);
    saveTwoDigit(month, date + 3);    
    itoa(year, date + 6, 10);

    date[2] = date[5] = DATE_SEP;
    date[10] = 0;

    return date;
}

// Formato de hora:  hh:mm:ss
// time debe ser un vector de al menos 9 posiciones
char * getTime(char * time) {
    uint64_t hour = syscall(RTC_ID, HOUR_REG, 0, 0, 0, 0, 0);
    uint64_t min = syscall(RTC_ID, MIN_REG, 0, 0, 0, 0, 0);
    uint64_t sec = syscall(RTC_ID, SEC_REG, 0, 0, 0, 0, 0); // Devuelve numero desde 2000

    saveTwoDigit(hour, time);
    saveTwoDigit(min, time + 3);    
    saveTwoDigit(sec, time + 6);

    time[2] = time[5] = TIME_SEP;
    time[8] = 0;

    return time;
}

void sleep(uint64_t millis) {
    syscall(SLEEP_ID, millis, 0, 0, 0, 0, 0);
}

void beep(uint32_t frequency, uint64_t millis) {
    syscall(BEEP_ID, frequency, 1, 0, 0, 0, 0); // PLAY
    syscall(SLEEP_ID, millis, 0, 0, 0, 0, 0);
    syscall(BEEP_ID, frequency, 0, 0, 0, 0, 0); // STOP
}

void exit() {
    syscall(KILL_ID, getPid(), 0, 0, 0, 0, 0);
    // syscall(EXIT_ID, 0, 0, 0, 0, 0, 0);
}

void halt() {
    syscall(HALT_ID, 0, 0, 0, 0, 0, 0);
}

void finish() {
    syscall(EXIT_ID, 0, 0, 0, 0, 0, 0);
}

uint64_t getTicks() {
    return syscall(TICKS_ID, 0, 0, 0, 0, 0, 0);
}

void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    uint64_t rgb = getRGB(r, g, b);
    syscall(PIXEL_ID, x, y, rgb, 0, 0, 0);
}

/* TP2-SO */

/* Aloca un bloque de al menos size memoria y devuelve un void * apuntando a la direccion del bloque.
   En caso de no haber memoria disponible, devuelve NULL */
/* Alloc a memory block of at leas size bytes and return a void * ptr to the block or 0 if error */
void * malloc(uint32_t size) {
    return (void *) syscall(MALLOC_ID, size, 0, 0, 0, 0, 0);
}

/* Free the memory block pointed by ptr */
void free(void * ptr) {
    syscall(FREE_ID, (uint64_t) ptr, 0, 0, 0, 0, 0);
}

/* Print memory size, occupied memory and free memory (STDOUT) */
void memStatus() {
    syscall(STATUS_ID, STDOUT, 0, 0, 0, 0, 0);
}

/* Print memory size, occupied memory and free memory (outFd) */
void memStatusFd(uint64_t outFd) {
    syscall(STATUS_ID, outFd, 0, 0, 0, 0, 0);
}

/* Print memory size, occupied memory and free memory (outFd) */
void memGetStatus(uint64_t * total, uint64_t * free, uint64_t * occupied) {
    syscall(GET_STATUS_ID, (uint64_t)total, (uint64_t)free, (uint64_t)occupied, 0, 0, 0);
}

/* Create a new process based on an entryPoint */
// DEPRECADA
uint64_t fork(void * entryPoint, char * name) {
    return syscall(NEW_PROC_ID, (uint64_t) entryPoint, (uint64_t) name, 0, 0, 0, 0);
}

/* Create a new process based on its name */
uint64_t newProcess(const char * name, uint64_t argc, char * argv[], uint64_t ground, uint64_t inFd, uint64_t outFd) {
    if (ground == FOREGROUND || ground == BACKGROUND)
        return syscall(NEW_PROC_ID, (uint64_t) name, argc, (uint64_t) argv, ground, inFd, outFd);
    return 0;
}

/* Kills a process given its pid */
uint64_t kill(uint64_t pid) {
    return syscall(KILL_ID, pid, 0, 0, 0, 0, 0);
}

/* Returns current process pid */
uint64_t getPid() {
    return syscall(PID_ID, 0, 0, 0, 0, 0, 0);
}

/* List all running processes (STDOUT) */
void ps() {
    syscall(PS_ID, STDOUT, 0, 0, 0, 0, 0);
}

/* List all running processes (outFd) */
void psFd(uint64_t outFd) {
    syscall(PS_ID, outFd, 0, 0, 0, 0, 0);
}

/* Set process priority to n */
uint64_t setPriority(uint64_t pid, uint64_t n) {
    return syscall(SET_PRIO_ID, pid, n, 0, 0, 0, 0);
}

/* Changes process state between READY and BLOCKED  */
uint64_t changeState(uint64_t pid) {
    return syscall(SET_STATE_ID, pid, 0, 0, 0, 0, 0);
}

/* Create a new named pipe */
uint64_t newPipe(char * name) {
    // return syscall(PIPE_ID, name, 0, 0, 0, 0, 0);
    return 0;
}

/* List all pipes (STDOUT) */
void pipeStatus() {
    // syscall(PIPE_STATUS_ID, STDOUT, 0, 0, 0, 0, 0);
}

/* List all pipes (outFd) */
void pipeStatusFd(uint64_t outFd) {
    // syscall(PIPE_STATUS_ID, outFd, 0, 0, 0, 0, 0);
}

// newSem
// closeSem
// wait
// post

/* List all semaphores (STDOUT) */
void semStatus() {
    // syscall(SEM_STATUS_ID, STDOUT, 0, 0, 0, 0, 0);
}

/* List all semaphores (outFd) */
void semStatusFd(uint64_t outFd) {
    // syscall(SEM_STATUS_ID, outFd, 0, 0, 0, 0, 0);
}