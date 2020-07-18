#include <stdint.h>
#include "io.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "glu.hpp"
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
	switch( n ) {
		// memory size
	case 6 :		
	case 7 :
		if( auto glu = machine_is<GLU>() ) {
			if( v ) {
				glu->bankA_size |= 1 << (n-6);
			} else {
				glu->bankA_size &= ~(1 << (n-6));
			}
		}
	default : return;
	}
}

void VIA2::writeB(int n, bool v) {
	switch(n) {
	case 2 :
		// power-off
		if( !v) {
			QuitEmulator();
		}
		break;
	case 7 :
		// VIA1 60MHz timer
		machine->via1->ca1_in(v);
	}	
}


