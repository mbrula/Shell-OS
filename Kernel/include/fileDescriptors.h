#ifndef _FILEDESC_H_
#define _FILEDESC_H_

#include <mutex.h>

#define BUFFER_SIZE 255
#define EOF         -1
#define SIGINT      -2

#define STDIN       0
#define STDOUT      1
#define STDERR      2

/* Structure that defines a fileDescriptor */
typedef struct {
    /* Properties */
    char * name;
    int fd;
    int pipe;
    // char buffer[BUFFER_SIZE];
    char * buffer;

    /* Index variables */
    int count;
    int read_index;
    int write_index;
    
    /* Mutexes */
    mutNode * mutRead;
    mutNode * mutCant;
    mutNode * mutWrite;
} fileDescriptor;

/* Structure for fileDescriptor nodes */
typedef struct node_fd {
    /* The fileDescriptor data */
    fileDescriptor data;

    /* Next node */
    struct node_fd * next;
} nodeFd;

/* Add STDIN, STDOUT and STDERR fd nodes to fd list */
void init_fds();

/* Write on buffer given fd number */
void write(int fd, const char * buffer, int count);

/* Read from buffer given fd number */
void read(int fd, char * buffer, int count);

/* PIPES MANAGEMENT */

/* Create new named Pipe */
int new_pipe(char * name);

/* Opens an existing Pipe */
int open_pipe(char * name);

/* Closes an existing Pipe */
void close_pipe(int fd);

/* Prints all Pipes */
void show_all_pipes();

#endif /* _FILEDESC_H_ */