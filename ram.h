#ifndef RAM_H_
#define RAM_H_

#include <stdint.h>

uint8_t ram[64 * 1024];

uint8_t ramread(uint16_t addr);
void ramwrite(uint16_t addr, uint8_t data);

#endif // RAM_H_
