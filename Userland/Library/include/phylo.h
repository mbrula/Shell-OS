#ifndef _PHYLO_H_
#define _PHYLO_H_

#include <stdint.h>

#define INIT_PH     1
#define MAX_PHYLO   15
#define PH_MUTEX    "mutexPhylo"
#define PH_NAME     "phyloNumber"
#define PH_PIPE     "pipePhylo"

#define THINK_TIME  2000
#define EAT_TIME    3000

#define ARG_SIZE    20

typedef enum {NONE = 0, WAIT, EAT} ph_state;
const char * state_words[] = {"WORKING", "WAITING", "EATING"};

#endif /* _PHYLO_H_ */