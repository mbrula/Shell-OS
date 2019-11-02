// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <keyboard.h>
#include <timelib.h>
#include <console.h>
// #include <naiveConsole.h>
#include <sound.h>
#include <memoryManager.h>
#include <moduleAddresses.h>
#include <process.h>
#include <strings.h>
#include <scheduler.h>

#include <mutex.h>
#include <interrupts.h>
#include <fileDescriptors.h>
#include <strings.h>

/* Located on loader.asm */
extern void hang();

void read_handler(uint64_t fd, char * buffer, uint64_t count) {
    read(fd, buffer, count);
}

void write_handler(uint64_t fd, const char * buffer, uint64_t count) {
    write(fd, buffer, count);
}

uint64_t time_handler() {
    return ticks_elapsed();
}

void clear_handler() {
    clear_console();
}

uint64_t rtc_handler(uint8_t reg){
    write_port(0x70, reg);
    uint64_t aux = read_port(0x71);
    return (aux >> 4) * 10 + (aux & 0x0F);
}

void sleep_handler(uint64_t millis){
    sleep(millis);
}

void beep_handler(uint16_t frequency, uint64_t state) {
    switch (state)
    {
        case 1: // PLAY
            play_sound(frequency);
            break;
        case 0: // STOP
            no_sound();
            break;
        default:
            break;
    }
}

void exit_handler() {
    hang();
}

void pixel_handler(uint64_t x, uint64_t y, uint64_t rgb) {
    Point auxPos = {x, y};
    Color auxColor = {(rgb & 0xFF0000) >> 16, (rgb & 0x00FF00) >> 8, rgb & 0x0000FF};
    draw_pixel(auxPos, auxColor);
}

/* ---------------------------- */

void * malloc_handler(uint64_t bytes) {
    return malloc(bytes);
}

void free_handler(void * ptr) {
    free(ptr);
}

void mm_print_status_handler() {
    mm_print_status();
}

void mm_get_status_handler(uint64_t * total, uint64_t * occupied, uint64_t * free) {
    mm_status(total, occupied, free);
}

/* ----------------------------- */

static void * get_module_address(char * name) {
    if (stringcmp(name, "SHELL"))
        return shellModuleAddress;
    if (stringcmp(name, "SLEEP"))
        return sleepModuleAddress;
    if (stringcmp(name, "LOOP"))
        return loopModuleAddress;
    if (stringcmp(name, "CAT"))
        return catModuleAddress;
    if (stringcmp(name, "WC"))
        return wcModuleAddress;
    if (stringcmp(name, "FILTER"))
        return filterModuleAddress;
    if (stringcmp(name, "PHYLO"))
        return phyloModuleAddress;
    if (stringcmp(name, "PROCESS_A"))
        return processAModuleAddress;
    if (stringcmp(name, "PROCESS_B"))
        return processBModuleAddress;
    return 0;
}

uint64_t create_handler(char * name, uint64_t argc, char ** argv, level context, char * fds) {
    int inFd = 0, outFd = 1, cursor = 0;
    char aux[5];

    stringcp_until_space(aux, fds);
    inFd = atoi(aux, &cursor);
    stringcp_until_space(aux, fds + cursor + 1);
    outFd = atoi(aux, &cursor);
    
    /* Search for process name in available processes */
    void * entryPoint = get_module_address(name);
    if (entryPoint == 0 || inFd < 0 || outFd < 0)
        return 0;
    return add_process(entryPoint, name, context, argc, argv, inFd, outFd);
}

uint64_t kill_handler(uint64_t pid) {
    return kill(pid);
}

uint64_t get_pid_handler() {
    return get_pid();
}

void list_all_process_handler() {
    list_all();
}

uint64_t set_priority_handler(uint64_t pid, uint8_t prio) {
    return set_priority(pid, prio);
}

uint64_t change_state_handler(uint64_t pid) {
    states state = get_state(pid);
	switch (state) {
		case READY:
			return set_state(pid, BLOCKED);
		case BLOCKED:
			return set_state(pid, READY);
		default:
			return 1;
	}
}

void halt_handler() {
    _hlt();
}

int new_pipe_handler(char * name) {
    return new_pipe(name);
}

int pipe_open_handler(char * name) {
    return open_pipe(name);
}

void pipe_close_handler(int fd) {
    close_pipe(fd);
}

void pipe_status_handler(){
    show_all_pipes();
}

// TODO: Change to Semaphore when implemented
uint64_t new_sem_handler(char * name, uint64_t init){
    return (uint64_t) new_mutex(name, init);
}

uint64_t sem_open_handler(char * name) {
    return (uint64_t) open_mutex(name);
}

void sem_close_handler(mutNode * sem){
    close_mutex(sem);
}

void sem_wait_handler(mutNode * sem){
    wait_mutex(sem);
}

void sem_post_handler(mutNode * sem){
    post_mutex(sem);
}

void sem_status_handler(){
    show_all_mutex();
}