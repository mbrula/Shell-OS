#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char * cpu_vendor(char *result);

char read_port(char port);
void write_port(char port, char reg);

#endif