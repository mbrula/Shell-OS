// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
int main() {
	int pid = getPid();
	while (1) {
		printf("\n\n-- Hola! Soy Loop. Mi PID es %d --\n", pid);
		sleep(300);
	}
	return 0;
} 