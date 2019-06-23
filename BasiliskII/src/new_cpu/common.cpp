#include "registers.hpp"
#include "exceptions.hpp"
#include "common.hpp"
uint32_t EA::addr() {
	if( iaddr != (uint32_t) -1 ) {
		return iaddr;
	}
	uint32_t iaddr = cpu->A[n];
	uint32_t pc = cpu->PC;
	switch( mode ) {
	case 0:
	case 1:
		ILLEGAL(cpu);
		break;
	case 2 :
		return iaddr;
	case 3 :
		cpu->A[n] += sz;
		return iaddr;
	case 4 :
		iaddr = ( cpu->A[n] -= sz);
		return iaddr;
	case 5 :
		iaddr += (int16_t)cpu->fetch_w();
		return iaddr;
	case 6 :
		iaddr = cpu->ea_ax(iaddr);
		return iaddr;
	case 7 :
		switch(n) {
		case 0 : iaddr = (int16_t)cpu->fetch_w(); break;
		case 1 : iaddr = cpu->fetch_l(); break;
		case 2 :
			if( ! w ) {
				iaddr = pc + cpu->fetch_l();
			} else {
				ILLEGAL(cpu);
			}
			break;
		case 3 :
			if( ! w ) {
				iaddr = cpu->ea_ax(pc);
			} else {
				ILLEGAL(cpu);
			}
			break;
		default : ILLEGAL(cpu);
		}
		return iaddr;
	}
	ILLEGAL(cpu);
}
static int imask[] = { ~0, 0xff, 0xffff, ~0, ~0 };
uint32_t EA::read() {
	switch(mode) {
	case 0 : return cpu->D[n] & imask[sz];
	case 1 :
		switch(sz) {
		case 1 : ILLEGAL(cpu); break;
		case 2 : return (int16_t)cpu->A[n];
		default : return cpu->A[n];
		}
	case 7 :
		if( n == 4 && ! w ) {
			switch(sz) {
			case 1 : return cpu->fetch_w() & 0xff;
			case 2 : return cpu->fetch_w();
			default : return cpu->fetch_l();
			}
		}
		/* fallthrough */
	default :
		switch(sz) {
		case 1 : return cpu->mmu->read_b(addr());
		case 2 : return cpu->mmu->read_w(addr());
		default : return cpu->mmu->read_l(addr());
		}
	}
	ILLEGAL(cpu);
}

void EA::write(uint32_t v) {
	switch(mode) {
	case 0 : cpu->D[n] = (cpu->D[n] &~ imask[sz]) | (v & imask[sz]); return;
	case 1 :
		switch(sz) {
		case 1 : ILLEGAL(cpu); return;
		case 2 : cpu->A[n] = (int16_t)v; return;
		default : cpu->A[n] = v; return;
		}		
	default :
		switch(sz) {
		case 1 : return cpu->mmu->write_b(addr(), v);
		case 2 : return cpu->mmu->write_w(addr(), v);
		default : return cpu->mmu->write_l(addr(), v);
		}
	}
}

uint32_t CPU::ea_ax(uint32_t ax) {
	uint16_t nw = fetch_w();
	uint32_t rv = R[(nw>>12) & 15];
	if( (nw>>11)&1 ) {
		rv = (int16_t)(rv & 0xffff);
	}
	rv <<=  (nw >> 9)&3;
	if( (nw >> 8) & 1 ) {
		uint32_t bd = 0;
		switch( (nw >> 4) & 3 ) {
		case 0 : ILLEGAL(this);
		case 1 : break;
		case 2 : bd = fetch_w(); break;
		case 3 : bd = fetch_l(); break;
		}
		if( (nw >> 3 ) & 1 ) {
			switch( nw & 7 ) {
			case 0 : return ax+rv+bd;
			case 1 : return mmu->read_l(ax+bd+rv);
			case 2 : return mmu->read_l(ax+bd+rv) + fetch_w();
			case 3 : return mmu->read_l(ax+bd+rv) + fetch_l();
			case 4 : ILLEGAL(this);
			case 5 : return mmu->read_l(ax+bd)+rv;
			case 6 : return mmu->read_l(ax+bd)+rv + fetch_w();
			case 7 : return mmu->read_l(ax+bd)+rv + fetch_l(); 
			}
		} else {
			switch( nw & 7 ) {
			case 0 : return bd+rv;
			case 1 : return mmu->read_l(bd+rv);
			case 2 : return mmu->read_l(bd+rv) + fetch_w(); 
			case 3 : return mmu->read_l(bd+rv) + fetch_l();
			default : ILLEGAL(this);
			}
		}
		ILLEGAL(this);
	} else {
		return ax + rv + int8_t(nw&0xff);
	}	
}

