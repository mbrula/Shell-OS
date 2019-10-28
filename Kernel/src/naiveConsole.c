#include <naiveConsole.h>

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;
static uint8_t * const limit = (uint8_t*)(0xB8000 + 80*2*25);

static uint8_t format = STDOUT_FORMAT;

void ncPrintError(const char * string) {
	format = STDERR_FORMAT;
	ncPrint(string);
	format = STDOUT_FORMAT;
}

void ncPrint(const char * string)
{
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

static void shiftUp(){
	for (int i = 0; i < (height - 1) * width * 2; i++)
		video[i] = video[width * 2 + i];
	for (int i = (height - 1) * width; i < height * width; i++)
		video[i * 2] = 0;
	currentVideo = video + (height - 1) * width * 2;
}

void ncPrintChar(char character)
{
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

void ncNewline()
{
	do
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
	//Esto es para empezar desde cero cuando te pasas al imprimir
	if(currentVideo>limit) currentVideo = video;
}

void ncTab() {
	for (int i = 0; i < TAB_SIZE; i ++)
		ncPrintChar(' ');
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear()
{
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

void ncPrintOnAddress(char * address, char * text){

    int i = 0;

    while(*text != 0){

        *(address+i) = *text;
        *(address+i+1) = 0x02;

        i+=2;
        text++;
    }
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

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


