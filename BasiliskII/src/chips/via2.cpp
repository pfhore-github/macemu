#include <stdint.h>
#include "io.hpp"
#include "via.hpp"
#include "registers.hpp"
#include "via2.hpp"
#include "machine.hpp"
// VIA2 

enum REGA {
	IRQ9 = 0,
	IRQA,
	IRQB,
	IRQC,
	IRQD,
	IRQE,
	RAM0_IRQV,
	RAM1
};
enum REGB {
	v2CDis = 0,
	v2BusLk,
	v2Poweroff,
	vFC3,
	v2TM1A,
	v2TM0A,
	v2SNDEXT,
	v2VBL
};
// STUB: nuBus?
bool VIA2::readA(int n) {
	return false;
}	

bool VIA2::readB(int n) {
	switch(n) {
	case v2CDis : return false;
	case v2BusLk : return true; /* ??? */
	case v2Poweroff : return false;
	case vFC3 : return machine->model_map2;
	case v2TM1A : return false;
	case v2TM0A : return false;
	case v2SNDEXT : return sjack;
	case v2VBL : return false;
	}
	return false;
}
void QuitEmulator();
void VIA2::writeA(int n, bool v) {
	return;
}

void VIA2::writeB(int n, bool v) {
	if(n == 2 && !v)
		QuitEmulator();
}


