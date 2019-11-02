// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <utils.h>

int main(int argc, char const *argv[] ) {
	if (argc < 1) return 1;
	uint64_t seconds = (uint64_t) atoi(argv[0]);
	if (seconds == -1)
		return 1;

    sleep(seconds * 1000);
	return 0;
} 
