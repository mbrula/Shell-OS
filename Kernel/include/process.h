#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdint.h>

#define STACK_SIZE	3000

/* Posible states of a process */
typedef enum {READY = 0, RUNNING, BLOCKED, UNDEFINED} states;

/* Type of resources used by the process */
typedef enum {NONE = 0, TIME, SEM} resources;

/* Defines for the level of the process */
typedef enum {FORE = 0, BACK} level;

/* Structure that defines a process */
typedef struct {
    /* Properties */
    char * name;
    uint64_t pid;
    uint64_t ppid;

    /* Stack and pointers */
    uint64_t sp;
    uint64_t bp;
    void * stack;

    /* State */
    uint8_t priority;
    level context;
    states state;

    /* Resources */
    resources res;
    // TODO add sem here    
} process;

/* Creates a new process and adds it to scheduler */
uint64_t add_process(void * entryPoint, char * name, level context, uint64_t argc, char ** argv, int inAlias, int outAlias);

/* Creates a new process */
process create_process(void * entryPoint, char * name, level context, uint64_t argc, char ** argv, int inAlias, int outAlias);

/* Deletes process */
void remove(process p);

/* If a valid process, kill */
void sig_int();

/* Print process stack */
void print_process_stack(process p);

#endif /* _PROCESS_H_ */