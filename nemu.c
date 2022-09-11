#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[])
{
	while (1) {
		cputick();
		getc(stdin);
	}

	return 0;
}
