#include "ram.h"


uint8_t ramread(uint16_t addr)
{
	return ram[addr];
}


void ramwrite(uint16_t addr, uint8_t data)
{
	ram[addr] = data;
}
