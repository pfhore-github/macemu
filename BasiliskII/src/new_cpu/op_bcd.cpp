#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "inst_cmn.hpp"
#define OP(name_) extern "C" void op_##name_(uint16_t op , int dn , int mode, int reg )
#include "op.h"
int from_bcd(uint8_t c) {
	return (c >> 4)*10 + (c & 15 );
}
uint8_t to_bcd(int v) {
	return ((v/10) << 4) | ( v % 10);
}

uint8_t nbcd(int v, bool b) {
	int d = 100 - from_bcd(v) - b;
	cpu.X = cpu.C = (d != 100);
	cpu.Z &= (d == 100);
	return to_bcd(d%100);
}

uint8_t sbcd(int a, int b, bool c) {
	int d = from_bcd(a) - from_bcd(b) - c;
	cpu.X = cpu.C = (d < 0);
	int dx = d % 100;
	cpu.Z &= dx == 0;
	return to_bcd(dx);
}

uint8_t abcd(int a, int b, bool c) {
	int d = from_bcd(a) + from_bcd(b) + c;
	cpu.X = cpu.C = d >= 100;
	int dx = d % 100;
	cpu.Z &= dx == 0;
	return to_bcd(dx);
}

uint16_t pack(uint16_t a, uint16_t adj) {
	a += adj;
	return ((a >> 4) & 0xf0) | (a & 0xf);
}

uint16_t unpack(uint16_t a, uint16_t adj) {
	return (((a & 0xf0) << 4) | (a & 0xf)) + adj;
}

OP(nbcd_d) {
	cpu.setD_B(reg, nbcd(cpu.R[reg], cpu.X));
}

OP(nbcd_m) {
	cpu.parse_EA(mode, 1, reg, true);
	cpu.ea_write(nbcd(cpu.ea_read(), cpu.X));
}

OP(sbcd_d) {
	cpu.R[dn] = sbcd( cpu.R[dn], cpu.R[reg], cpu.X);
}

OP(sbcd_m) {
	uint8_t a = read_b(--cpu.R[8+dn]);
	uint8_t b = read_b(--cpu.R[8+reg]);
	write_b(cpu.R[8+dn], sbcd( a, b, cpu.X));
}

OP(pack_d) {
	uint16_t adj = fetch_w();
	cpu.R[dn] = pack( cpu.R[reg], adj);
}

OP(pack_m) {
	uint16_t adj = fetch_w();
	uint16_t a = read_w(cpu.R[8+reg] -= 2);
	write_b(--cpu.R[8+dn], pack( a, adj) );
}

OP(unpk_d) {
	uint16_t adj = fetch_w();
	cpu.R[dn] = unpack( cpu.R[reg], adj);
}

OP(unpk_m) {
	uint16_t adj = fetch_w();
	uint16_t a = read_w(--cpu.R[8+reg]);
	write_w(cpu.R[8+dn] -= 2, unpack( a, adj) );
}

OP(abcd_d) {
	cpu.R[dn] = abcd( cpu.R[dn], cpu.R[reg], cpu.X);
}

OP(abcd_m) {
	uint8_t a = read_b(--cpu.R[8+dn]);
	uint8_t b = read_b(--cpu.R[8+reg]);
	write_b(cpu.R[8+dn], abcd( a, b, cpu.X));
}
