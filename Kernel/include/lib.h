#ifndef _LIB_H_
#define _LIB_H_

#include <stdint.h>

/* Sets on memory the character */
void * memset(void * destination, int32_t character, uint64_t length);

/* Copies on destination, length characters from source */
void * memcpy(void * destination, const void * source, uint64_t length);

/* Gets cpu vendor */
char * cpu_vendor(char *result);

/* Reads port (hardware) */
char read_port(char port);

/* Writes port (hardware) */
void write_port(char port, char reg);

/* Makes an atomic swap using xchg */
void atom_swap(uint64_t * dir, uint64_t value);

/* Aquire a mutex using xchg. Sets 1 and returns prev value */
uint64_t mutex_acquire(uint64_t * dir);

/* Release a mutex using xchg. Sets 0 and returns prev value */
uint64_t mutex_release(uint64_t * dir);

/* Forces timer tick to interrupt */
uint64_t force_timer_tick();

/* Gets pow of a given number and base */
uint64_t pow(int base, int exponent);

/* Converts a number to base 2, returns exponent */
uint64_t exp2(uint64_t number);

/* Returns max given two values */
uint64_t max(uint64_t n1, uint64_t n2);

#endif /* _LIB_H_ */