/***************************************************
                    binarySem.h
****************************************************/

#ifndef _BINARY_SEM_H_
#define _BINARY_SEM_H_

#include <stdint.h>

typedef struct wait_node_mut {
    uint64_t pid;
    struct wait_node_mut * next; 
} waitNodeMut;

typedef struct mutex_data {
    char * name;
    uint64_t lock;
    waitNodeMut * blocked;
    waitNodeMut * last;
    uint64_t init;
    uint64_t pidAquired;
} mutex;

typedef struct mutex_node {
    mutex mutex;
    struct mutex_node * next;
} mutNode;

/* Opens an existing mutex */
mutNode * open_mutex(char * name);

/* Create a new mutex */
mutNode * new_mutex(char * name, uint8_t init);

/* Perform wait on an existing mutex */
void wait_mutex(mutNode * mutex);

/* Perform post on an existing mutex */
void post_mutex(mutNode * mutex);

/* Close an existing mutex */
void close_mutex(mutNode * mutex);

/* Deallocate system resources aquired by process for mutex */
void deallocate_mutex(mutNode * mutex, uint64_t pid);

/* Print all mutexes */
void show_all_mutex();

/* Print list of blocked processes */
void print_blocked_processes(mutNode * mutex);

#endif /* _BINARY_SEM_H_ */