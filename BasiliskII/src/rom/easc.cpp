#include "data.hpp"
#include "prototype.hpp"
#include "asc.hpp"
#include "via1.hpp"
#include "machine.hpp"
#include <assert.h>
static const ROM::EASC_param params_list[] = {
	{ false, 0, 0, 0, 0x8000, 0xC000, 0, 0xC080 }, 
	{ true, 8, 8, 8, 0x8000, 0xA145, 0xBFC9, 0 },
	{ true, 8, 8, 8, 0x8000, 0x9838, 0xB505, 0xD745 },
	{ true, 12, 6, 12, 0x8000, 0x8FAD, 0x9838, 0x8000 },
	{ true, 8, 8, 8, 0xA145, 0xAADC, 0xA145, 0x879C },
};

namespace ROM {
// $45C1C
void play_chime_1() {
	if( machine->asc->read(ASC_REG::VERSION) == 0 ) {
		// pure ASC
		play_asc_2();	// $07058
	} else {
		// EASC variant
		play_easc(1);		// $45C64
	}
}
// $45C52
void play_chime_4() {
	if( machine->asc->read(ASC_REG::VERSION) == 0 ) {
		// pure ASC
		play_asc_5();	// $0706A
	} else {
		// EASC variant
		play_easc(4);		// $45C64
	}
}
// $45C64
void play_easc(int num) {
	auto params = &params_list[num];
	bool is_sonora = easc_is_sonora();
	if( is_sonora ) { // $BD43A
		// half irq enable
		machine->asc->write(ASC_REG::A_IRQ_CTL, 1);
		machine->asc->write(ASC_REG::B_IRQ_CTL, 1);
		// 22 RECORD(disabled)
		machine->asc->write(ASC_REG::REC, 2);
	}
	// FIFO RESET
	machine->asc->write(ASC_REG::FIFO_MODE, machine->asc->read(ASC_REG::FIFO_MODE) | 0x80);
	machine->asc->write(ASC_REG::FIFO_MODE, machine->asc->read(ASC_REG::FIFO_MODE) & 0x7f);
	// VOLUE RESET
	machine->asc->write(ASC_REG::VOLUME, 0xe0);
	// MODE RESET to FIFO
	machine->asc->write(ASC_REG::MODE, 1);
	if( ! is_sonora ) { // $BD44C
		// RESET clock
		machine->asc->write(ASC_REG::CLOCK, 0);
		// change to stereo?
		machine->asc->write(ASC_REG::CONTROL, 2);
	} else {
		machine->asc->write(ASC_REG::A_FIFO_CTL, 0);
		machine->asc->write(ASC_REG::B_FIFO_CTL, 0);
		machine->asc->write16(ASC_REG::A_SAMPLE_RATE_INCREMENT, 0x812F); // 1.001
		machine->asc->write16(ASC_REG::B_SAMPLE_RATE_INCREMENT, 0x812F); // 1.001
		machine->asc->write16(ASC_REG::A_CDXA_DECOMP + 0, 0);
		machine->asc->write16(ASC_REG::A_CDXA_DECOMP + 2, 0x3c);
		machine->asc->write16(ASC_REG::A_CDXA_DECOMP + 4, 0xcc73);
		machine->asc->write16(ASC_REG::A_CDXA_DECOMP + 6, 0xc962);
		machine->asc->write16(ASC_REG::B_CDXA_DECOMP + 0, 0);
		machine->asc->write16(ASC_REG::B_CDXA_DECOMP + 2, 0x3c);
		machine->asc->write16(ASC_REG::B_CDXA_DECOMP + 4, 0xcc73);
		machine->asc->write16(ASC_REG::B_CDXA_DECOMP + 6, 0xc962);
		machine->asc->write(ASC_REG::A_LEFT_VOLUME, 0x7f);
		machine->asc->write(ASC_REG::A_RIGHT_VOLUME, 0);
		machine->asc->write(ASC_REG::B_LEFT_VOLUME, 0);
		machine->asc->write(ASC_REG::B_RIGHT_VOLUME, 0x7f);
		// enable CD-XA
		machine->asc->write(ASC_REG::A_FIFO_CTL, 0x80);
		machine->asc->write(ASC_REG::B_FIFO_CTL, 0x80);
	}
	
	if( is_sonora ) {
		for(int i = 0; i < 128; ++i ) {
			machine->asc->write32(0, 0x80808080);
			machine->asc->write32(0x400, 0x80808080);
		}
	}
	if( params->long_sound ) {
		for(int i = 0; i < params->repeat1; ++i ) {
			easc_beep(1, params->data1, 1, 1, 0x4000); // $45EA2
			wait_until_easc_fifo_half();					// $45E3C
		}
		for(int i = 0; i < params->repeat2; ++i ) {
			easc_beep(params->data2, params->data1, 1, 1, 0x4000); // $45EA2
			wait_until_easc_fifo_half();					// $45E3C
		}
		for(int i = 0; i < params->repeat3; ++i ) {
			easc_beep( params->data2, params->data1, 1, params->data3, 0x4000 ); // $45EA2
			wait_until_easc_fifo_half();					// $45E3C
		}
	}
	for(int i = 0, vol = 0x4000; i < 0x21 ; ++i, vol -= 0x200 ) {
		easc_beep(params->data2, params->data1, params->data4, params->data3, vol ); // $45EA2
		wait_until_easc_fifo_half();					// $45E3C
	}
	if( is_sonora ) { // $BD470
		int v;
		do {
			v = check_easc_fifo_done(); // $BD494
		} while ( v != 4 && v != 3);
	}
	machine->asc->write( ASC_REG::MODE, 0 );
	machine->asc->write( ASC_REG::VOLUME, 0 );
	if( is_sonora ) { // $BD482
		machine->asc->write( ASC_REG::A_IRQ_CTL, 0 );
		machine->asc->write( ASC_REG::B_IRQ_CTL, 0 );
	}
}

// $45E3C
void wait_until_easc_fifo_half() {
	while( check_easc_fifo_half() == 1 ) { // $BD41E
		SDL_Delay(0);
	}
}

// $45EA2
void easc_beep(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4, int16_t volume) {
	static uint8_t a5_v[256];
	if( a5_v[0] == 0 ) {
		FILE* a5p = fopen(DATA_PATH "beep.bin", "rb");
		fread(a5_v, 256, 1, a5p);
		fclose(a5p);
	}
	uint16_t vs[] = { v2, v1, v4, v3 };	
	uint32_t ws[] = { 0, 0, 0, 0 };
	for(;;) {
		int snd = 0;
		for(int i = 0; i < 4; ++i ) {
			ws[i] += (vs[i] ? vs[i] : 0x10000);
			snd += ( a5_v[ uint8_t(ws[i] >> 16) ] - 0x80);
		}
		// $45F16
		int v = (snd * volume) >> 16;
		machine->asc->write(0, v + 0x80);
		machine->asc->write(0x400, v + 0x80);
		switch( check_easc_fifo_empty(  ) ) {
		case 0 :
		case 2 :
			break;
		case 1 :
		case 3 :
			return;
		}	
	} 
}


// $BD3EC
int check_easc_fifo_empty() {
	uint8_t version = machine->asc->read(ASC_REG::VERSION);
	int v = machine->asc->read(ASC_REG::FIFO_IRQ_STATUS);
	if( version == 0xBC || version == 0xBB ) {
		// SONORA or Ardbeg(?)(2 buffer)
		if( (v & 8) == 0 ) {
			return 0;
		} else {
			return 1;
		}
	} else {
		// 1 buffer
		if( (v & 2) == 0 ) {
			return 2;
		} else {
			return 3;
		}
	}
}
// $BD41E
int check_easc_fifo_half() {
	int x = machine->asc->read(ASC_REG::FIFO_IRQ_STATUS) & 4;
	if( x == 0 ) {
		x = machine->asc->read(ASC_REG::FIFO_IRQ_STATUS) & 1;
		if( x == 0 ) {
			return 1;
		} else {
			return 2;
		}
	} else {
		return 0;
	}
}

// $BD494
int check_easc_fifo_done() {
	uint8_t version = machine->asc->read(ASC_REG::VERSION);
	if( version == 0xBC || version == 0xBB ) {
		DR(0) = machine->asc->read(ASC_REG::FIFO_IRQ_STATUS) & 0xc;
		// SONORA or Ardbeg(?)(2 buffer)
		if( DR(0) == 0xc ) {
			return 4;
		}
	}
	return machine->asc->read(ASC_REG::FIFO_IRQ_STATUS) & 0x3;
}
// $BD43A/BD44C/BD45E/BD470/BD482
bool easc_is_sonora() {
	uint8_t version = machine->asc->read(ASC_REG::VERSION);
	return (version & 0xf0) == 0xB0; // is sonora/Plance EASC
}


}
static void check_easc() {
	assert( ROM::test_machine_register( machine->asc, AR(3) ) );
	assert( ROM::test_machine_register( machine->via1, AR(5) ) );
}
namespace ROMWrapper {
using namespace ROM;
void run_45C1C() {
	check_easc();
	DR(0) = 1;
	play_chime_1();
}
void run_45C52() {
	check_easc();
	DR(0) = 4;
	play_chime_4();
}
void run_45C64() {
	assert( test_machine_register( machine->asc, AR(3) ) );
	AR(2) = AR(6);
	AR(6) = AR(3);
	play_easc(DR(0));
	AR(3) = AR(6);
	cpu.PC = AR(6) = AR(2);	
}

void run_45E3C() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	wait_until_easc_fifo_half();
	cpu.PC = AR(1);
}
void run_45EA2() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	easc_beep(DR(2) >> 16, DR(2) & 0xffff, DR(4) >> 16, DR(4) & 0xffff, DR(5) & 0xffff); 
	cpu.PC = AR(1);	
}


