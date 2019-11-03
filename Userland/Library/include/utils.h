#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

int concat(char * to, const char * from);
void swap(char *x, char *y);
char* reverse(char *buffer, int i, int j);
int atoi(const char* buffer);
int pow(int base, int exponent);
char* itoa(int value, char* buffer, int base);
uint64_t strlen(const char * string);
int strcmp(const char * string1, const char * string2);
int strcpy (char *dest, const char *src);
char* strcat (char *t, const char *s);
char* saveTwoDigit(int number, char * string);
int isNumber(char character);
uint64_t getRGB(uint8_t r, uint8_t g, uint8_t b);

int strcmpUntilSpace (const char *t, const char *s, int * count);
int strcpyUntilSpace (char *dest, const char *src);

#endif /* UTILS_H_ */