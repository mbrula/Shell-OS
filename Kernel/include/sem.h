#ifndef _SEM_H_
#define _SEM_H_

#include <stdint.h>
#include <mutex.h>

typedef struct sem_data {
    char * name;
    mutNode * mutex;
    mutNode * delay;
    uint64_t count;
} semaphore;

typedef struct sem_node {
    semaphore sem;
    struct sem_node * next;
} semNode;

/* Opens an existing semaphore */
semNode * open_sem(char * name);

/* Create a new semaphore */
semNode * new_sem(char * name, uint8_t init);

/* Perform wait on an existing semaphore */
void wait_sem(semNode * sem);

/* Perform post on an existing semaphore */
void post_sem(semNode * sem);

/* Close an existing semaphore */
void close_sem(semNode * sem);

/* Deallocate system resources aquired by process for semaphore */
void deallocate_sem(semNode * sem, uint64_t pid);

/* Print all semaphores */
void show_all_sem();

/* Print list of blocked processes */
void print_blocked_processes_sem(semNode * sem);

#endif /* _SEM_H_ */