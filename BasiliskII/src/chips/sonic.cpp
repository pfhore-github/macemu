#include "sonic.hpp"
enum class SONIC_REG {
	CMD,
	DCR,
	TCR,
	IMR,
};
static constexpr SONIC_REG reg(int n) { return SONIC_REG(( n >> 1 ) & 0xff) ; }
uint16_t SONIC::read16(int n) {
	switch(reg(n)) {
	case SONIC_REG::CMD : return cmd_reg;
	}
	return 0;
}
void SONIC::write16(int n, uint16_t v) {
	switch(reg(n)) {
	case SONIC_REG::CMD:
		cmd_reg = v;
		if( v & 1 ) {
			// HTX
		}
		if( v & 2 ) {
			// TXP
		}
		if( v & 4 ) {
			//RXDIS
		}
		break;
	}
}
