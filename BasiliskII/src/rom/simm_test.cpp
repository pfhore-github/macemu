#include <stdint.h>
#include "sysdeps.h"
#include "main.h"
#include "registers.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "inst_cmn.hpp"
#include "rbv.hpp"
#include "oss.hpp"
#include "jaws.hpp"
#include "intel.h"
#include "mem.hpp"
namespace ROM {
// $A0242
check_simm_size_t check_simm_size(uint32_t base, uint32_t end, uint32_t data, uint32_t unit) {
	uint32_t begin = base;
	uint32_t d2 = 0;
	while( begin < end-4 ) {
		d2 = read_l(end-4);
		write_l(end-4, data);
		if( read_l(end-4) == data )
			break;
		end -= unit;		
	}
	if( begin >= end-4 ) {
		return { 0, end };
	}
	do {
		begin += unit;
	} while( read_l(begin-4) != data );
	write_l( begin-4, d2 );
	uint32_t size = begin - base;
	return { size, end };
}

// $A0286
std::bitset<4> check_simm(uint32_t base, uint32_t check) {
	uint32_t v1 = read_l(base); 
	uint32_t v2 = read_l(base+4);
	write_l(base, check);
	write_l(base+4, 0xffffffff);
	std::bitset<4> invalid = 0;
	for(int i = 3; i >= 0; --i ) {
		if( read_b(base+i) != ( check & 0xff) ) {
			invalid.set(i);
		} else {
			check ^= 0xff;
			write_b(base+i, read_b(base+i)^0xff);
			if( read_b(base+i) != ( check & 0xff) ) {
				invalid.set(i);
			}
		}
		check >>= 8;
	}
	write_l(base, v1);
	write_l(base+4, v2);
	return invalid;
}
}
namespace ROMWrapper {
using namespace ROM;
void run_A0242() {
	uint32_t fp = AR(6);
	uint32_t unit = read_l( AR(5) );
	auto [ size, end ]  = check_simm_size( AR(0), AR(1), DR(1), unit );
	AR(0) = end;
	DR(5) |= ( size ? 32 - __builtin_clz(size/unit) : 0 );
	DR(0) += size;
	cpu.PC = fp;
}

void run_A0286() {
	uint32_t fp = AR(6);
	DR(6) |= check_simm(AR(0), DR(1)).to_ulong();
	cpu.PC = fp;
}
}
