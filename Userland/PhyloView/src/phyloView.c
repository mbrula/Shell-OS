// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>
#include <phylo.h>

int main(int argc, char const * argv[]) {
	if(argc < 2) return 1;
	uint64_t aux = atoi(argv[0]);
	uint8_t ** states = (uint8_t **) aux;
	aux = atoi(argv[1]);
	uint8_t * totalNumber = (uint8_t *) aux;

	uint64_t mutex = semOpen(PH_MUTEX);
	if (mutex == 0) return 1;

	printf("\n--PHYLO VIEW - TOTAL NUM: %d", *totalNumber);

	while(1){
		sleep(3000);
		semWait(mutex);
		printf("\n\n\t\tTABLE STATE\n");
		for(int i = 0; i < (*totalNumber) ; i++) {
			printf("\nPhylo number %d is %s", i, state_words[(*states)[i]]);
		}		
		semPost(mutex);
	}

	semClose(mutex);
	return 0;
} 