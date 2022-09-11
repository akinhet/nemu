#ifndef CPU_H_
#define CPU_H_

#include <stdint.h>

#include "bus.h"


static struct {
	uint8_t a;         // accumulator
	uint8_t x;         // x register
	uint8_t y;         // y register
	uint8_t stkp;      // stack pointer
	uint16_t pc;       // program counter
	uint8_t status;    // status register
} cpu;

enum FLAGS6502 {
	C = (1 << 0),    // carry bit
	Z = (1 << 1),    // zero
	I = (1 << 2),    // disable interrupts
	D = (1 << 3),    // decimal mode
	B = (1 << 4),    // break
	U = (1 << 5),    // unused
	V = (1 << 6),    // overflow
	N = (1 << 7),    // negative
};


void cpureset();    // reset the cpu to a known state
void cpuirq();      // perform an interrupt
void cpunmi();      // perform a nonmaskable interrupt
void cputick();     // perform one clock cycle

#endif // CPU_H_
