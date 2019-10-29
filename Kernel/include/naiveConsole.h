#ifndef _NAIVE_CONSOLE_H_
#define _NAIVE_CONSOLE_H_

#include <stdint.h>

#define TAB_SIZE        6
#define STDOUT_FORMAT   2
#define STDERR_FORMAT   4

#define STDIN   0
#define STDOUT  1
#define STDERR  2

/* Prints string on stdout */
void ncPrint(const char * string);

/* Prints character on stdout */
void ncPrintChar(char character);

/* Draws a new line */
void ncNewline();

/* Makes a tab */
void ncTab();

/* Prints the value in decimal */
void ncPrintDec(uint64_t value);

/* Prints a value in hexa */
void ncPrintHex(uint64_t value);

/* Prints a value in binary */
void ncPrintBin(uint64_t value);

/* Prints a value in any base */
void ncPrintBase(uint64_t value, uint32_t base);

/* Clears screen */
void ncClear();

/* Prints string on address */
void ncPrintOnAddress(char * address, char * text);

/* Prints in Error fd */
void ncPrintError(const char * string);

#endif /* _NAIVE_CONSOLE_H_ */