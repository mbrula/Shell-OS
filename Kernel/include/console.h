#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdint.h>
#include <stdarg.h>
#include <videoDriver.h>

/* Initializes the console */
void init_console();

/* Deletes a character */
void backspace();

/* Prints a 0 ending string with the chosen color for background and foreground */
void print_with_colors(Color chosenForeground, Color chosenBackground, char * str, va_list list);

/* Print only one character on stdout */
void print_char(char c);

/* Print only one character on stderr */
void printError_char(char c);

/* Prints a string with count length on stdout */
void print_N(const char * src, uint64_t length);

/* Prints a string with count length on stderr */
void printError_N(const char * str, uint64_t length);

/* Prints on stdout a formatted string */
void print(char * str, ...);

/* Prints an integer */
void printInteger(uint64_t dec);

/* Prints on stderr a string like printf */
void printError(char * str, ...);

/* Moves all lines one up */
void move_all_up();

/* Deletes a line */
void clear_line(uint64_t line);

/* Clears screen */
void clear_console();

/* Gets max characters per line */
int get_max_line();

#endif /* _CONSOLE_H_ */