void run_BD3EC() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	switch( check_easc_fifo_empty()) {
	case 0 :
		cpu.PC = 0x40845eac;		
		break;
	case 1 :
		cpu.PC = 0x40845F3A;
		break;
	case 2 :
		cpu.Z = true;
		cpu.PC = 0x40845F36;
		break;
	case 3 :
		cpu.Z = false;
		cpu.PC = 0x40845F36;
		break;
	}
}
void run_BD41E() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	switch( check_easc_fifo_half() ) {
	case 0:
		cpu.PC = 0x40845E46;
		break;
	case 1:
		cpu.Z = true;
		cpu.PC = 0x40845E44;
		break;
	case 2:
		cpu.Z = false;
		cpu.PC = 0x40845E44;
		break;
	}
}
void run_BD494() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	int v = check_easc_fifo_done();
	switch( v ) {
	case 4 :
		DR(0) = 0xC;
		cpu.Z = true;
		cpu.PC = 0x40845E1E;
		return;
	default :
		DR(0) = v;
		cpu.PC = 0x40845E18;
	}
}
void run_BD43A() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	cpu.Z = ROM::easc_is_sonora();
	cpu.PC = 0x40845C7C;
}
void run_BD44C() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	cpu.Z = ROM::easc_is_sonora();
	cpu.PC = 0x40845CAE;
}
void run_BD45E() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	cpu.Z = ROM::easc_is_sonora();
	cpu.PC = 0x40845D36;
}
void run_BD470() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	cpu.Z = ROM::easc_is_sonora();
	cpu.PC = 0x40845E0E;
}
void run_BD482() {
	assert( test_machine_register( machine->asc, AR(6) ) );
	cpu.Z = ROM::easc_is_sonora();
	cpu.PC = 0x40845E2C;
}


}
