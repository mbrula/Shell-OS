#ifndef _INTERRUPS_H_
#define _INTERRUPS_H_

#include <idtLoader.h>

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);

void _sirqHandler(void);

void _exception00Handler(void);
void _exception06Handler(void);

/* Clears all interrupts */
void _cli(void);

/* Sets all interrupts */
void _sti(void);

/* Halt processor */
void _hlt(void);
void halt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

/* Finishes CPU ejecution */
void haltcpu(void);

#endif /* _INTERRUPS_H_ */
