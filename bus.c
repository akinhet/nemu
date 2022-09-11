#include "bus.h"


void buswrite(uint16_t addr, uint8_t data)
{
	for (int i = 0; i < sizeof(devlist)/sizeof(devlist[0]); i++) {
		if (devlist[i].startaddr <= addr && addr <= devlist[i].endaddr) {
			devlist[i].write(addr, data);
			return;
		}
	}
}


uint8_t busread(uint16_t addr, _Bool readonly)
{
	for (int i = 0; i < sizeof(devlist)/sizeof(devlist[0]); i++) {
		if (devlist[i].startaddr <= addr && addr <= devlist[i].endaddr) {
			return devlist[i].read(addr);
		}
	}

	return 0x00;
}
