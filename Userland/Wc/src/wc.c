// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
int main() {
	int car;
	int lines = 0;
	putchar('\n');
	while ((car = getchar()) >= 0) {
		putchar(car);
		if (car == '\n')
			lines++;
	}
	printf("\t-- Cantidad de Lineas ingresadas: %d --\n", lines);
	return 0;
} 