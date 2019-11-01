// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <memoryManager.h>
#include <moduleAddresses.h>
#include <process.h>
#include <console.h>
#include <timelib.h>

#include <scheduler.h>

/* Static auxiliary functions */
static nodeScheduler * search(uint64_t pid);
static nodeScheduler * search_prev(uint64_t pid);
static nodeScheduler * search_ready(nodeScheduler * start);
static void init_halt();
static uint8_t change_time();

/* Node of process running (circular list) */
static nodeScheduler * current;
static nodeScheduler * pHalt;

/* Synchronization variable for different stages */
static schedulerState stage;


/* Gives control to processes on list   */
uint64_t scheduler(uint64_t sp) {
    /* Only count timer if timer tick was not forced */
    if (stage != BLOCK && stage != DELETED && stage != STARTHALT) timer_handler(); 

    switch(stage) {
        /* When empty list, return the sp given (only at the begining) */
        case EMPTY: return sp; 

        /* When first process set stage, and return sp */
        case FIRST:
            stage = NORMAL; 
            current->process.state = RUNNING; 
            return current->process.sp;

        /* Normal functionality */
        case NORMAL: case BLOCK: current->process.sp = sp; break;

        /* If deleted previous current, not save sp and search */
        case DELETED: break;

        /* When halt is runnig */
        case HALT: pHalt->process.sp = sp; break;

        /* When seted to start halt from outside */
        case STARTHALT: stage = HALT; return pHalt->process.sp;
    }

    /* Only when normal and process has to remain on scheduler */
    if (stage == NORMAL && !change_time())
        return current->process.sp;
    
    /* Change current process (only when normal) */
    current->times = 0;
    if (stage == NORMAL)
        current->process.state = READY;
    
    nodeScheduler * toSet;
    /* If noraml or blocked don't consider current */
    if (stage == NORMAL || stage == BLOCK)
        toSet = search_ready(current->next);
    /* If normal or halt, start checking from current */
    else toSet = search_ready(current);

    /* Not found a ready process, set halt */
    if (toSet == 0) {
        stage = HALT;
        return pHalt->process.sp;
    }

    /* Run the found process */
    current = toSet;
    stage = NORMAL;
    current->process.state = RUNNING;
    return current->process.sp;
}

/* Initializes the scheduler */
void init_scheduler() {
    stage = EMPTY;
    current = 0;    
    init_halt();
}

/* Adds the process p to shceduler */
uint8_t add(process p) {
    /* Create new node */
    nodeScheduler * node = (nodeScheduler *)malloc(sizeof(nodeScheduler));
    if (node == 0) return 0; // No more memory

    /* Initializes node */
    node->times = 0;
    node->process = p;
    node->next = node;

    /* Its first process */
    if (current == 0) {  
        current = node;
        stage = FIRST;
        return p.pid;
    }

    node->next = current->next;
    current->next = node;
    return p.pid;
}

/* Kills current process */
void kill_current() {
    kill(get_pid());
    force_timer_tick();
}

/* Deletes a process given pid */
uint64_t kill(uint64_t pid) {
    if (current == 0 || pid < 1)
        return 0;
    
    /* Search of the previous node of the process to kill */
    nodeScheduler * prevNode = search_prev(pid);
    /* Not found */
    if (prevNode == 0) return 0;

    /* Only process on list */
    if (current == current->next) {
        remove(current->process);
        free(current);
        current = 0;
        stage = STARTHALT;
        // halt(); // TODO, is this needed? No need to kill cpu
        return pid;
    }

    nodeScheduler * toErase = prevNode->next;
    prevNode->next = toErase->next;
    
    /* If its kill of current process (allways called from kill_current) */
    if (toErase == current) {
        current = toErase->next;
        stage = DELETED;
    }

    remove(toErase->process);
    free(toErase);
    return pid;              
}

/* Sets priority of the process given its PID */
uint64_t set_priority(uint64_t pid, uint8_t n) {
    if (n > MAX_PRIO) n = MAX_PRIO;

    nodeScheduler * node = search(pid);
    if (node == 0) return 0;

    node->process.priority = n;
    return 1;
}

