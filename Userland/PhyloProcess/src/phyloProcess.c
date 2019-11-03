// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <phyloProcess.h>

char my_sem[20];
uint64_t phnumber;
uint64_t * states[];

int main(int argc, char const * argv[]) {
	if(argc < 1) return 1;

	phnumber = (uint64_t) atoi(argv[0]);
	* states = (uint64_t *) atoi(argv[1]);

	char num[5];
	strcpy(my_sem, PH_NAME);
	itoa(getPid(), num, 10);
	strcat(my_sem, num);

	while(1){
		sleep(2000);
		take_fork(phnumber); 	/* adquiere dos tenedores o se bloquea */
		sleep(3000); 			/* come */
		put_fork(phnumber); 	/* pone de vuelta ambos tenedores en la mesa */
	}

	return 0;
} 

void take_fork(int i) {
	semWait(getPid()); 			/* entra a la región crítica */
	states[i] = WAIT;			/* registra el hecho de que el filósofo i está hambriento */
	try(i); 					/* trata de adquirir 2 tenedores */
	semPost(getPid()); 			/* sale de la región crítica */
	semWait(my_sem); 			/* se bloquea si no se adquirieron los tenedores */
}

void put_fork(int i) {
	semWait(getPid()); 			/* entra a la región crítica */
	states[i] = NONE; 			/* el filósofo terminó de comer */
	try(i+1); 					/* verifica si el vecino izquierdo puede comer ahora */
	try(i-1); 					/* verifica si el vecino derecho puede comer ahora */
	semPost(getPid()); 			/* sale de la región crítica */
}

void try(int i) {
	if (states[i] == WAIT && states[i-1] != EAT && states[i+1] != EAT) {
		states[i] = EAT;
		semPost(my_sem);
	}
}