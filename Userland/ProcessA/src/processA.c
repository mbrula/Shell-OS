// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>

#define N 	10

uint64_t countParaPrueba = 0;
// extern uint64_t goParaPrueba;

int main(int argc, char const * argv[]) {
	uint64_t sons[N];
	uint64_t mutex = newSem("mutex", 0);
	if (mutex == 0) return 1;

	puts("\n--PROCESO A--\n");

	sleep(1000);

	char arg1[100];
	itoa((uint64_t) &countParaPrueba, arg1, 10);
	char * argv2[] = {arg1, 0, 0};
    int argc2 = 1;

	int i = 0;
	for (i = 0; i < N; i++) {
		sons[i] = newProcess("PROCESS_B", argc2, argv2, BACKGROUND, 0, 1);
	}

	ps();

	sleep(5000);
	printf("\n\n%d\n\n", countParaPrueba);
	semClose(mutex);
	
	return 0;
}

// int main(int argc, char const *argv[]) {
// 	int i = 0;
//     while (1) {
// 		i++;
// 		puts("\n########## pA ##########\n");
// 		// ps();
// 		sleep(1000);
// 		// if (i == 5) {
// 		// 	kill(2);
// 		// }
// 		// if (i == 10) {
// 		// 	kill(1);
// 		// }
// 	}	
// 	return 0;
// } 