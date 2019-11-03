// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManager.h>
#include <console.h>
#include <mutex.h>
#include <process.h>
#include <strings.h>

#include <fileDescriptors.h>

/* Search a name or FileDescriptor on the list */
static nodeFd * search_name(char * name);
static nodeFd * search_fd(int fd);
/* Add new FileDescriptor node to the list */
static nodeFd * add_fd_list(char* name);

/* List pointers */
static nodeFd * first = 0;
static nodeFd * last = 0;

/* Add STDIN, STDOUT and STDERR fd nodes to fd list */
void init_fds() {
    add_fd_list("stdin");
    add_fd_list("stdout");
    add_fd_list("stderr");
}

// TODO: Check if mutCant has to be a COUNT SEMAPHORE IF MORE THAN ONE EOF IS SENT or sth like that. TE JODES??
/* Write on buffer given fd number */
void write(int fd, const char * buffer, int count) {
    /* If no characters were sent, return */
    if (count == 0) return;

    /* Get realFd (Alias) from current process */
    int aliasFd = get_process_alias(fd);
    if (aliasFd < 0) return; // FD not listed
    
    /* Check if node with fd exists */
    nodeFd * node = search_fd(aliasFd);
    if (node == 0) return;

    /* Wait for the write end to be free */
    wait_mutex(node->data.mutWrite);

    int eof = 0, i = 0;

    switch (node->data.fd)
    {
    case STDOUT:
        /* Print count characters in console */
        print_N(buffer, count);
        break;
    case STDERR:
        /* Print count characters in console Error */
        printError_N(buffer, count);
        break;
    default:
        /* Write COUNT characters in buffer (or until EOF if sent) */
        for (i = node->data.write_index; i < (node->data.write_index + count) && eof == 0; i++) {
            node->data.buffer[i % BUFFER_SIZE] = *buffer;
            if (*buffer == EOF) 
                eof = 1;
            buffer++;
        }
        node->data.write_index = i;

        /* If count reached or EOF, let a process read */
        int available = node->data.write_index - node->data.read_index;
        if (eof == 1 || (node->data.count > 0 && node->data.count <= available))
            post_mutex(node->data.mutCant);
        break;
    }
    /* Let another process write */
    post_mutex(node->data.mutWrite);
}

/* Read from buffer given fd number */
void read(int fd, char * buffer, int count) {
    /* If no characters were sent, return */
    if (count == 0) return;

    /* Get realFd (Alias) from current process */
    int aliasFd = get_process_alias(fd);
    if (aliasFd < 0) return; // FD not listed
    
    /* Check if node with fd exists */
    nodeFd * node = search_fd(aliasFd);
    if (node == 0) return;

    /* Wait for the read end to be free */
    wait_mutex(node->data.mutRead);

    /* Check if characters available are enough */
    if (count > (node->data.write_index - node->data.read_index)) {
        node->data.count = count;
        wait_mutex(node->data.mutCant);
    }

    int i = 0, eof = 0;
    /* Read COUNT characters from buffer (or until EOF if read) */
    for (i = node->data.read_index; i < (node->data.read_index + count) && eof == 0; i++) {
        *buffer = node->data.buffer[i % BUFFER_SIZE];
        if (*buffer == EOF)
            eof = 1;
        buffer++;
    }
    node->data.read_index = i;

    /* Ocasionally reset indexes */
    if (node->data.read_index == node->data.write_index) {
        node->data.read_index = 0;
        node->data.write_index = 0;
    }
    /* Reset count index */
    node->data.count = 0;

    /* Let another process write */
    post_mutex(node->data.mutRead);
}

/* PIPES MANAGEMENT */

/* Create new named Pipe */
int new_pipe(char * name) {
    /* Check if fileDescriptor already exists */
    nodeFd * prevFd = search_name(name);
    if (prevFd != 0) return prevFd->data.fd;  // TODO: PROBAR QUE PASA SI YA EXISTE

    /* Add node to fileDescriptor list */
    nodeFd * node = add_fd_list(name);
    if (node == 0) return -1;

    /* Flag the fileDescriptor as a pipe */
    node->data.pipe = 2;

    /* Add the node to process fileDescriptor list */
    add_process_fd(node->data.fd);

    return node->data.fd;
}

