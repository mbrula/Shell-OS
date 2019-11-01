// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>

int main(int argc, char const *argv[] ) {
	int millis = 1000;
	if (argc == 1) millis = atoi(argv[1]);
    sleep(millis);
	puts("\nExiting\n");
	return 0;
} 
