#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#define OP(name_) extern "C" void op_##name_(CPU* cpu, uint16_t op __attribute__((unused)), int dn __attribute__((unused)), int mode __attribute__((unused)), int reg __attribute__((unused)))
#include "op.h"
int from_bcd(uint8_t c) {
	return (c >> 4)*10 + (c & 15 );
}
uint8_t to_bcd(int v) {
	return ((v/10) << 4) | ( v % 10);
}

uint8_t nbcd(CPU* cpu, int v, bool b) {
	int d = 100 - from_bcd(v) - b;
	cpu->X = cpu->C = (d != 100);
	cpu->Z = d == 100;
	return to_bcd(d%100);
}

uint8_t sbcd(CPU* cpu, int a, int b, bool c) {
	int d = from_bcd(a) - from_bcd(b) - c;
	cpu->X = cpu->C = (d < 0);
	int dx = d % 100;
	cpu->Z = dx == 0;
	return to_bcd(dx);
}

uint8_t abcd(CPU* cpu, int a, int b, bool c) {
	int d = from_bcd(a) + from_bcd(b) + c;
	cpu->X = cpu->C = d >= 100;
	int dx = d % 100;
	cpu->Z = dx == 0;
	return to_bcd(dx);
}

uint16_t pack(uint16_t a, uint16_t adj) {
	a += adj;
	return (a >> 4) & 0xf0 | a & 0xf;
}

uint16_t unpack(uint16_t a, uint16_t adj) {
	return (((a & 0xf0) << 4) | (a & 0xf)) + adj;
}

OP(nbcd_d) {
	cpu->D[reg] = nbcd(cpu, cpu->D[reg], cpu->X);
}

OP(nbcd_m) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write(nbcd(cpu, ea.read(), cpu->X));
}

OP(sbcd_d) {
	cpu->D[reg] = sbcd(cpu, cpu->D[dn], cpu->D[reg], cpu->X);
}

OP(sbcd_m) {
	uint8_t a = cpu->mmu->read_b(--cpu->A[dn]);
	uint8_t b = cpu->mmu->read_b(--cpu->A[reg]);
	cpu->mmu->write_b(cpu->A[reg], sbcd(cpu, a, b, cpu->X));
}

OP(pack_d) {
	uint16_t adj = cpu->fetch_w();
	cpu->D[dn] = pack( cpu->D[reg], adj);
}

OP(pack_m) {
	uint16_t adj = cpu->fetch_w();
	uint16_t a = cpu->mmu->read_w(cpu->A[reg] -= 2);
	cpu->mmu->write_w(cpu->A[dn] -= 2, pack( a, adj) );
}

OP(unpk_d) {
	uint16_t adj = cpu->fetch_w();
	cpu->D[dn] = unpack( cpu->D[reg], adj);
}

OP(unpk_m) {
	uint16_t adj = cpu->fetch_w();
	uint16_t a = cpu->mmu->read_w(cpu->A[reg] -= 2);
	cpu->mmu->write_w(cpu->A[dn] -= 2, unpack( a, adj) );
}

OP(abcd_d) {
	cpu->D[reg] = abcd(cpu, cpu->D[dn], cpu->D[reg], cpu->X);
}

OP(abcd_m) {
	uint8_t a = cpu->mmu->read_b(--cpu->A[dn]);
	uint8_t b = cpu->mmu->read_b(--cpu->A[reg]);
	cpu->mmu->write_b(cpu->A[reg], abcd(cpu, a, b, cpu->X));
}
