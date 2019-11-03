// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <phylo.h>

uint64_t ph_qty = INIT_PH;
uint64_t pids[];
static uint16_t * states[MAX_PHYLO];

int main() {
	puts("\n\n-- Programa de Filosofos --\n");
	
	initialize();
	uint64_t argc = 2;
	char * argv[] = {&ph_qty , states};
	newProcess("PHYLO_VIEW", argc, argv, BACKGROUND, 0, 1);
	char in = 0;
	while ((in=getchar()) != 'q' || in!='a'|| in!='r') {
		switch(in){
			case 'a': if(ph_qty+1 < MAX_PHYLO) addPhylo(); 	
					  break;
			case 'r': removePhylo(); 
					  break;
			case 'q': quitPhylo(); 
					  break;
			default:  break;
		}
	}
	return 0;
} 

void initialize(){
	char name[20], num[5];
	int pid, argc = 2;
	char * argv[] = {0 , states};
	for(uint64_t p = 0; p < INIT_PH; p++, strcpy(name, PH_NAME)){
		argv[0] = p;
		pid = newProcess("PHYLO_PROCESS", argc, argv, BACKGROUND, 0, 1); // TODO: Ver si hay que pasarle args
		itoa(pid, num, 10);
		strcat(name, num);
		newSem(name, 1);		// Phylo sem -> PH_NAME + pid 
		waitSem(pid);			// Pid sem for 
		pids[p] = pid;
		newSem(num,1);
		post(pid);
	}
	newSem("buffer", 1);
	newSem(SEM_TABLE, 1);
}

void addPhylo(){
	uint64_t argc = 2;
	char * argv[] = {++ph_qty, states};
	char *name = PH_NAME;
	strcat(name,ph_qty);
	pids[ph_qty] = newProcess(name, argc, argv, BACKGROUND, 0, 1);
	strcat(name=PH_NAME, pids[ph_qty]);
	newSem(name,1);
}

void removePhylo(){
	kill(pids[ph_qty]);
	char * name = SEM_PHYLO;
	strcat(name,pids[ph_qty--]);
	closeSem(name);
}

void quitPhylo(){
	exit();
}