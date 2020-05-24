#include "io.hpp"
#include "rbv.hpp"
// TODO
uint8_t RBV::read(int addr) {
	switch(addr & 0x7) {
	case RBV_REG::EXP : return 0;
	case RBV_REG::MONP : return monP;
	case RBV_REG::CHPT: return 0;
	case RBV_REG::SIER : return sIER;
	default : return Via_Lite::read(addr & 0x7);
	}
}

void RBV::write(int addr, uint8_t v) {
	switch(addr & 0x7) {
	case RBV_REG::EXP :
		return;
		// TODO
	case RBV_REG::MONP : set_monp(v); return;
	case RBV_REG::SIER : sIER = v; return;
	default : Via_Lite::write(addr & 0x7, v); return;
	}
}
void SonorRBV::write(int addr, uint8_t v) {
	switch(addr) {
	case 0x1 : return ; // TODO; unknown; set 0
	case 0x4 : return ; // TODO; unknown; set 0x40
	case 0x5 : return ; // TODO; unknown; set 2 or 3(LCIII+ only)
	default : RBV::write(addr, v); return;
	}
}

void RBV::set_monp(uint8_t v) {
	monP = v;
	// TODO
}
