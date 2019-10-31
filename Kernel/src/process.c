// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <memoryManager.h>
#include <console.h>
#include <strings.h>
#include <stack.h>

#include <process.h>

/* Static count of the pids given */
static uint64_t c_pid = 0;

/* Creates a new process and adds it to scheduler */
uint64_t add_process(void * entryPoint, char * name, level context, uint64_t argc, char ** argv, int inAlias, int outAlias) {
    process data = create_process(entryPoint, name, context, argc, argv, inAlias, outAlias);
    
    /* Add process to scheduler */
    // add(data); TODO add to scheduler
    // if (context == FORE && data.pid > 1)
    //     block(0);
    //     // block(SCREEN);
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
    // data.ppid = getPid(); TODO when scheduler
    data.sp = (uint64_t) lastAddress - sizeof(stackFrame);
    data.bp = (uint64_t) stackBase;
    data.priority = 3;
    data.context = context;
    data.state = READY;
    data.stack = processStack;

    return data;
}

/* Frees all the resources used by the process */
static void free_resources(process p) {
    // fdPointer * aux = p.first; TODO free also fdlist
    // while (aux != 0){
    //     fdPointer * aux2 = aux;
    //     aux = aux->next;
    //     free(aux2);
    // }
    // if (p.state != BLOCKED) return;
    // if (p.sem == 0) removeNodeT(p.pid);
    // else deallocateSem(p.sem, p.pid);    
}

/* Deletes process */
void remove(process p) {
    free_resources(p);
    free(p.stack);
    free(p.name);
}

/* If a valid process, kill (used when ctrl + C) */
void sig_int() {
    // if (getPid() > 1) TODO when scheduler
    //     killCurrent();
}

/* Print process stack */
void print_process_stacak(process p) {
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