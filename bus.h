#ifndef BUS_H_
#define BUS_H_

#include <stdint.h>

#include "ram.h"

struct devonbus {
	uint16_t startaddr;
	uint16_t endaddr;
	void (*write) (uint16_t, uint8_t);
	uint8_t (*read) (uint16_t);
};

static struct devonbus devlist[] = {
	{ 0x0000, 0xFFFF, ramwrite, ramread },
};


void buswrite(uint16_t addr, uint8_t data);
uint8_t busread(uint16_t addr, _Bool readonly);

#endif // BUS_H_
