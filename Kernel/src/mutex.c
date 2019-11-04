// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <strings.h>
#include <lib.h>
#include <scheduler.h>
#include <memoryManager.h>
#include <console.h>

#include <mutex.h>

static mutNode * first = 0;

/* Static  auxiliary functions */
static int search_mutex(mutNode * mutex);
static void close_pid_mutex(mutNode * mutex, uint64_t pid);

/* Opens an existing mutex */
mutNode * open_mutex(char * name) {
    mutNode * iterator = first;
    while (iterator != 0) {
        if (stringcmp(name, iterator->mutex.name)) 
            return iterator;
        iterator = iterator->next;
    }
    return 0;    
}

/* Create a new mutex. INIT 0 means UNLOCKED, INIT 1 means LOCKED */
mutNode * new_mutex(char * name, uint8_t init) {
    /* Check if the mutex already exists */
    mutNode * prevMutex = open_mutex(name);
    if (prevMutex != 0) return prevMutex;

    /* Create mutex */
    mutexD mutex;
    mutex.name = (char *) malloc(stringlen(name) + 1);
    if (mutex.name == 0) return 0; // No more Memory
    stringcp(mutex.name, name);
    mutex.blocked = 0;
    mutex.last = 0;
    mutex.pidAquired = 0;

    /* Set initial values. If init bigger than 1, save 1 */
    if (init >= 1) {
        mutex.lock = 1;
        mutex.init = 1;
    } else {
        mutex.lock = 0;
        mutex.init = 0;
    }

    /* Create node */
    mutNode * node = (mutNode *) malloc(sizeof(mutNode));
    if (node == 0) {
        free(mutex.name);
        return 0;  // No more memory
    }
    node->next = 0;
    node->mutex = mutex;

    /* Add node to list */
    if (first != 0)
        node->next = first;
    first = node;

    return node;
}

/* Perform wait on an existing mutex */
void wait_mutex(mutNode * mutex) {
    /* Check if mutex exists */
    if (!search_mutex(mutex)) return;

    /* If 0 returned, aquired mutex */
    if (mutex_acquire(&mutex->mutex.lock) == 0) {
        mutex->mutex.pidAquired = get_pid();
        return;
    }

    /* mutex locked */
    /* Create waitNodeMut */
    waitNodeMut * node = (waitNodeMut *) malloc(sizeof(waitNodeMut));
    if (node == 0) return; // No more memory
    node->pid = get_pid();
    node->next = 0;

    /* Add the node to the end of the blocked list */
    if (mutex->mutex.blocked == 0)
        mutex->mutex.blocked = node;
    else
        mutex->mutex.last->next = node;
    mutex->mutex.last = node;

    /* Block the current process with mutex */
    block(SEM, mutex);
}

/* Perform post on an existing mutex */
void post_mutex(mutNode * mutex) {
    /* Check if mutex exists */
    if (!search_mutex(mutex)) return;
    
    mutex->mutex.pidAquired = 0;

    /* If no processes are waiting, release lock */
    if (mutex->mutex.blocked == 0) {
        mutex_release(&mutex->mutex.lock);
        return;
    } 

    /* Unblock the first process from the list */
    waitNodeMut * p = mutex->mutex.blocked;
    mutex->mutex.blocked = mutex->mutex.blocked->next;
    mutex->mutex.pidAquired = p->pid;
    unblock(p->pid);
    
    /* Free the waitNodeMut block */
    free((void *) p);
}

/* Close an existing mutex */
void close_mutex(mutNode * mutex) {
    close_pid_mutex(mutex, get_pid());
}

/* Deallocate system resources aquired by process for mutex */
void deallocate_mutex(mutNode * mutex, uint64_t pid) {
    if (!search_mutex(mutex) || pid == 0) return;

    /* Post every mutex aquired by PID process if their init was unlocked */
    mutNode * pidIt = first;
    while (pidIt != 0) {
        if (pidIt->mutex.pidAquired == pid)
            if (pidIt->mutex.init == 0)
                post_mutex(pidIt);
            else close_pid_mutex(pidIt, pid);
        else close_pid_mutex(pidIt, pid);
        pidIt = pidIt->next;
    }    
}

/* Print all mutexes */
void show_all_mutex() {
    /* Check if list is empty */
    if (first == 0) {
        print("\nThere are no Mutexes created");
        return;
    }

    /* Print each mutex's state */
    mutNode * iterator = first;
    print("\nName\t\t\tState\t\t\tBlocked Processes\n");
    while (iterator != 0) {
        print(iterator->mutex.name); print("\t\t"); 
        print((iterator->mutex.lock) ? "Locked" : "Unlocked"); print("\t\t");
        print_blocked_processes(iterator);
        print("\n");
        iterator = iterator->next;
    }
}

/* Check if a mutex pointer belongs to the mutex list */
static int search_mutex(mutNode * mutex) {
    mutNode * iterator = first;
    while (iterator != 0) {
        if (iterator == mutex) return 1;
        iterator = iterator->next;
    }
    return 0;
}

/* Close an existing mutex for the process of the given pid */
static void close_pid_mutex(mutNode * mutex, uint64_t pid) {
    waitNodeMut * iterator = mutex->mutex.blocked;
    waitNodeMut * prev = 0;

    /* Check if blocked list is empty */
    if (iterator == 0) return;

    /* Search for PID in blocked list */
    while (iterator != 0 && iterator->pid != pid) {
        prev = iterator;
        iterator = iterator->next;
    }

    /* PID not found */
    if (iterator == 0) return;

    /* PID found. Update blocked list */
    if (prev == 0) mutex->mutex.blocked = iterator->next;
    else prev->next = iterator->next;
    if (iterator->next == 0) mutex->mutex.last = prev;

    /* Free the waitNodeMut block */
    free((void *) iterator);
}

/* Print list of blocked processes */
void print_blocked_processes(mutNode * mutex) {
    if (mutex == 0) return;
    waitNodeMut * iterator = mutex->mutex.blocked;
    while (iterator != 0) {
        print(" - %d", iterator->pid);
        iterator = iterator->next;
    }
}