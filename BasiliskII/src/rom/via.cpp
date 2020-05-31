#include "data.hpp"
#include "registers.hpp"
#include "prototype.hpp"
#include "via1.hpp"
#include "machine.hpp"
#include "rbv.hpp"
#include <assert.h>
namespace ROM {
// $2E8C
void reset_via() {
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::VIA1) ) {
		const VIA_DATA_T* vdata1 = model->via_data1;
		if( vdata1 ) {
			machine->via1->write(VIA_REG::RA, vdata1->regA); // REG A
			machine->via1->write(VIA_REG::DDRA, vdata1->dirA); // DIR A
			machine->via1->write(VIA_REG::RB, vdata1->regB); // REG B
			machine->via1->write(VIA_REG::DDRB, vdata1->dirB); // DIR B
			machine->via1->write(VIA_REG::PCR, vdata1->pcr);  // PCR
			machine->via1->write(VIA_REG::ACR, vdata1->acr);  // AUX-CR
			machine->via1->write(VIA_REG::IER, 0x7f);				   // IRQ
		}
	}
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::VIA2) ) {
		auto vdata2 = model->via_data2;
		if( vdata2) {
			uint8_t d3 = machine->via2->read(VIA_REG::RA); // REG A
			machine->via2->write(VIA_REG::RA, d3|vdata2->regA); // REG A
			machine->via2->write(VIA_REG::DDRA, vdata2->dirA); // DIR A
			machine->via2->write(VIA_REG::RB, vdata2->regB); // REG B
			machine->via2->write(VIA_REG::DDRB, vdata2->dirB); // DIR B
			machine->via2->write(VIA_REG::PCR, vdata2->pcr);  // PCR
			machine->via2->write(VIA_REG::ACR , vdata2->acr);  // AUX-CR
			machine->via2->write(VIA_REG::IER, 0x7f);						// IRQ
			machine->via2->write(VIA_REG::TIMER1_L, 0x6e); // TIMER=6510cnt(=8.31ms):= 1/120s?
			machine->via2->write(VIA_REG::TIMER1_H, 0x19);
		}
	}
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::RBV) ) {
		machine->rbv->write(int(RBV_REG::IER), 0x7f);
		machine->rbv->write(int(RBV_REG::RB), 0x8f);
		if( ROM_FLG.test( ROM_FLG_T::MONITOR) ) {
			cpu.Z = dynamic_cast<NoFPU*>(fpu) != nullptr;	// has FPU?
			if( cpu.Z ) {
				machine->rbv->write(int(RBV_REG::RB), 0x8e);
			}
			machine->rbv->write(int(RBV_REG::SIER), 0x7f);
		} else {
			// turns off video
			machine->rbv->write(int(RBV_REG::MONP), RBV_VIDOFF);
			machine->rbv->write(int(RBV_REG::SIER), 0xff);			
		}			
	}
}

template<class T>
inline uint8_t read_via_impl(T&& via,
							 VIA_REG::VIA_REG_t data,
							 VIA_REG::VIA_REG_t dir,
							 uint8_t mask) {
	uint8_t v_1 = via->read(data);
	uint8_t v_2 = via->read(dir);
	via->write(dir, v_2 & mask);
	uint8_t v = via->read(data);
	via->write(dir, v_2);
	via->write(data, v_1);
	return v;
}



// $47CC
uint32_t read_id_from_via() {
	uint8_t h = 0, l = 0;
	uint32_t v = 0;
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::VIA1) ) {
		h = read_via_impl(machine->via1, VIA_REG::RA, VIA_REG::DDRA, motherboard->via1A_mask);
		l = read_via_impl(machine->via1, VIA_REG::RB, VIA_REG::DDRB, motherboard->via1B_mask);
	}	
	v = h << 24 | l << 16;		
	if( ! INIT_HW_FLG.test( INIT_HW_FLG_T::VIA2 ) ) {
		return v;
	}
	h = read_via_impl(machine->via2, VIA_REG::RA, VIA_REG::DDRA, motherboard->via2A_mask);
	l = read_via_impl(machine->via2, VIA_REG::RB, VIA_REG::DDRB, motherboard->via2B_mask);
	v |= h << 8 | l;
	return v;
}

// reset VIA timer2
// $BA090
void reset_VIA1_timer2() {
	getHWInfo2(0);				// $47752
	machine->via1->write( VIA_REG::ACR, 0 );
	machine->via1->write( VIA_REG::IER, 0x20 );
	machine->via1->write( VIA_REG::TIMER2_L, 0xff );
	machine->via1->write( VIA_REG::TIMER2_H, 0xff );
}
}

namespace ROMWrapper {
using namespace ROM;
void run_02E8C() {
	get_motherboard(AR(0));
	get_model(AR(1));
	INIT_HW_FLG = DR(0);
	ROM_FLG = DR(1);
	reset_via();
	cpu.PC = AR(6);
}

void run_047CC() {
	get_motherboard(AR(0));
	assert( test_machine_register( machine->via1, motherboard->via1_addr) );
	INIT_HW_FLG = DR(0);
	DR(1) = read_id_from_via();
	cpu.PC = AR(6);
}

void run_BA090() {
	get_motherboard(AR(0));
	assert( test_machine_register( machine->via1, motherboard->via1_addr) );
	reset_VIA1_timer2();
	cpu.PC = AR(6);
}
}
