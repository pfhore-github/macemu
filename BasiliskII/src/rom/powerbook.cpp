#include "data.hpp"
#include "prototype.hpp"
#include "via1.hpp"
#include "rbv.hpp"
#include "jaws.hpp"
#include "machine.hpp"
#include "io.hpp"
#include <assert.h>
static constexpr int rMonP = 0x0010;  /* [RBV only] RBV video monitor type. */
namespace ROM {
// $47826
bool send_to_pb_key() {
	if( motherboard->rom_flg.test( ROM_FLG_T::MONITOR) ) {
		machine->rbv->set(RBV_REG::PB_RBV_X33, 1); 
	}
	return send_to_pb_dword(0x00B30110);
}

// $47840
// cmd: MSC
// (00/80-FF)xx : 1byte
// xx01xx: 2byte
// xxxx(02-7F)xx: 3byte
// non-MAC
// (00/80-FF)xx : 1byte
// xx01xx: 3byte
// xxxx(02-7F)xx: 4byte
bool send_to_pb_dword(uint32_t data) {
	auto via2 = machine->get_via2();
	machine->via1->read(VIA_REG::RB);
	if( ! machine->via2->bit(VIA_REG::RB, TRANS_READY) ) {
		return false;
	}
	bool is_msc = false;
	// send 1st byte
    bool ret = send_to_pb_byte(data, &is_msc); // $478C2
	if( ! ret ) {
		// if failure stop it
		return false;
	}
	int8_t r3_2 = int16_t(data) >> 8;
	// if 15bit of cmd is 1, 1byte cmd
	if( r3_2 < 0 ) {
		return true;
	}
	uint16_t r3_h = data >> 16;
	if( ! is_msc ) { // no MSC
		ret = send_to_pb_byte(r3_2); // $478C2
		if( ! ret ) {
			// if failure, stop it.
			return false;
		}
	}
	// if cmd is 00XX, 1byte cmd
	if( r3_2 < 1 ) {
		return true;
	}
	// send 3rd byte
	ret = send_to_pb_byte(r3_h & 0xff); // $478C2
	if( ! ret ) {
		// if failure, stop it.
		return false;
	}
	// yyxx01xx; 2-byte
	if( r3_2 < 2 ) {
		return true;
	}
	return send_to_pb_byte( r3_h >> 8); // $478C2
}

// $478C2
bool send_to_pb_byte(uint8_t data, bool* is_msc) {
	auto via2 = machine->get_via2();
	if( motherboard->rom_flg.test( ROM_FLG_T::MSC ) ) {
		if( is_msc ) {
			*is_msc = true;
		}
		// send via SR
		machine->via1->set(VIA_REG::ACR, 4 ); // shift out mode
		machine->via1->write(VIA_REG::SR, data); // transport data to 68HC05(IOP?)
	} else {
		// $478E6
		if( is_msc ) {
			*is_msc = false;
		}
		// send via port A
		via2->write(VIA_REG::DDRA, 0xff); // all bit is out
		via2->write(VIA_REG::RA_H, data);
	}
	
	// $478F0
	
	bool ret = false;
	via2->clear(VIA_REG::RB, TRANS_STANDBY);
	machine->via1->read(VIA_REG::RB);

	if( ! via2->bit(VIA_REG::RB, TRANS_READY) ) { 
		via2->set(VIA_REG::RB, TRANS_STANDBY);	   
		machine->via1->read(VIA_REG::RB);
		if( via2->bit(VIA_REG::RB, TRANS_READY) ) { 
			ret = true;
		}
	}
	// $47918
	via2->set(VIA_REG::RB, TRANS_STANDBY);
	via2->write(VIA_REG::DDRA, 0); // all bit is in...
	return ret;
}

// $47928
std::optional<uint8_t> recv_from_pb(bool* is_msc) {
	auto via2 = machine->get_via2();
	std::optional<uint8_t> ret;
	if( motherboard->rom_flg.test( ROM_FLG_T::MSC ) ) {
		// $4793A
		if( is_msc ) {
			*is_msc = true;
		}
		// send command to IOP
		machine->via1->clear(VIA_REG::ACR, 4 ); // shift in mode
		machine->via1->read(VIA_REG::SR); // transport DR(3) from 68HC05(IOP?)
		via2->clear( VIA_REG::RB, 2 );
		machine->via1->read( VIA_REG::RB );
		if( ! via2->bit(VIA_REG::RB, TRANS_READY ) ) { 
			via2->set( VIA_REG::RB, 2);
			machine->via1->read( VIA_REG::RB );
			if( via2->bit(VIA_REG::RB, TRANS_READY ) ) { 
				ret = machine->via1->read( VIA_REG::SR );
			}
		}				
	} else {
		// JAWS
		if( is_msc ) {
			*is_msc = false;
		}

		// set A-port all read
		via2->write( VIA_REG::DDRA, 0 );
		// 
		if( ! via2->bit(VIA_REG::RB, 1) ) {
			machine->via1->read( VIA_REG::RB );
			via2->set( VIA_REG::RB, 2);
			ret = via2->read( VIA_REG::RA_H );
			if( via2->bit(VIA_REG::RB, 1) ) { 
				machine->via1->read( VIA_REG::RB );
			}
		}
	}
	// 479a0
	via2->set( VIA_REG::RB, 2);
	if( ROM_FLG.test( ROM_FLG_T::MSC ) ) {
		machine->via1->set(VIA_REG::ACR, 4);
	} else {
		machine->via1->write(VIA_REG::DDRA, 0);
	}
	// 479B4
	return ret;
}


// $8AC00
void init_powerbook() {
	if( ! HWCFG_FLAGS.test(HWCFG_T::POWERBOOK) ) {
		return;
	} 
	uint32_t a2 = read_l(0xD18);	 // unknwon low level global
	// TODO
	if( read_l(a2+0x112) == 0x53415241 /* SARA */ ) {
		write_l(a2+0x112, 0);
		run_rom(a2+0x12);
	}
	init_pb_duo();
	getHWInfo(0);
	uint32_t dr4 = DR(2);
	DR(1) = 16;
	AR(2) = motherboard->via1_addr;
	run_rom(0x4B53C);
	DR(0) = -88 - DR(1);
	DR(1) = 0;
	if( DR(0) == 0 ) {
		DR(1) = 115;
		AR(2) = model->unknown_60;
		if( AR(2) ) {
			DR(1) = read_rom8(AR(2)) + 3;					
		}
		// 8AC7C
		motherboard = model->motherboard;
		AR(2) = motherboard->via1_addr;
		run_rom(0x4B53C);
		DR(1) += 2;
	}
	DR(2) = dr4;
	motherboard = model->motherboard;
	if( (uint8_t)DR(2) != 12 ) {
		if( machine->pb->bit( 0x34000, 0 ) ) {
			DR(2) = machine->pb->read( 0x4000 ) & 2;
			if( DR(1) != DR(2) ) {
				machine->pb->write(0x4000, DR(1) );
				DR(2) = 208;
				run_rom( 0x47840 );
				SDL_Delay(100);
			}
		}
	} else {
		// 8ACEC
		if( DR(2) == 10 ) {
			DR(1) |= 1;
			if( DR(1) & 2 ) {
				DR(2) = model->machine_id & 7;
				if( DR(2) == 5 || DR(2) == 2 ) {
					DR(1) &= ~ 1;
				}
			}
			machine->rbv->write( rMonP, machine->rbv->read( rMonP ) &~ RBV_DEPTH );
			DR(3) = 0x171;
			run_rom( 0x47840 );
			INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
			run_rom( 0x8ad6c );
			INIT_FLAGS.reset( INIT_FLAG_T::FAULT_SKIP );					
		}
	}
	// $8ADC0
	getHWInfo(0);				// $02F64
	if( (uint8_t)DR(2) != 12 ) {
		if( (uint8_t)DR(2) != 10 ) {
			return;
		}
		machine->rbv->set( 0x33, 3 );
	} else {					
		// $8ADEE
		DR(3) = 0xEA0003E0;
		run_rom( 0x47840 );
		DR(3) = 10;
		run_rom( 0x478C2 );				
	}
	// $8AE14
	AR(0) = motherboard->vdac_addr;
	INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
	run_rom( 0x8ae4a );
	INIT_FLAGS.reset( INIT_FLAG_T::FAULT_SKIP );
	return;
}

static void init_pb_duo_fail() {
	AR(7) = 0x2600;
	EXCEPTION_STATUS = 16;
	sad_mac(0);
}

// $B29C4
void init_pb_duo() {
	// $B2A7C
	constexpr uint16_t V1 = 0x000A; /* gen 0A(powerBook Duo) */
	constexpr uint32_t V2 = 0x8003;
	FILE* fp = fopen(DATA_PATH "pb_firmware.bin", "rb");
	std::vector<uint8_t> C_AA9C0(V2);
	fread(C_AA9C0.data(), V2, 1, fp);
	fclose(fp);
	constexpr uint32_t V3 = 0x408AA9C0;
	getHWInfo(0);				// $02F64
	if( ( GID << 8 | GEN ) != V1 ) {
		return;
	}

	DR(1) = V2;
	AR(1) = V3;
	if( ! send_to_pb_dword( 0xE2 ) ) { // $47840
		return init_pb_duo_fail();
	}
	// recv from ???(must be $62)
	if( recv_from_pb().value_or(0) == 0x62 ) { // $47928
		return;
	}
	if( ! send_to_pb_dword( 0xE1 ) ) { // $47840
		return init_pb_duo_fail();
	}
	// send firmware?
	for( uint8_t d3 : C_AA9C0 ) {
		if( ! send_to_pb_byte( d3 ) ) {	 // $478C2
			return init_pb_duo_fail();
		}
	}
	if( ! send_to_pb_dword( 0xE2 ) ) { // $47840
		return init_pb_duo_fail();
	}

	if( recv_from_pb().value_or(0) == 0x62 ) {
		return;
	} else {
		init_pb_duo_fail();
	}
}

}
namespace ROMWrapper {
using namespace ROM;
void run_47826() {
	get_motherboard( AR(0) );
	cpu.Z = send_to_pb_key();
}

void run_47840() {
	get_motherboard( AR(0) );
	cpu.Z = send_to_pb_dword( DR(3) );
	if( motherboard ) {
		AR(2) = motherboard->via1_addr;
		AR(3) = motherboard->via2_addr ? motherboard->via2_addr : motherboard->rbv_addr;
	}
	cpu.PC = AR(6);
}

void run_478C2() {
	get_motherboard( AR(0) );
	bool msc;
	bool ret = send_to_pb_byte(DR(3), &msc);	
	DR(4) = msc << 31;
	DR(6) = ret ? 0 : 2;
	cpu.Z = ret;
	cpu.PC = AR(5);
}

void run_47928() {
	get_motherboard ( AR(0) );
	bool msc;
	auto ret = recv_from_pb(&msc);
	if( ret ) {
		cpu.Z = true;
		DR(6) = 0;
		DR(3) = *ret;
	} else {
		cpu.Z = false;
		DR(6) = 2;
		DR(3) = 0;
	}
	DR(4) = msc << 31;
	cpu.PC = AR(5);
}

void run_8AC00() {
	INIT_FLAGS = DR(7) >> 16;
	EXCEPTION_STATUS = DR(7);
	init_powerbook();
	DR(7) = INIT_FLAGS.to_ulong() << 16 | EXCEPTION_STATUS;
}

void run_B29C4() {
	init_pb_duo();
	cpu.PC = AR(6);
}


}
