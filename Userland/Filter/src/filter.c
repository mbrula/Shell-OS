// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>

static int esVocal(int car);

int main() {
	int car;
	putchar('\n');
	while ((car = getchar()) >= 0) {
		if (!esVocal(car))
			putchar(car);
	}
	return 0;
}

static int esVocal(int car) {
	if (car == 'a' || car == 'A' || car == 'e' || car == 'E' || car == 'i' || 
	car == 'I' || car == 'o' || car == 'O' || car == 'u' || car == 'U') {
		return 1;
	}
	return 0;
}