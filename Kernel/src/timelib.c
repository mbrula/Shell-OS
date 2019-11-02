// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManager.h>
#include <scheduler.h>
#include <process.h>
#include <console.h>

#include <timelib.h>

/* Some static functions for list management */
static void add_node_T(uint64_t pid, uint64_t time);
static void update_list();
static void remove_next_T(nodeTime * node);
static void print_list_time();

/* static variables */
static nodeTime * firstT = 0;
static uint64_t ticks = 0;

/* Handles timer */
void timer_handler() {
	ticks++;
    update_list();
}

/* Count of elapsed ticks */
int ticks_elapsed() {
	return ticks;
}

/* Seconds elapsed */
int seconds_elapsed() {
	return ticks / 18;
}

/* Syscall calls here */
void sleep(uint64_t millis) {
    if (millis < 55) return;
    uint64_t pid = get_pid();
    add_node_T(pid, millis);
    block(TIME, 0);
}

/* Updates the values off all the waiting processes */
static void update_list() {
    nodeTime * iterator = firstT;
    nodeTime * prev = firstT;

    /* We iterate on list */
    while (iterator != 0) {
        iterator->time--;
        /* We free the process */
        if (iterator->time == 0) {
            nodeTime * next = iterator->next;
            unblock(iterator->pid);
            free(iterator);

            /* First node */
            if (prev->next == next) {
                firstT = next;
                prev = next;
            } else prev->next = next;
            iterator = next;
        } else {
            /* Normal iteration */
            prev = iterator;
            iterator = iterator->next;
        }        
    }
}

/* Removes the next node of the given one */
static void remove_next_T(nodeTime * node) {
    nodeTime * toFree = node->next;
    node->next = toFree->next;
    free(toFree);
}

/* Removes a node given its pid */
void remove_node_T(uint64_t pid) {
    if (firstT == 0) return;

    nodeTime * aux = firstT;

    /* If its the first one */
    if (firstT->pid == pid) {
        firstT = firstT->next;
        free(aux);
        return;
    }

    for (aux = firstT; aux->next != 0 && aux->next->pid != pid; aux = aux->next);
    if (aux->next == 0) return; // Not found
    remove_next_T(aux);
}

/* Adds a Node to the begining of the list */
static void add_node_T(uint64_t pid, uint64_t time) {
   
    nodeTime * node = (nodeTime *) malloc(sizeof(nodeTime));
    node->pid = pid;
    node->time = time / TICK_LENGTH;
    node->next = 0;

    if (firstT != 0) node->next = firstT;    
    firstT = node;
}

/* Auxiliary fucntion to print nodes */
static void print_list_time() {
    print("\nList of sleep nodes:\n");
    nodeTime * iterator = firstT;
    uint64_t count = 0;
    while (iterator != 0) {
        print("INDEX: %d - PID: %d - REQ: %d\n", count++, iterator->pid, iterator->time);
        iterator = iterator->next;
    }
    
}