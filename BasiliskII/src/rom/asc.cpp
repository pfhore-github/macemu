#include "data.hpp"
#include "asc.hpp"
#include "via1.hpp"
#include "prototype.hpp"
#include <assert.h>
void nanosleep_x(double ns);
namespace ROM {
// $7058
void play_asc_2() {
	ASC_param param = { 0x0607, 39, 0, 5000, 4, { 49308, 57526, 73962, 98993 } };
	play_asc(param); // $0706E
}
// $705E
void play_asc_3() {
	ASC_param param = { 0x0506, 39, 3000, 20000, 1, { 197234, 0, 0, 0 } };
	play_asc(param); // $0706E
}
// $706A
void play_asc_5() {
	ASC_param param = { 0x0607, 43, 3000, 25000, 4, { 82927, 103659, 124391, 166231 } };
	play_asc(param); // $0706E
}

// $706E
void play_asc(const ASC_param& p) {
	// disable ASC
	machine->asc->write(ASC_REG::MODE, 0);
	// set to 22257Hz
	machine->asc->write(ASC_REG::CLOCK, 0);

	uint16_t vol = p.volume << 2;
	// set volume
	if( machine->asc->read(ASC_REG::VERSION) == 0 ) {
		vol >>= 5;
	}
	machine->asc->write(ASC_REG::VOLUME, vol);

	// if not asc then change CONTROL to PWM?
	machine->asc->write(ASC_REG::CONTROL, machine->asc->read(ASC_REG::VERSION) != 0);
	int addr = 0x810;
	// reset freq table
	for(int j = 0; j < 32; ++j ) {
		machine->asc->write(addr++, 0);
	}
	
	// ???
	for(int i = 0; i < 8; ++i ) {
		machine->asc->write(addr++, -2);
	}
	
	// change to WAVE TABLE MODE
	machine->asc->write(ASC_REG::MODE, 2);

	// disable companding
	machine->asc->write(ASC_REG::FIFO_MODE, 0);
	
	static uint8_t d0_1[] = { 0x40, 0xff, 0x01, 0xc0 };
	static uint8_t d0_2[] = { 0x10, 0x3f, 0x01, 0x30 };
	// set wave data
	static uint8_t* wave = machine->asc->read(ASC_REG::VERSION) != 0 ? d0_1 : d0_2;
	for(int j =0, a = 0; j < 32; ++j ) {
		for(int i =0; i < 64; ++i ) {
			machine->asc->write(a++, wave[j%4]);
		}
	}
	// $70E0
	int d5 = 0;
	addr = 0x0814;
	// A0 = ASC_REG::BASE
	int a0 = 0;
	// $70F8
	int k = 0;
	int chs = p.chs;
	for(unsigned int i = 0; i < p.repeat; ++i ) {
		// set wave table
		if( --d5 == -1 ) {
			if( chs ) {
				d5 = p.delay;
				machine->asc->write32(addr, p.freq[k++]);
				addr += 8;
				--chs;
			}
		}
		// $711C
		uint16_t d4 = machine->asc->read(a0);
		a0 = (a0+1) & 0x1ff;
		d4 += machine->asc->read(a0);
		uint8_t d4x = d4 >> 1;
		machine->asc->write(a0+0x600, d4x);
		machine->asc->write(a0+0x400, d4x);
		machine->asc->write(a0+0x200, d4x);
		machine->asc->write(a0, d4x);
		// reset IEQ
		nanosleep_x(p.wait*1500);
		machine->via1->read(VIA_REG::IER);
	} 
	
	// $7136
	// reset wave table
	for(int i = 0; i < 4; ++i ) {
		machine->asc->write32(0x814 + 8 * i , 0);
	}
	// stop ASC
	machine->asc->write(ASC_REG::MODE, 0);
	return;
}

}
static void check_asc() {
	assert( ROM::test_machine_register(machine->asc, AR(3) ) );
	assert( ROM::test_machine_register(machine->via1, AR(5) ) );
}
namespace ROMWrapper {
void run_07058() {
	check_asc();
	ROM::play_asc_2();
}
void run_0705E() {
	check_asc();
	ROM::play_asc_3();
}

void run_0706A() {
	check_asc();
	ROM::play_asc_5();
}


void run_0706E() {
	check_asc();
	ROM::ASC_param p;
	uint32_t a4 = AR(4);
	p.volume = ROM::read_rom16(a4);
	p.wait = ROM::read_rom32(a4+2);
	p.delay = ROM::read_rom32(a4+6);
	p.repeat = ROM::read_rom32(a4+10);
	p.chs = ROM::read_rom16(a4+14);
	for(int i = 0; i < p.chs; ++i ) {
		p.freq[i] = ROM::read_rom32(a4+16+4*i);
	}
	ROM::play_asc(p);
	cpu.PC = AR(6);
}
}
