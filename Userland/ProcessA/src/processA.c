// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>

#define N 	10

uint64_t countParaPrueba = 0;

int main(int argc, char const * argv[]) {
	uint64_t mutex = newSem("mutexSync", 1);
	if (mutex == 0) return 1;

	puts("\n--PROCESO PADRE--\n");

	sleep(1000);

	char arg1[100]; // TODO: No es char[10]??
	itoa((uint64_t) &countParaPrueba, arg1, 10);
	char * argv2[] = {arg1, 0, 0};
    int argc2 = 1;

	int i = 0;
	for (i = 0; i < N; i++) {
		newProcess("PROCESS_B", argc2, argv2, BACKGROUND, 0, 1);
	}

	ps();

	puts("\n--PADRE ESPERANDO--\n");
	sleep(3000);
	printf("\nCuenta Final: %d\n", countParaPrueba);
	semClose(mutex);
	
	return 0;
}