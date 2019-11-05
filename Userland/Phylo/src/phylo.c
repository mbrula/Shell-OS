// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>
#include <phylo.h>

/* Start Phylo process: create philosophers and semaphores */
void initialize();
/* Add new philosopher */
void addPhylo();
/* Remove philosopher */
void removePhylo();
/* Exit game */
void quitPhylo();

uint64_t pids[MAX_PHYLO];
uint64_t sem[MAX_PHYLO];
uint8_t ph_qty = 0;
uint8_t states[MAX_PHYLO];
char * args[3][MAX_PHYLO];
char ** argvs[MAX_PHYLO];
uint64_t mutex;

int main(int argc, char const * argv[]) {
	mutex = newSem(PH_MUTEX, 1);
	if (mutex == 0) return 1;

	puts("\n\n-- Programa de Filosofos --\n");
	ph_qty = 0;

	initialize();

	char arg1[10], arg2[10];
	itoa((uint64_t) &states, arg1, 10);
	itoa((uint64_t) &ph_qty, arg2, 10);
	char * argv2[] = {arg1, arg2, 0};
	int argc2 = 2;
	uint64_t view = newProcess("PHYLO_VIEW", argc2, argv2, BACKGROUND, 0, 1);

	char in = 0;
	do {
		in = getchar();
		switch(in){
			case 'a': if ((ph_qty + 1) < MAX_PHYLO) addPhylo(); 	
					  break;
			case 'r': removePhylo(); 
					  break;
			default:  break;
		}
	} while (in != 'q');

	quitPhylo();
	kill(view);
	semClose(mutex);

	return 0;
} 

void initialize(){
	int i = 0;
	for(i = 0; i < INIT_PH; i++){
		addPhylo();
	}
}

void addPhylo(){
	semWait(mutex);
	states[ph_qty] = NONE;

	args[0][ph_qty] = (char *) malloc(ARG_SIZE);
	args[1][ph_qty] = (char *) malloc(ARG_SIZE);
	args[2][ph_qty] = (char *) malloc(ARG_SIZE);
	argvs[ph_qty] = (char **) malloc(4 * sizeof(char *));
	itoa((uint64_t) &states, args[0][ph_qty], 10);
	itoa((uint64_t) &ph_qty, args[2][ph_qty], 10);
	itoa(ph_qty, args[1][ph_qty], 10);
	int i = 0;
	for (i = 0; i < 3; i++)
		argvs[ph_qty][i] = args[i][ph_qty];
	argvs[ph_qty][i] = 0;
	int argc3 = 3;

	char name[ARG_SIZE];
	strcpy(name, PH_NAME);
	strcat(name, args[1][ph_qty]);
	/* Creo un semaforo para el filosofo con nombre PH_NAME + ph_num */
	sem[ph_qty] = newSem(name, 1);
	/* Creo un proceso filosofo nuevo */
	uint64_t pid = newProcess("PHYLO_PROCESS", argc3, argvs[ph_qty], BACKGROUND, 0, 1);
	pids[ph_qty] = pid;
	printf("\n\t\tNew Phylo: %d", ph_qty);
	ph_qty++;
	semPost(mutex);
}

void removePhylo(){
	semWait(mutex);
	ph_qty--;
	kill(pids[ph_qty]);
	semClose(sem[ph_qty]);
	int i = 0;
	for (i = 0; i < 3; i++)
		free(args[i][ph_qty]);
	free(argvs[ph_qty]);
	printf("\n\t\tRemoved Phylo: %d", ph_qty);
	semPost(mutex);
}

void quitPhylo(){
	int i = 0, qty = ph_qty;
	for (i = 0; i < qty; i++) {
		removePhylo();
	}
}