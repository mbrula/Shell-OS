// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <phyloView.h>

int main(int argc, char const * argv[]) {
	if(argc < 1) return 1;

	uint64_t mutex = semOpen("buffer");
	char name[5];
	uint64_t * ph_qty = (uint64_t *) argv[0];
	uint64_t * states[] = (uint64_t **) argv[1];

	while(1){
		sleep(5000);
		for(int i = 0; i < *ph_qty ; i++) {
			puts("Phylo number ");
			putchar(i);
			/*puts(": \tPid: ");
			itoa(pids[i], name, 10);*/
			puts(name);
			semWait(mutex);
			puts(*states[i]==0?" is WORKING":(*states[i]==1?" is WAITING":" is EATING"));
			semPost(mutex);
		}		
	}
	return 0;
} 