// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
int main() {
	char input[MAX_LENGTH] = {0};
	int ret = 0;
	putchar('\n');
	while (ret >= 0) {
		ret = gets(input, MAX_LENGTH);
		putchar('\n');
		if (ret >= 0)
			puts(input);
		putchar('\n');
	}
	return 0;
} 