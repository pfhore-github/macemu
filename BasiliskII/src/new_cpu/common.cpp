#include "registers.hpp"
#include "memory.hpp"
void ILLEGAL(REGS* regs) __attribute__((noreturn));
void ILLEGAL(REGS* regs) { abort(); }
static inline uint32_t* A7(REGS* regs) {
	if( ! regs->S ) { return &regs->USP; }
	else if( regs->IX ) { return &regs->ISP; }
	else { return &regs->MSP; }
}
uint32_t ea_ax(REGS* regs, uint16_t nw, uint32_t ax) {
	uint32_t rv = regs->R[(nw>>12) & 15];
	if( (nw>>11)&1 ) {
		rv = (int16_t)(rv & 0xffff);
	}
	rv <<=  (nw >> 9)&3;
	if( (nw >> 8) & 1 ) {
		uint32_t bd = 0;
		switch( (nw >> 4) & 3 ) {
		case 0 : ILLEGAL(regs);
		case 1 : break;
		case 2 : bd = fetch_w(regs->PC); break;
		case 3 : bd = fetch_l(regs->PC); break;
		}
		if( (nw >> 3 ) & 1 ) {
			switch( nw & 7 ) {
			case 0 : return ax+rv+bd;
			case 1 : return read_l(ax+bd+rv);
			case 2 : return read_l(ax+bd+rv) + fetch_w(regs->PC);
			case 3 : return read_l(ax+bd+rv) + fetch_l(regs->PC);
			case 4 : ILLEGAL(regs);
			case 5 : return read_l(ax+bd)+rv;
			case 6 : return read_l(ax+bd)+rv + fetch_w(regs->PC);
			case 7 : return read_l(ax+bd)+rv + fetch_l(regs->PC); 
			}
		} else {
			switch( nw & 7 ) {
			case 0 : return bd+rv;
			case 1 : return read_l(bd+rv);
			case 2 : return read_l(bd+rv) + fetch_w(regs->PC); 
			case 3 : return read_l(bd+rv) + fetch_l(regs->PC);
			default : ILLEGAL(regs);
			}
		}
		ILLEGAL(regs);
	} else {
		return ax + rv + int8_t(nw&0xff);
	}	
}

