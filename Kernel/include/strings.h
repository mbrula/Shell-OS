#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <stdint.h>

/* Gets lenght of string */
uint64_t stringlen(char * str);

/* Copies one string onto a destination */
void strngcp(char * destination, char * origin);

/* Compares two strings */
uint8_t stringcmp(char * str1, char * str2);

/* Copies origin to the end of destination */
char * stringcat (char * destination, char * origin);

/* Converts an number on base to string, stored on buff */
uint32_t int_to_string(uint64_t value, char * buffer, uint32_t base);

#endif /* _STRINGS_H_ */