/***************************************************
                    Phylo.h
****************************************************/

#include <clib.h>
#include <utils.h>

#define INIT_PH 5
#define INIT_FORKS 5
#define MAX_PHYLO 15
#define SEM_TABLE "table"
#define SEM_PHYLO "phyloSem"
#define PH_NAME "phyloNumber"

typedef enum {NONE = 0, WAIT, EAT} ph_state;

/* Start Phylo process: create philosophers and semaphores */
void initialize();

/* Add new philosopher */
void addPhylo();

/* Remove philosopher */
void removePhylo();

/* Exit game */
void quitPhylo();
