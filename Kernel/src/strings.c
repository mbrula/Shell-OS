// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>

#include <strings.h>

/* Gets string length */
uint64_t stringlen(char * str) {
    uint64_t ret = 0;
    while(*(str+ret) != 0) ret++;
    return ret;
}

/* Copies one string onto a destination */
void stringcp(char * destination, char * origin) {
    while (*origin != 0) {
		*destination = *origin;
		destination++;
		origin++;
	};
	*destination = 0;
}

/* Copies one string onto a destination until space or \0 */
void stringcp_until_space (char *destination, const char *origin) {
	while (*origin != 0 && *origin != ' ') {
		*destination = *origin;
		destination++;
		origin++;
	};
	*destination = 0;
}

/* Compares two strings */
uint8_t stringcmp(char * str1, char * str2) {
	while (*str1 && *str2) {
		if (*str1 != *str2) return 0;
		str1++;
		str2++;
	}
	if (*str1 == *str2) return 1;
	return 0;
}

/* Copies origin to the end of destination */
char * stringcat (char * destination, char * origin) {
	char * aux=destination;
	while (*destination)
		destination++;
	stringcp(destination, origin);
	return aux;
}

/* Converts an number on base to string, stored on buff */
uint32_t int_to_string(uint64_t value, char * buffer, uint32_t base) {
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    /* End of string */
    *p = 0;

    /* Reverse the string */
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

/* Returns 1 if character is a number or 0 if not */
static int is_number(char character){
	if (character >= '0' && character <= '9')
		return 1;
	return 0;
}

/* Convert a string into a number and save cursor */
int atoi(char* buffer, int * cursor) {
	int i = 0;
	int result = 0;
	int len = stringlen(buffer);

	while(buffer[i] != 0 && is_number(buffer[i])){
		result += (pow(10, --len) * (buffer[i] - '0'));
		i++;
	}
    *cursor = i;
	if (buffer[i] == 0 && i != 0)
		return result;
	return -1;
}