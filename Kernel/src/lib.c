#include <stdint.h>

/* Sets on memory the character */
void * memset(void * destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

/* Copies on destination, length characters from source */
void * memcpy(void * destination, const void * source, uint64_t length) {
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 && (uint64_t)source % sizeof(uint32_t) == 0 && length % sizeof(uint32_t) == 0) {
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	} else {
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

/* Gets pow */
uint64_t pow(int base, int exponent) {
	int result = 1;
	for (uint64_t i = 0; i < exponent; i++){
		result = result * base;
	}
	return result;
}

/* Converts a number to base 2, returns exponent */
uint64_t exp2(uint64_t number) {
	uint64_t exp = 1;
	float aux = (float)number;
	while (aux > 2) {
		aux /= 2;
		exp++;
	}
	return exp;
}

/* Returns max given two values */
uint64_t max(uint64_t n1, uint64_t n2) {
	if (n1 >= n2) return n1;
	return n2;
}
