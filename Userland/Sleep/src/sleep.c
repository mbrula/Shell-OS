// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
int main(int argc, char const *argv[] ) {
	putchar(argc);
	putchar('\n');
	int millis = 5000;
    sleep(millis);
	puts("\nExiting");
	return 0;
} 
