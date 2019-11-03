/***************************************************
                    PhyloProcess.h
****************************************************/
#include <clib.h>
#include <phylo.h>

/* Try to take 2 forks or block */
void take_fork(int i);

/* Finish eating, check if another phylo can eat now */
void put_fork(int i);

/* Check both sides looking for free forks */
void try(int i);