// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>
#include <phylo.h>

/* Try to take 2 forks or block */
void take_fork(uint8_t i);
/* Finish eating, check if another phylo can eat now */
void put_fork(uint8_t i);
/* Check both sides looking for free forks */
void try_fork(uint8_t i);

uint8_t ** states;
uint64_t mutex;
uint64_t phyloSem;
uint8_t * totalNumber;

int main(int argc, char const * argv[]) {
	if(argc < 3) return 1;
	uint64_t aux = atoi(argv[0]);
	states = (uint8_t **) aux;
	uint8_t phNumber = atoi(argv[1]);
	aux = atoi(argv[2]);
	totalNumber = (uint8_t *) aux;

	mutex = semOpen(PH_MUTEX);
	if (mutex == 0) return 1;

	printf("\n--PHYLO NUMBER %d", phNumber);

	char name[20], num[5];
	strcpy(name, PH_NAME);
	itoa(phNumber, num, 10);
	strcat(name, num);
	phyloSem = semOpen(name);
	if (phyloSem == 0) return 1;

	while(1){
		sleep(THINK_TIME);
		take_fork(phNumber); 	/* adquiere dos tenedores o se bloquea */
		sleep(EAT_TIME); 			/* come */
		put_fork(phNumber); 	/* pone de vuelta ambos tenedores en la mesa */
	}

	semClose(phyloSem);
	semClose(mutex);
	return 0;
} 

static uint8_t getLeft(uint8_t i) {
	return (i + *totalNumber - 1) % (*totalNumber);
}

static uint8_t getRight(uint8_t i) {
	return (i + 1) % (*totalNumber);
}

void take_fork(uint8_t i) {
	semWait(mutex); 			/* entra a la región crítica */
	(*states)[i] = WAIT;			/* registra el hecho de que el filósofo i está hambriento */
	try_fork(i); 					/* trata de adquirir 2 tenedores */
	semPost(mutex); 			/* sale de la región crítica */
	semWait(phyloSem); 			/* se bloquea si no se adquirieron los tenedores */
}

void put_fork(uint8_t i) {
	semWait(mutex); 			/* entra a la región crítica */
	(*states)[i] = NONE; 			/* el filósofo terminó de comer */
	try_fork(getLeft(i)); 					/* verifica si el vecino izquierdo puede comer ahora */
	try_fork(getRight(i)); 					/* verifica si el vecino derecho puede comer ahora */
	semPost(mutex); 			/* sale de la región crítica */
}

void try_fork(uint8_t i) {
	if ((*states)[i] == WAIT && (*states)[getLeft(i)] != EAT && (*states)[getRight(i)] != EAT) {
		(*states)[i] = EAT;
		semPost(phyloSem);
	}
}
