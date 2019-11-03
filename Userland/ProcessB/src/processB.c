// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>

#define M   1000

int main(int argc, char const * argv[]) {
	if (argc < 1) return 1;
	uint64_t * countParaPrueba = (uint64_t *) atoi(argv[0]);

	int pid = getPid();
	printf("\n--PROCESO HIJO # %d--\n", pid);

	uint64_t mutex = semOpen("mutexSync");
	if (mutex == 0) return 1;

	semWait(mutex);
	int i = 0, j = 0;
	for (i = 0; i < M; i++){
		uint64_t aux = *countParaPrueba;
		while (j < 10000) j++;
		*countParaPrueba = aux + 1;
		j = 0;
	}
	semPost(mutex);
	semClose(mutex);
	printf("\nCOUNT: %d para PID %d", *countParaPrueba, pid);

	return 0;
}

// int main(int argc, char const *argv[]) {
//     while (1) {
// 		puts("\n----------- pB -----------\n");
// 		// ps();
// 		sleep(500);
// 	}	
// 	return 0;
// } 