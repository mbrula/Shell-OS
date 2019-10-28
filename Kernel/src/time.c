#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void sleep(uint64_t millis) {
	//Prepara para que pueda recibir iterrupciones
    _sti();
    int start = ticks_elapsed();
    while (ticks_elapsed() >= start && (ticks_elapsed() - start) * TICK_LENGTH < millis)
        halt();
}