/* Get the state of the process given its PID */
states get_state(uint64_t pid) {
    nodeScheduler * node = search(pid);
    if (node == 0) return UNDEFINED;
    return node->process.state;
}

/* Sets the state of the process given its PID */
uint64_t set_state(uint64_t pid, states state) {
    if (state == RUNNING) return 0;
    nodeScheduler * node = search(pid);
    if (node == 0) return 0;
    
    /* Cannot set to ready a process blocked by a resource */
    if (state == READY && node->process.res != NONE) return 0; 

    /* If not the one currently running */
    if (node != current) {
        node->process.state = state;
        return 1;
    }

    /* If current process is set to READY */
    if (state == READY) {
        if (current->process.state == RUNNING) return 0;
        else node->process.state = state; // When all blocked
        return 1;
    }
    
    /* If we blocked the current process */
    node->process.state = state;
    stage = BLOCK;
    force_timer_tick();
    return 0;
}

/* Blocks current process (only used when blocked by resource) */
uint64_t block(resources res) {
    if (current == 0) return 0;
    current->process.state = BLOCKED;
    current->process.res = res;
    if (res == SEM) /*current->process.sem = sem*/; // TODO when mutex
    stage = BLOCK;
    
    
    force_timer_tick();
    return 1;
}

/* Unblocks process given its pid (only used when blocked by resource) */
uint64_t unblock(uint64_t pid) {
    if (current == 0) return 0;
    nodeScheduler * node = search(pid);
    if (node == 0) return 0;

    node->process.res = NONE;
    node->process.state = READY;
    return 1;
}

/* Returns current process pid */
uint64_t get_pid() {
    if (current == 0)
        return 0;
    return current->process.pid;
}

/* Returns current process ppid */
uint64_t get_ppid() {
    if (current == 0)
        return 0;
    return current->process.ppid;
}

/* Checks if a process is loaded */
uint8_t check_if_loaded() {
    return current != 0 && current->process.state == RUNNING;
}

/* Return current process */
nodeScheduler * get_current(){
    return current;
}

/* Prints all processes */
void list_all() {
    if (current == 0) {
        print("NOP\t-1\tThere is no Process in the scheduler");
        return;
    }

    nodeScheduler * node = current;
    print("\nName\tPID\tSP\t\tBP\tPrio\tLevel\t\tState\n");
    do {
        process p = node->process;
        char* stat = p.state==0? "Ready" : (p.state==1? "Running":"Blocked");
        char* lvl = p.context==0? "Foreground" : "Background";
        print(p.name); print("\t"); 
        printHex(p.pid); print("\t");
        printHex(p.sp); print("\t"); 
        printHex(p.bp); print("\t"); 
        printHex((uint64_t)p.priority); print("\t"); 
        print(lvl); print("\t"); 
        print(stat); print("\n");
        node = node->next;
    } while (node != current);
    print("\n");
}


/* Searches for a node given its pid */
static nodeScheduler * search(uint64_t pid) {
    nodeScheduler * node = search_prev(pid);
    if (node == 0) return 0;
    return node->next;
}

/* Searches for the previous node that matches the given pid */
static nodeScheduler * search_prev(uint64_t pid) {
    if (current == 0) return 0;
    nodeScheduler * node = current;
    do {
        if (pid == node->next->process.pid)
            return node;
        node = node->next;
    } while (node != current);
    return 0;
}

/* Initializes the halt process */
static void init_halt() {
    process aux = create_process(idleModuleAddress, "IDLE", FORE, 0, 0, 0, 0);
    pHalt = (nodeScheduler *)malloc(sizeof(nodeScheduler));
    if (pHalt == 0) return; // No more memory
    pHalt->times = 0;
    pHalt->process = aux;
    pHalt->next = pHalt;
}

/* Increments times and checks if current remains or not */
static uint8_t change_time() {
    current->times++;
    return current->times >= pow(2, MAX_PRIO - current->process.priority);
}

/* Returns the firs process ready starting from start to current, if not returns 0 */
static nodeScheduler * search_ready(nodeScheduler * start) {
    nodeScheduler * iterator = start;
    while (iterator->process.state != READY) {
        iterator = iterator->next;
        if (iterator == current) return 0;
    }
    return iterator;
}