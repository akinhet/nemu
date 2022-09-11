#include "cpu.h"


uint8_t fetched      = 0x00;
uint16_t temp        = 0x0000;
uint16_t addr_abs    = 0x0000;
uint16_t addr_rel    = 0x00;
uint8_t  opcode      = 0x00;
uint8_t  cycles      = 0;
uint32_t clock_count = 0;


uint8_t getflag(enum FLAGS6502 f);          // get status flag
void    setflag(enum FLAGS6502 f, _Bool v);    // set status flag

uint8_t fetch();
uint8_t read(uint16_t addr);
void    write(uint16_t addr, uint8_t data);

// addressing modes ==========
uint8_t IMP();	uint8_t IMM();
uint8_t ZP0();	uint8_t ZPX();
uint8_t ZPY();	uint8_t REL();
uint8_t ABS();	uint8_t ABX();
uint8_t ABY();	uint8_t IND();
uint8_t IZX();	uint8_t IZY();

// opcodes ===================================================
uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

uint8_t XXX(); // trap for all unofficial opcodes


struct instruction {
	char *name;
	uint8_t (*operate)(void);
	uint8_t (*addrmode)(void);
	uint8_t cycles;
};

struct instruction lookup[] = {
	{ "BRK", BRK, IMM, 7 },{ "ORA", ORA, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 3 },{ "ORA", ORA, ZP0, 3 },{ "ASL", ASL, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PHP", PHP, IMP, 3 },{ "ORA", ORA, IMM, 2 },{ "ASL", ASL, IMP, 2 },{ "???", XXX, IMP, 2 },{ "???", NOP, IMP, 4 },{ "ORA", ORA, ABS, 4 },{ "ASL", ASL, ABS, 6 },{ "???", XXX, IMP, 6 },
	{ "BPL", BPL, REL, 2 },{ "ORA", ORA, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "ORA", ORA, ZPX, 4 },{ "ASL", ASL, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "CLC", CLC, IMP, 2 },{ "ORA", ORA, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "ORA", ORA, ABX, 4 },{ "ASL", ASL, ABX, 7 },{ "???", XXX, IMP, 7 },
	{ "JSR", JSR, ABS, 6 },{ "AND", AND, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "BIT", BIT, ZP0, 3 },{ "AND", AND, ZP0, 3 },{ "ROL", ROL, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PLP", PLP, IMP, 4 },{ "AND", AND, IMM, 2 },{ "ROL", ROL, IMP, 2 },{ "???", XXX, IMP, 2 },{ "BIT", BIT, ABS, 4 },{ "AND", AND, ABS, 4 },{ "ROL", ROL, ABS, 6 },{ "???", XXX, IMP, 6 },
	{ "BMI", BMI, REL, 2 },{ "AND", AND, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "AND", AND, ZPX, 4 },{ "ROL", ROL, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "SEC", SEC, IMP, 2 },{ "AND", AND, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "AND", AND, ABX, 4 },{ "ROL", ROL, ABX, 7 },{ "???", XXX, IMP, 7 },
	{ "RTI", RTI, IMP, 6 },{ "EOR", EOR, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 3 },{ "EOR", EOR, ZP0, 3 },{ "LSR", LSR, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PHA", PHA, IMP, 3 },{ "EOR", EOR, IMM, 2 },{ "LSR", LSR, IMP, 2 },{ "???", XXX, IMP, 2 },{ "JMP", JMP, ABS, 3 },{ "EOR", EOR, ABS, 4 },{ "LSR", LSR, ABS, 6 },{ "???", XXX, IMP, 6 },
	{ "BVC", BVC, REL, 2 },{ "EOR", EOR, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "EOR", EOR, ZPX, 4 },{ "LSR", LSR, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "CLI", CLI, IMP, 2 },{ "EOR", EOR, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "EOR", EOR, ABX, 4 },{ "LSR", LSR, ABX, 7 },{ "???", XXX, IMP, 7 },
	{ "RTS", RTS, IMP, 6 },{ "ADC", ADC, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 3 },{ "ADC", ADC, ZP0, 3 },{ "ROR", ROR, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PLA", PLA, IMP, 4 },{ "ADC", ADC, IMM, 2 },{ "ROR", ROR, IMP, 2 },{ "???", XXX, IMP, 2 },{ "JMP", JMP, IND, 5 },{ "ADC", ADC, ABS, 4 },{ "ROR", ROR, ABS, 6 },{ "???", XXX, IMP, 6 },
	{ "BVS", BVS, REL, 2 },{ "ADC", ADC, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "ADC", ADC, ZPX, 4 },{ "ROR", ROR, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "SEI", SEI, IMP, 2 },{ "ADC", ADC, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "ADC", ADC, ABX, 4 },{ "ROR", ROR, ABX, 7 },{ "???", XXX, IMP, 7 },
	{ "???", NOP, IMP, 2 },{ "STA", STA, IZX, 6 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 6 },{ "STY", STY, ZP0, 3 },{ "STA", STA, ZP0, 3 },{ "STX", STX, ZP0, 3 },{ "???", XXX, IMP, 3 },{ "DEY", DEY, IMP, 2 },{ "???", NOP, IMP, 2 },{ "TXA", TXA, IMP, 2 },{ "???", XXX, IMP, 2 },{ "STY", STY, ABS, 4 },{ "STA", STA, ABS, 4 },{ "STX", STX, ABS, 4 },{ "???", XXX, IMP, 4 },
	{ "BCC", BCC, REL, 2 },{ "STA", STA, IZY, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 6 },{ "STY", STY, ZPX, 4 },{ "STA", STA, ZPX, 4 },{ "STX", STX, ZPY, 4 },{ "???", XXX, IMP, 4 },{ "TYA", TYA, IMP, 2 },{ "STA", STA, ABY, 5 },{ "TXS", TXS, IMP, 2 },{ "???", XXX, IMP, 5 },{ "???", NOP, IMP, 5 },{ "STA", STA, ABX, 5 },{ "???", XXX, IMP, 5 },{ "???", XXX, IMP, 5 },
	{ "LDY", LDY, IMM, 2 },{ "LDA", LDA, IZX, 6 },{ "LDX", LDX, IMM, 2 },{ "???", XXX, IMP, 6 },{ "LDY", LDY, ZP0, 3 },{ "LDA", LDA, ZP0, 3 },{ "LDX", LDX, ZP0, 3 },{ "???", XXX, IMP, 3 },{ "TAY", TAY, IMP, 2 },{ "LDA", LDA, IMM, 2 },{ "TAX", TAX, IMP, 2 },{ "???", XXX, IMP, 2 },{ "LDY", LDY, ABS, 4 },{ "LDA", LDA, ABS, 4 },{ "LDX", LDX, ABS, 4 },{ "???", XXX, IMP, 4 },
	{ "BCS", BCS, REL, 2 },{ "LDA", LDA, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 5 },{ "LDY", LDY, ZPX, 4 },{ "LDA", LDA, ZPX, 4 },{ "LDX", LDX, ZPY, 4 },{ "???", XXX, IMP, 4 },{ "CLV", CLV, IMP, 2 },{ "LDA", LDA, ABY, 4 },{ "TSX", TSX, IMP, 2 },{ "???", XXX, IMP, 4 },{ "LDY", LDY, ABX, 4 },{ "LDA", LDA, ABX, 4 },{ "LDX", LDX, ABY, 4 },{ "???", XXX, IMP, 4 },
	{ "CPY", CPY, IMM, 2 },{ "CMP", CMP, IZX, 6 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 8 },{ "CPY", CPY, ZP0, 3 },{ "CMP", CMP, ZP0, 3 },{ "DEC", DEC, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "INY", INY, IMP, 2 },{ "CMP", CMP, IMM, 2 },{ "DEX", DEX, IMP, 2 },{ "???", XXX, IMP, 2 },{ "CPY", CPY, ABS, 4 },{ "CMP", CMP, ABS, 4 },{ "DEC", DEC, ABS, 6 },{ "???", XXX, IMP, 6 },
	{ "BNE", BNE, REL, 2 },{ "CMP", CMP, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "CMP", CMP, ZPX, 4 },{ "DEC", DEC, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "CLD", CLD, IMP, 2 },{ "CMP", CMP, ABY, 4 },{ "NOP", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "CMP", CMP, ABX, 4 },{ "DEC", DEC, ABX, 7 },{ "???", XXX, IMP, 7 },
	{ "CPX", CPX, IMM, 2 },{ "SBC", SBC, IZX, 6 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 8 },{ "CPX", CPX, ZP0, 3 },{ "SBC", SBC, ZP0, 3 },{ "INC", INC, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "INX", INX, IMP, 2 },{ "SBC", SBC, IMM, 2 },{ "NOP", NOP, IMP, 2 },{ "???", SBC, IMP, 2 },{ "CPX", CPX, ABS, 4 },{ "SBC", SBC, ABS, 4 },{ "INC", INC, ABS, 6 },{ "???", XXX, IMP, 6 },
	{ "BEQ", BEQ, REL, 2 },{ "SBC", SBC, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "SBC", SBC, ZPX, 4 },{ "INC", INC, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "SED", SED, IMP, 2 },{ "SBC", SBC, ABY, 4 },{ "NOP", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "SBC", SBC, ABX, 4 },{ "INC", INC, ABX, 7 },{ "???", XXX, IMP, 7 },
};


void cpureset()
{
	cpu.a = 0;
	cpu.x = 0;
	cpu.y = 0;
	cpu.stkp = 0xFD;
	cpu.status = 0x00 | U;

	addr_abs = 0xFFFC;
	uint16_t lo = read(addr_abs);
	uint16_t hi = read(addr_abs + 1);
	cpu.pc = (hi << 8) | lo;

	addr_abs = 0x0000;
	addr_rel = 0x0000;
	fetched = 0x00;

	cycles = 8;
}


void cpuirq()
{
	if (getflag(I) == 0) {
		write(0x0100 + cpu.stkp, (cpu.pc >> 8) & 0x00FF);
		cpu.stkp--;
		write(0x0100 + cpu.stkp, cpu.pc & 0x00FF);
		cpu.stkp--;

		setflag(B, 0);
		setflag(U, 1);
		setflag(I, 1);
		write(0x0100 + cpu.stkp, cpu.status);
		cpu.stkp--;

		addr_abs = 0xFFFE;
		uint16_t lo = read(addr_abs);
		uint16_t hi = read(addr_abs + 1);
		cpu.pc = (hi << 8) | lo;

		cycles = 7;
	}
}


void cpunmi()
{
	write(0x0100 + cpu.stkp, (cpu.pc >> 8) & 0x00FF);
	cpu.stkp--;
	write(0x0100 + cpu.stkp, cpu.pc & 0x00FF);
	cpu.stkp--;

	setflag(B, 0);
	setflag(U, 1);
	setflag(I, 1);
	write(0x0100 + cpu.stkp, cpu.status);
	cpu.stkp--;

	addr_abs = 0xFFFA;
	uint16_t lo = read(addr_abs);
	uint16_t hi = read(addr_abs + 1);
	cpu.pc = (hi << 8) | lo;

	cycles = 8;
}


void cputick()
{
	if (cycles == 0) {
		opcode = busread(cpu.pc, 0);
		cpu.pc++;

		cycles = lookup[opcode].cycles;

		uint8_t addcycles1 = lookup[opcode].operate();
		uint8_t addcycles2 = lookup[opcode].addrmode();

		cycles += (addcycles1 & addcycles2);
	}

	cycles--;
}


uint8_t read(uint16_t addr)
{
	return busread(addr, 0);
}


void write(uint16_t addr, uint8_t data)
{
	buswrite(addr, data);
}


// addressing modes
uint8_t IMP()
{
	fetched = cpu.a;
	return 0;
}


uint8_t IMM()
{
	addr_abs = cpu.pc++;
	return 0;
}


uint8_t ZP0()
{
	addr_abs = read(cpu.pc);
	cpu.pc++;
	addr_abs &= 0x00FF;
	return 0;
}


uint8_t ZPX()
{
	addr_abs = (read(cpu.pc) + cpu.x);
	cpu.pc++;
	addr_abs &= 0x00FF;
	return 0;
}


uint8_t ZPY()
{
	addr_abs = (read(cpu.pc) + cpu.y);
	cpu.pc++;
	addr_abs &= 0x00FF;
	return 0;
}


uint8_t REL()
{
	addr_rel = read(cpu.pc);
	cpu.pc++;
	if (addr_rel & 0x80)
		addr_rel |= 0xFF00;
	return 0;
}


uint8_t ABS()
{
	uint16_t lo = read(cpu.pc);
	cpu.pc++;
	uint16_t hi = read(cpu.pc);
	cpu.pc++;

	addr_abs = (hi << 8) | lo;
	return 0;
}


uint8_t ABX()
{
	uint16_t lo = read(cpu.pc);
	cpu.pc++;
	uint16_t hi = read(cpu.pc);
	cpu.pc++;

	addr_abs = (hi << 8) | lo;
	addr_abs += cpu.x;

	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}


uint8_t ABY()
{
	uint16_t lo = read(cpu.pc);
	cpu.pc++;
	uint16_t hi = read(cpu.pc);
	cpu.pc++;

	addr_abs = (hi << 8) | lo;
	addr_abs += cpu.y;

	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}


uint8_t IND()
{
	uint16_t ptr_lo = read(cpu.pc);
	cpu.pc++;
	uint16_t ptr_hi = read(cpu.pc);
	cpu.pc++;

	uint16_t ptr = (ptr_hi << 8) | ptr_lo;

	if (ptr_lo == 0x00FF)
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
	else
		addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);

	return 0;
}


uint8_t IZX()
{
	uint16_t ptr = read(cpu.pc);
	cpu.pc++;

	uint16_t lo = read((uint16_t)(ptr + (uint16_t)cpu.x) & 0x00FF);
	uint16_t hi = read((uint16_t)(ptr + (uint16_t)cpu.x + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;

	return 0;
}


uint8_t IZY()
{
	uint16_t ptr = read(cpu.pc);
	cpu.pc++;

	uint16_t lo = read(ptr & 0x00FF);
	uint16_t hi = read((ptr + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;
	addr_abs += cpu.y;

	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}


// instructions
uint8_t fetch()
{
	if (!(lookup[opcode].operate == IMM))
		fetched = read(addr_abs);
	return fetched;
}


uint8_t ADC()
{
	fetch();

	temp = (uint16_t)cpu.a + (uint16_t)fetched + (uint16_t)getflag(C);

	setflag(C, temp > 255);
	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x80);
	setflag(V, (~((uint16_t)cpu.a ^ (uint16_t)fetched) & ((uint16_t)cpu.a ^ temp)) & 0x0080);

	cpu.a = temp & 0x00FF;

	return 1;
}


uint8_t AND()
{
	fetch();

	cpu.a &= fetched;
	setflag(Z, cpu.a == 0x00);
	setflag(N, cpu.a & 0x80);

	return 1;
}


uint8_t ASL()
{
	temp = (uint16_t)cpu.a << 1;

	setflag(C, temp > 255);
	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x80);

	if (lookup[opcode].addrmode == IMP)
		cpu.a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);

	return 0;
}


uint8_t BCC()
{
	if (getflag(C) == 0) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t BCS()
{
	if (getflag(C) == 1) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t BEQ()
{
	if (getflag(Z) == 1) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t BIT()
{
	fetch();

	setflag(Z, (cpu.a & fetched) == 0);
	setflag(N, fetched & 0x80);
	setflag(V, fetched & 0x40);

	return 0;
}


uint8_t BMI()
{
	if (getflag(N) == 1) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t BNE()
{
	if (getflag(Z) == 0) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t BPL()
{
	if (getflag(N) == 0) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t BRK()
{
	cpu.pc++;

	write(cpu.stkp, (cpu.pc >> 8) & 0x00FF);
	cpu.stkp--;
	write(cpu.stkp, cpu.pc & 0x00FF);
	cpu.stkp--;

	setflag(I, 1);
	setflag(B, 1);
	write(cpu.stkp, cpu.status);
	cpu.stkp--;

	cpu.pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);

	return 0;
}


uint8_t BVC()
{
	if (getflag(V) == 0) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t BVS()
{
	if (getflag(V) == 1) {
		cycles++;
		addr_abs = cpu.pc + addr_rel;

		if ((cpu.pc & 0xFF00) != (addr_abs & 0xFF00))
			cycles++;

		cpu.pc = addr_abs;
	}

	return 0;
}


uint8_t CLC()
{
	setflag(C, 0);

	return 0;
}


uint8_t CLD()
{
	setflag(D, 0);

	return 0;
}


uint8_t CLI()
{
	setflag(I, 0);

	return 0;
}


uint8_t CLV()
{
	setflag(V, 0);

	return 0;
}


uint8_t CMP()
{
	fetch();

	temp = (uint16_t)cpu.a - (uint16_t)fetched;
	setflag(C, cpu.a >= fetched);
	setflag(Z, cpu.a == fetched);
	setflag(N, (temp & 0x00FF) & 0x80);

	return 1;
}


uint8_t CPX()
{
	fetch();

	temp = (uint16_t)cpu.x - (uint16_t)fetched;
	setflag(C, cpu.x >= fetched);
	setflag(Z, cpu.x == fetched);
	setflag(N, (temp & 0x00FF) & 0x80);

	return 0;
}


uint8_t CPY()
{
	fetch();

	temp = (uint16_t)cpu.y - (uint16_t)fetched;
	setflag(C, cpu.y >= fetched);
	setflag(Z, cpu.y == fetched);
	setflag(N, (temp & 0x00FF) & 0x80);

	return 0;
}


uint8_t DEC()
{
	fetch();

	temp = fetched - 1;

	write(addr_abs, temp & 0x00FF);

	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x0080);

	return 0;
}


uint8_t DEX()
{
	cpu.x--;

	setflag(Z, cpu.x == 0);
	setflag(N, cpu.x & 0x80);

	return 0;
}


uint8_t DEY()
{
	cpu.y--;

	setflag(Z, cpu.y == 0);
	setflag(N, cpu.y & 0x80);

	return 0;
}


uint8_t EOR()
{
	fetch();

	cpu.a ^= fetched;

	setflag(Z, cpu.a == 0);
	setflag(N, cpu.a & 0x80);

	return 1;
}


uint8_t INC()
{
	fetch();

	temp = fetched + 1;

	write(addr_abs, temp & 0x00FF);

	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x0080);

	return 0;
}


uint8_t INX()
{
	cpu.x++;

	setflag(Z, cpu.x == 0);
	setflag(N, cpu.x & 0x80);

	return 0;
}


uint8_t INY()
{
	cpu.y++;

	setflag(Z, cpu.y == 0);
	setflag(N, cpu.y & 0x80);

	return 0;
}


uint8_t JMP()
{
	cpu.pc = addr_abs;

	return 0;
}


uint8_t JSR()
{
	cpu.pc--;

	write(0x0100 + cpu.stkp, (cpu.pc >> 8) & 0x00FF);
	cpu.stkp--;
	write(0x0100 + cpu.stkp, cpu.pc & 0x00FF);
	cpu.stkp--;

	cpu.pc = addr_abs;

	return 0;
}


uint8_t LDA()
{
	fetch();

	cpu.a = fetched;

	setflag(Z, cpu.a == 0);
	setflag(N, cpu.a & 0x80);

	return 1;
}


uint8_t LDX()
{
	fetch();

	cpu.x = fetched;

	setflag(Z, cpu.x == 0);
	setflag(N, cpu.x & 0x80);

	return 1;
}


uint8_t LDY()
{
	fetch();

	cpu.y = fetched;

	setflag(Z, cpu.y == 0);
	setflag(N, cpu.y & 0x80);

	return 1;
}


uint8_t LSR()
{
	fetch();

	if (lookup[opcode].addrmode == IMP) {
		temp = (uint16_t)cpu.a >> 1;
		setflag(C, cpu.a & 0x01);
	} else {
		temp = (uint16_t)fetched >> 1;
		setflag(C, fetched & 0x01);
	}

	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x80);

	if (lookup[opcode].addrmode == IMP)
		cpu.a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);

	return 0;
}


uint8_t NOP()
{
	switch (opcode) {
		case 0x1C:
		case 0x3C:
		case 0x5C:
		case 0x7C:
		case 0xDC:
		case 0xFC:
			return 1;
			break;
	}

	return 0;
}


uint8_t ORA()
{
	fetch();

	cpu.a |= fetched;

	setflag(Z, cpu.a == 0);
	setflag(N, cpu.a & 0x80);

	return 1;
}


uint8_t PHA()
{
	write(0x0100 + cpu.stkp, cpu.a);
	cpu.stkp--;

	return 0;
}


uint8_t PHP()
{
	setflag(U, 1);
	setflag(B, 1);

	write(0x0100 + cpu.stkp, cpu.status);
	cpu.stkp--;

	setflag(U, 0);
	setflag(B, 0);

	return 0;
}


uint8_t PLA()
{
	cpu.stkp++;
	cpu.a = read(0x0100 + cpu.stkp);

	setflag(Z, cpu.a == 0);
	setflag(N, cpu.a & 0x80);

	return 0;
}


uint8_t PLP()
{
	cpu.stkp++;
	cpu.status = read(0x0100 + cpu.stkp);

	return 0;
}


uint8_t ROL()
{
	fetch();

	temp = (uint16_t)fetched << 1 | getflag(C);
	cpu.a = temp & 0x00FF;

	setflag(C, temp & 0x0100);
	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x0080);

	if (lookup[opcode].addrmode == IMP)
		cpu.a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);

	return 0;
}


uint8_t ROR()
{
	fetch();

	temp = (uint16_t)fetched >> 1 | getflag(C) << 7;
	cpu.a = temp & 0x00FF;

	setflag(C, fetched & 0x01);
	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x0080);

	if (lookup[opcode].addrmode == IMP)
		cpu.a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);

	return 0;
}


uint8_t RTI()
{
	cpu.stkp++;
	cpu.status = read(0x0100 + cpu.stkp);
	setflag(B, 0);
	setflag(U, 0);
	cpu.stkp++;
	cpu.pc = read(0x0100 + cpu.stkp) | read(0x0100 + cpu.stkp + 1) << 8;
	cpu.stkp++;

	return 0;
}


uint8_t RTS()
{
	cpu.stkp++;
	cpu.pc = read(0x0100 + cpu.stkp) | read(0x0100 + cpu.stkp + 1) << 8;
	cpu.stkp++;

	cpu.pc++;

	return 0;
}


uint8_t SBC()
{
	fetch();

	uint16_t invval = ((uint16_t)fetched) ^ 0x00FF;

	temp = (uint16_t)cpu.a + invval + (uint16_t)getflag(C);

	setflag(C, temp > 255);
	setflag(Z, (temp & 0x00FF) == 0);
	setflag(N, temp & 0x80);
	setflag(V, (temp ^ invval) & ((uint16_t)cpu.a ^ temp) & 0x0080);

	cpu.a = temp & 0x00FF;

	return 1;
}


uint8_t SEC()
{
	setflag(C, 1);

	return 0;
}


uint8_t SED()
{
	setflag(D, 1);

	return 0;
}


uint8_t SEI()
{
	setflag(I, 1);

	return 0;
}


uint8_t STA()
{
	write(addr_abs, cpu.a);

	return 0;
}


uint8_t STX()
{
	write(addr_abs, cpu.x);

	return 0;
}


uint8_t STY()
{
	write(addr_abs, cpu.y);

	return 0;
}


uint8_t TAX()
{
	cpu.x = cpu.a;

	setflag(Z, cpu.x == 0);
	setflag(N, cpu.x & 0x80);

	return 0;
}


uint8_t TAY()
{
	cpu.y = cpu.a;

	setflag(Z, cpu.y == 0);
	setflag(N, cpu.y & 0x80);

	return 0;
}


uint8_t TSX()
{
	cpu.x = cpu.stkp;

	setflag(Z, cpu.x == 0);
	setflag(N, cpu.x & 0x80);

	return 0;
}


uint8_t TXA()
{
	cpu.a = cpu.x;

	setflag(Z, cpu.a == 0);
	setflag(N, cpu.a & 0x80);

	return 0;
}


uint8_t TXS()
{
	cpu.stkp = cpu.x;

	return 0;
}


uint8_t TYA()
{
	cpu.a = cpu.y;

	setflag(Z, cpu.a == 0);
	setflag(N, cpu.a & 0x80);

	return 0;
}
