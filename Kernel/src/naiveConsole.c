// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>

#include <naiveConsole.h>

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;
static uint8_t * const limit = (uint8_t*)(0xB8000 + 80*2*25);

static uint8_t format = STDOUT_FORMAT;

/* Prints in Error fd */
void ncPrintError(const char * string) {
	format = STDERR_FORMAT;
	ncPrint(string);
	format = STDOUT_FORMAT;
}

/* Prints string on stdout */
void ncPrint(const char * string) {
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

/* Shifts up the lines */
static void shiftUp() {
	for (uint64_t i = 0; i < (height - 1) * width * 2; i++)
		video[i] = video[width * 2 + i];
	for (uint64_t i = (height - 1) * width; i < height * width; i++)
		video[i * 2] = 0;
	currentVideo = video + (height - 1) * width * 2;
}

/* Prints character on stdout */
void ncPrintChar(char character) {
	if (currentVideo >= limit)
		shiftUp();
	if (character == '\n')
		ncNewline();
	else if (character == '\t')
		ncTab();
	else if (character == '\b') {
		currentVideo -= 2;
		*currentVideo = ' ';
	} else {
		*currentVideo = character;
		currentVideo++;
		*currentVideo = format;
		currentVideo++;
	}
}

/* Draws a new line */
void ncNewline() {
	do {
		ncPrintChar(' ');
	} while((uint64_t)(currentVideo - video) % (width * 2) != 0);
	if(currentVideo>limit) currentVideo = video;
}

/* Makes a tab */
void ncTab() {
	for (uint64_t i = 0; i < TAB_SIZE; i ++)
		ncPrintChar(' ');
}

/* Prints the value in decimal */
void ncPrintDec(uint64_t value) {
	ncPrintBase(value, 10);
}

/* Prints a value in hexa */
void ncPrintHex(uint64_t value) {
	ncPrintBase(value, 16);
}

/* Prints a value in binary */
void ncPrintBin(uint64_t value) {
	ncPrintBase(value, 2);
}

/* Prints a value in any base */
void ncPrintBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

/* Clears screen */
void ncClear() {
	for (uint64_t i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

/* Prints string on address */
void ncPrintOnAddress(char * address, char * text) {
    uint64_t i = 0;
    while(*text != 0) {
        *(address+i) = *text;
        *(address+i+1) = 0x02;

        i+=2;
        text++;
    }
}

/* Converts a decimal number to a given base */
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
	return digits;
}


