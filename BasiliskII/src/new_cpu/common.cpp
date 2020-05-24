#include "registers.hpp"
#include "common.hpp"
void CPU::parse_EA(int mode, int sz, int n, bool w) {
	ea_sz = sz;
	if( mode == 0 ) {
		ea_mode = REG_D;
		ea_v = n;
		return;
	} else if( mode == 1 ) {
		ea_mode = REG_A;
		ea_v = n;
		return;
	} else if( mode == 7 && n == 4 ) {
		ea_mode = IMM;		
		switch(sz) {
		case 1 : ea_v = fetch_w() & 0xff; break;
		case 2 : ea_v = fetch_w(); break;
		default : ea_v = fetch_l(); break;
		}
		return;
	}
	ea_mode = ADDR;	
	uint32_t pc = PC;
	switch( mode ) {
	case 2 : ea_v = R[8+n]; return; 
	case 3 :
		ea_v = R[8+n];
		R[8+n] += sz;
		if( n == 7 && sz == 1 )
			R[8+n]++;
		return;
	case 4 :
		R[8+n] -= sz;
		if( n == 7 && sz == 1 )
			R[8+n]--;
		ea_v = R[8+n];
		return;
	case 5 :
		ea_v = R[8+n] + (int16_t)fetch_w();
		return ;
	case 6 :
		ea_v = R[8+n];
		break;
	case 7 :
		switch(n) {
		case 0 : ea_v = (int16_t)fetch_w(); return;
		case 1 : ea_v = fetch_l(); return;
		case 2 :
			if( w )
				throw Exception::IllegalInst();
			else
				ea_v = pc + (int16_t)fetch_w();
			return;
		case 3 :
			if( w )
				throw Exception::IllegalInst();
			else
				ea_v = pc;
			break;
		default : throw Exception::IllegalInst();
		}
	}
	uint16_t nw = fetch_w();
	uint32_t rv = R[(nw>>12) & 15];
	if( ! ((nw>>11)&1) ) {
		rv = (int16_t)(rv & 0xffff);
	}
	rv <<=  (nw >> 9)&3;
	if( (nw >> 8) & 1 ) {
		if( (nw >> 7) & 1 ) {
			rv = 0;
		}
		uint32_t bd = 0;
		switch( (nw >> 4) & 3 ) {
		case 0 : throw Exception::IllegalInst();
		case 1 : break;
		case 2 : bd = (int16_t)fetch_w(); break;
		case 3 : bd = fetch_l(); break;
		}
		if( !((nw >> 6 ) & 1) ) {
			switch( nw & 7 ) {
			case 0 : ea_v += rv+bd; return;
			case 1 : ea_v = read_l(ea_v+bd+rv); return;
			case 2 : ea_v = read_l(ea_v+bd+rv) + (int16_t)fetch_w(); return;
			case 3 : ea_v = read_l(ea_v+bd+rv) + fetch_l(); return;
			case 4 : throw Exception::IllegalInst();
			case 5 : ea_v = read_l(ea_v+bd)+rv; return;
			case 6 : ea_v = read_l(ea_v+bd)+rv + (int16_t)fetch_w(); return;
			case 7 : ea_v = read_l(ea_v+bd)+rv + fetch_l(); return;
			}
		} else {
			switch( nw & 7 ) {
			case 0 : ea_v = bd+rv; return;
			case 1 : ea_v = read_l(bd+rv); return;
			case 2 : ea_v = read_l(bd+rv) + (int16_t)fetch_w(); return;
			case 3 : ea_v = read_l(bd+rv) + fetch_l(); return;
			default : throw Exception::IllegalInst();
			}
		}
	} else {
		ea_v += rv + int8_t(nw&0xff);
	}	
}
static int imask[] = { ~0, 0xff, 0xffff, ~0, ~0 };
uint32_t CPU::ea_read() {
	switch(ea_mode) {
	case REG_D:
		return cpu.R[ea_v] & imask[ea_sz];
	case REG_A:
		switch(ea_sz) {
		case 1 : throw Exception::IllegalInst();
		case 2 : return (int16_t)cpu.R[8+ea_v];
		default : return cpu.R[8+ea_v];
		}
	case IMM:
		return ea_v;
	case ADDR:
		switch(ea_sz) {
		case 1 : return read_b(ea_v);
		case 2 : return read_w(ea_v);
		default : return read_l(ea_v);
		}
	}
	return 0;
}

void CPU::ea_write(uint32_t v) {
	switch(ea_mode) {
	case REG_D:
		switch(ea_sz) {
		case 1 : cpu.setD_B(ea_v, v); return;
		case 2 : cpu.setD_W(ea_v, v); return;
		default : cpu.R[ea_v] = v; return;
		}
	case REG_A:
		switch(ea_sz) {
		case 1 : throw Exception::IllegalInst();
		case 2 : cpu.setA_W(ea_v, v); return;
		default : cpu.R[8+ea_v] = v; return;
		}		
	case IMM:
		throw Exception::IllegalInst();
	case ADDR:
		switch(ea_sz) {
		case 1 : return write_b(ea_v, v);
		case 2 : return write_w(ea_v, v);
		default : return write_l(ea_v, v);
		}
	}
}


