#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#define MAX_PRIO 3
#define SIZE 4000

#include <process.h>
#include <mutex.h>

typedef enum {EMPTY = 0, FIRST, NORMAL, BLOCK, DELETED, HALT, STARTHALT} schedulerState;

/* Structure for the node in scheduler */
typedef struct node_sched {
    /* For proirity management */
    uint64_t times;

    /* The process */
    process process;

    struct node_sched * next;
} nodeScheduler;

/* Creates the scheduler */
void init_scheduler();

/* Chooses next process to give the CPU */
uint64_t scheduler(uint64_t sp);

/* Adds a new process to the scheduler */
uint8_t add(process p);

/* Kills current process */
void kill_current();

/* Deletes a process given PID */
uint64_t kill(uint64_t pid);

/* Sets priority of the process given its PID */
uint64_t set_priority(uint64_t pid, uint8_t n);

/* Get the state of the process given its PID */
states get_state(uint64_t pid);

/* Sets the state of the process given its PID */
uint64_t set_state(uint64_t pid, states state);

/* Blocks current process (only used when blocked by resource) */
uint64_t block(resources res , mutNode * mutex) ;

/* Unblocks process given its pid (only used when unblocked by resource) */
uint64_t unblock(uint64_t pid);

/* Returns current process pid */
uint64_t get_pid();

/* Returns current process parent pid */
uint64_t get_ppid();

/* Checks if a process is loaded */
uint8_t check_if_loaded();

/* Return current process */
nodeScheduler * get_current();

/* Prints all processes */
void list_all();

#endif /* _SCHEDULER_H_ */