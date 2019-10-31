#ifndef _MEM_MANAGER_H_
#define _MEM_MANAGER_H_

#include <stdint.h>

/* Memory Manager builder */
void create_manager(uint8_t * address, uint64_t totalBytes);

/* Memory Manager user functions */

/* Reserves bytes space on memory */
void * malloc(uint64_t bytes);

/* Frees space on memory */
void free(void * pmmtr);

/* Gets memory status */
void mm_status(uint64_t * total, uint64_t * occupied, uint64_t * free);

/* Prints memory status */
void mm_print_status();

/* Returns the first address from the next block, 
** assuming ptr is a valid return from malloc */
void * get_last_address(void * ptr);

/* Prints memory state (for testing purposes only) */
void mm_print_state();

#endif /* _MEM_MANAGER_H_ */