/* Opens an existing Pipe */
int open_pipe(char * name) {
    /* Check if pipe exists */
    nodeFd * node = search_name(name);
    if (node == 0 || node->data.pipe == 0) return -1;

    node->data.pipe++;

    /* Add the node to process fileDescriptor list */
    add_process_fd(node->data.fd);

    return node->data.fd;
}

/* Closes an existing Pipe */
void close_pipe(int fd) {
    /* Check if pipe exists */
    nodeFd * node = search_fd(fd);
    if (node == 0 || node->data.pipe == 0) return;

    if (node->data.pipe > 1) node->data.pipe--;

    /* Check if someone else is waiting to write */
    mutNode * mut = node->data.mutWrite;
    if (mut != 0 && mut->mutex.lock == 0 && mut->mutex.blocked == 0) {
        /* Write EOF signal in pipe */
        char eof = EOF;
        write(fd, &eof, 1);
    }
    
    /* Remove the node from process fileDescriptor list */
    remove_process_fd(fd);

}

/* Prints all Pipes */
void show_all_pipes() {
    int printed = 0;
    nodeFd * iterator = first;
    print("\nName\t\t\tState\t\tBlocked Processes\n");
    while (iterator != 0) {
        if (iterator->data.pipe > 0) {
            print(iterator->data.name); print("\t\t");
            print((iterator->data.pipe > 1) ? "Open" : "Closed"); print("\t\t");
            print_blocked_processes(iterator->data.mutCant);
            print_blocked_processes(iterator->data.mutRead);
            print_blocked_processes(iterator->data.mutWrite);
            print("\n");
            printed = 1;
        }
        iterator = iterator->next;
    }

    if (!printed) 
        print("\nThere are no Pipes created");
}

/* Search a name on the list */
static nodeFd * search_name(char * name) {
    nodeFd * iterator = first;
    while (iterator != 0) {
        if (stringcmp(iterator->data.name, name))
            return iterator;
        iterator = iterator->next;
    }
    return 0;
}

/* Search a fileDescriptor on the list */
static nodeFd * search_fd(int fd) {
    nodeFd * iterator = first;
    while (iterator != 0) {
        if (iterator->data.fd == fd)
            return iterator;
        iterator = iterator->next;
    }
    return 0;
}

/* Add new FileDescriptor node to the list */
static nodeFd * add_fd_list(char* name) {
    /* Create a new fileDescriptor structure */
    nodeFd * newFd = (nodeFd *) malloc(sizeof(nodeFd));
    if (newFd == 0) return 0; // No more Memory
    newFd->data.name = (char *) malloc(stringlen(name) + 1);
    if (newFd->data.name == 0) { // No more Memory
        free(newFd);
        return 0;
    }
    stringcp(newFd->data.name, name);

    /* Add the node to the list, with the next fd number */
    newFd->next = 0;
    if (first == 0) {
        newFd->data.fd = 0;
        first = newFd;
        last = newFd;
    } else {
        newFd->data.fd = last->data.fd + 1;
        last->next = newFd;
        last = newFd;
    }
    /* Pipes are flagged outside */
    newFd->data.pipe = 0;

    /* Initialize indexes */
    newFd->data.count = 0;
    newFd->data.read_index = 0;
    newFd->data.write_index = 0;

    /* Create new mutexes for syncronization */
    newFd->data.mutRead = new_mutex(name, 0); // Reading starts unblocked
    
    char mutName[stringlen(name) + 3], numAux[]="2", writeAux[]="W";
    stringcp(mutName, name);
    stringcat(mutName, numAux);
    newFd->data.mutCant = new_mutex(mutName, 1); // ReadCant starts blocked

    stringcp(mutName, name);
    stringcat(mutName, writeAux);
    newFd->data.mutWrite = new_mutex(mutName, 0); // Writing starts unblocked

    return newFd;
}