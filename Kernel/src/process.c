// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <memoryManager.h>
#include <console.h>
#include <strings.h>
#include <stack.h>
#include <scheduler.h>
#include <timelib.h>

#include <process.h>

/* Static auxiliary functions */
static void free_parent(process p);
static fdNode * add_process_alias(int oldFd, int newFd);
static void remove_fdlist(process p);

/* Static count of the pids given */
static uint64_t c_pid = 0;

/* Creates a new process and adds it to scheduler */
uint64_t add_process(void * entryPoint, char * name, level context, uint64_t argc, char ** argv, int inAlias, int outAlias) {
    process data = create_process(entryPoint, name, context, argc, argv, inAlias, outAlias);
   
    /* Add process to scheduler */
    add(data);
    if (context == FORE && data.pid > 1) block(NONE, 0);
    return data.pid;
}


/* Creates a new process */
process create_process(void * entryPoint, char * name, level context, uint64_t argc, char ** argv, int inAlias, int outAlias) {
    void * processStack = malloc(STACK_SIZE);
    if (processStack == 0) {}

    /* Creates stack for process */
    char * lastAddress = (char *) get_last_address(processStack);
    char * stackBase = lastAddress - sizeof(uint64_t);
    stackFrame stack = init_stack(entryPoint, (void *) stackBase);
    /* Pass on arguments to start main */
    stack.rdi = argc;
    stack.rsi = (uint64_t) argv;
    /* Copies stack frame to memory */
    memcpy(lastAddress - sizeof(stackFrame), &stack, sizeof(stackFrame));
    
    /* Creates process data */
    process data;
    data.name = (char *) malloc(stringlen(name) + 1);
    stringcp(data.name, name);
    data.pid = c_pid++;
    data.ppid = get_pid();
    data.sp = (uint64_t) lastAddress - sizeof(stackFrame);
    data.bp = (uint64_t) stackBase;
    data.priority = 3;
    data.context = context;
    data.state = READY;
    data.stack = processStack;
    data.res = NONE;
    data.mutex = 0;
    data.firstFd = add_process_alias(0, inAlias);
    if (data.firstFd != 0) data.firstFd->next = add_process_alias(1, outAlias);

    return data;
}

/* Frees all the resources used by the process */
static void free_resources(process p) {
    remove_fdlist(p);
    switch (p.res) {
        case SEM: deallocate_mutex(p.mutex, p.pid);
        case TIME: remove_node_T(p.pid);
        default: free_parent(p);       
    }
}

/* Deletes process */
void remove(process p) {
    free_resources(p);
    free(p.stack);
    free(p.name);
}

/* If a valid process, kill (used when ctrl + C) */
void sig_int() {
    uint64_t pid = get_pid();
    if (pid > 1 && !stringcmp(get_current()->process.name, "PHYLO")) {
        print("\n\t Killed process %d\n", pid);
        kill_current();
    }
}

/* Print process stack */
void print_process_stack(process p) {
    char * lastAddress = (char *) get_last_address(p.stack);
    print("\nProcess %d \n", p.pid);
    uint64_t * aux;
    for (aux = (uint64_t *) p.sp; aux < (uint64_t *) lastAddress; aux++) {
        print("- Address: 0x");
        printHex((uint64_t)aux);
        print(" - Content: 0x");
        printHex(*aux);
        print("\n");
    }
    print("\n-----------------------\n");
}

/* Set parent to ready if process is running on foreground */
static void free_parent(process p) {
    if (p.ppid >= 1 && p.context == FORE)
        set_state(p.ppid, READY);
}

/* Add new filedescriptor to process list */
fdNode * add_process_fd(int fd) {
    /* Create aux process from current to modify fd List */
    nodeScheduler * current = get_current();
    if (current == 0) return 0;
    process p = current->process;

    /* Check if fd already exists in process list */
    fdNode * iterator = p.firstFd;
    fdNode * prev = iterator;
    while (iterator != 0) {
        if (iterator->fd == fd)
            return 0;
        prev = iterator;
        iterator = iterator->next;
    }

    /* Node doesn't exist, create node */
    fdNode * node = (fdNode *) malloc(sizeof(fdNode));
    if (node == 0) return 0; // No more memory
    node->fd = fd;
    node->alias = fd;
    node->next = 0;

    /* Insert node in process list */
    if (p.firstFd == 0)
        p.firstFd = node;
    else
        prev->next = node;
    
    /* Update node from scheduler */
    current->process = p;

    return node;
}

/* Remove a filedescriptor from process list. Returns 1 if removed, 0 if not */
int remove_process_fd(int fd) {
    int removed = 0;

    /* Create aux process from current to modify fd List */
    nodeScheduler * current = get_current();
    if (current == 0) return 0;
    process p = current->process;
    if (p.firstFd == 0) return 0;

    /* Check if fd already exists in process list */
    fdNode * iterator = p.firstFd;
    fdNode * prev = iterator;
    while (iterator != 0) {
        if (iterator->fd == fd) {
            if (iterator == p.firstFd) {
                /* First node */
                p.firstFd = p.firstFd->next;
                free(iterator);
            } else {
                /* Intermediate node */
                prev->next = iterator->next;
                free(iterator);
            }
            removed = 1;
            break;
        }
        prev = iterator;
        iterator = iterator->next;
    }

    /* Update node from scheduler */
    get_current()->process = p;

    return removed;
}

/* Return realFd (Alias) fro the current process or -1 if not listed */
int get_process_alias(int fd) {
    nodeScheduler * current = get_current();
    if (current == 0) return -1;
    process p = current->process;

    /* Search for fd in process list */
    fdNode * iterator = p.firstFd;
    while (iterator != 0) {
        if (iterator->fd == fd)
            return iterator->alias;
        iterator = iterator->next;
    }
    return -1;
}

/* Add filedescriptor to process list with Alias when create */
static fdNode * add_process_alias(int oldFd, int newFd) {
    /* Create fdNode */
    fdNode * node = (fdNode *) malloc(sizeof(fdNode));
    if (node == 0) return 0;

    /* Set node properties */
    node->fd = oldFd;
    node->alias = newFd;
    node->next = 0;

    return node;
}

static void remove_fdlist(process p) {
    fdNode * iterator = p.firstFd;
    while (iterator != 0){
        fdNode * prev = iterator;
        iterator = iterator->next;
        free(prev);
    }  
}