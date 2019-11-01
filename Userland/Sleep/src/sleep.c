// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>

int main(int argc, char const *argv[] ) {
	puts("\nARGV[0] is :");
	puts(argv[0]);
	puts("\nARGV[1] is :");
	puts(argv[1]);
	puts("\nARGC is :");
	putchar('0' + (char)argc);

	int millis = 5000;
	// if (argc == 1) millis = atoi(argv[0]);
    sleep(millis);
	puts("\nExiting\n");
	return 0;
} 
