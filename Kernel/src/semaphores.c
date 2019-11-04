#include <strings.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <console.h>

#include <sem.h>

static semNode * first = 0;

/* Check if a sem pointer belongs to the sem list */
static int search_sem(semNode * sem);

/* Opens an existing semaphore */
semNode * open_sem(char * name) {
    semNode * iterator = first;
    while (iterator != 0) {
        if (stringcmp(name, iterator->sem.name))
            return iterator;
        iterator = iterator->next;
    }
    return 0;
}

/* Create a new semaphore */
semNode * new_sem(char * name, uint8_t init) {
    /* Check if the sem already exists */
    semNode * prevSem = open_sem(name);
    if (prevSem != 0) return prevSem;

    /* Create semaphore */
    semaphore sem;
    sem.name = (char *) malloc(stringlen(name) + 1);
    if (sem.name == 0) return 0; // No more Memory
    stringcp(sem.name, name);

    char semNames[stringlen(name) + 6], mutAux[] = "_mut", delAux[] = "_del";
    stringcp(semNames, name);
    stringcat(semNames, mutAux);
    sem.mutex = new_mutex(semNames, 0); // Mutex starts unlocked

    stringcp(semNames, name);
    stringcat(semNames, delAux);
    // If init == 0, Delay starts locked. If not, Delay starts unlocked
    sem.delay = new_mutex(semNames, (init == 0) ? 1 : 0);
    
    sem.count = init;

    /* Create node */
    semNode * node = (semNode *) malloc(sizeof(semNode));
    if (node == 0) {
        free(sem.name);
        return 0; // No more Memory
    }
    node->next = 0;
    node->sem = sem;

    /* Add node to list */
    if (first != 0)
        node->next = first;
    first = node;

    return node;
}

/* Perform wait on an existing semaphore */
void wait_sem(semNode * sem) {
    /* Check if sem exists */
    if (!search_sem(sem)) return;

    /* Waits for the sem to be green */
    wait_mutex(sem->sem.delay);
    
    /* Prevents concurrent modifications */
    wait_mutex(sem->sem.mutex);
    
    /* Update count. If greater than 0, more processes shall pass */
    sem->sem.count--;
    if (sem->sem.count > 0)
        post_mutex(sem->sem.delay);
    
    post_mutex(sem->sem.mutex);
}

/* Perform post on an existing semaphore */
void post_sem(semNode * sem) {
    /* Check if sem exists */
    if (!search_sem(sem)) return;

    /* Prevents concurrent modifications */
    wait_mutex(sem->sem.mutex);
    
    /* Update count. If 1, start post chain (each process will continue if greater than 1) */
    sem->sem.count++;
    if (sem->sem.count == 1)
        post_mutex(sem->sem.delay);
    post_mutex(sem->sem.mutex);
}

/* Close an existing semaphore */
void close_sem(semNode * sem) {
    close_mutex(sem->sem.mutex);
    close_mutex(sem->sem.delay);
}

/* Print all semaphores */
void show_all_sem() {
    /* Check if list is empty */
    if (first == 0) {
        print("\nThere are no Semaphores created");
        return;
    }

    /* Print each sem's state */
    semNode * iterator = first;
    print("\nName\t\tCount\t\tBlocked Processes\n");
    while (iterator != 0) {
        print(iterator->sem.name); print("\t\t"); 
        print("%d", iterator->sem.count); print("\t\t");
        print_blocked_processes_sem(iterator);
        print("\n");
        iterator = iterator->next;
    }
}

/* Print list of blocked processes */
void print_blocked_processes_sem(semNode * sem) {
    print_blocked_processes(sem->sem.mutex);
    print_blocked_processes(sem->sem.delay);
}

/* Check if a sem pointer belongs to the sem list */
static int search_sem(semNode * sem) {
    semNode * iterator = first;
    while (iterator != 0) {
        if (iterator == sem) return 1;
        iterator = iterator->next;
    }
    return 0;
}
