#include "data.hpp"
#include "prototype.hpp"
#include "via1.hpp"
#include "rbv.hpp"
#include "jaws.hpp"
#include "msc.hpp"
#include "powerbook.hpp"
#include "machine.hpp"
#include "io.hpp"
#include "intel.h"
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
// non-MSC
// (00/80-FF)xx : 1byte
// xx01xx: 3byte
// xxxx(02-7F)xx: 4byte
bool send_to_pb_dword(uint32_t data) {
	bool is_msc = false;
	// send 1st byte
    send_to_pb_byte(data, &is_msc); // $478C2
	int8_t r3_2 = int16_t(data) >> 8;
	// if 15bit of cmd is 1, 1byte cmd
	if( r3_2 < 0 ) {
		return true;
	}
	uint16_t r3_h = data >> 16;
	if( ! is_msc ) { // no MSC
		send_to_pb_byte(r3_2); // $478C2
	}
	// if cmd is 00XX, 1byte cmd
	if( r3_2 < 1 ) {
		return true;
	}
	// send 3rd byte
	send_to_pb_byte(r3_h & 0xff); // $478C2
	// yyxx01xx; 2-byte
	if( r3_2 < 2 ) {
		return true;
	}
	return send_to_pb_byte( r3_h >> 8); // $478C2
}

// $478C2
bool send_to_pb_byte(uint8_t data, bool* is_msc) {
	bool msc = motherboard->rom_flg.test( ROM_FLG_T::MSC );
	if( is_msc ) {
		*is_msc = msc;
	}
	auto via2 = machine->get_via2();
	if( msc ) {
		machine->via1->set( VIA_REG::ACR, 4 ); // SR is shift out mode
		machine->via1->write( VIA_REG::SR, data );
	} else {
		via2->write( VIA_REG::DDRA, 0xff ); // all bits are write
		via2->write( VIA_REG::RA_H, data );
	}
	bool ret = false;
	via2->clear( VIA_REG::RB, PB_TRANS_READ_MODE );
	if( ! via2->bit( VIA_REG::RB, PB_TRANS_READY ) ) {
		via2->set( VIA_REG::RB, PB_TRANS_READ_MODE );
		if( via2->bit( VIA_REG::RB, PB_TRANS_READY ) ) {
			ret = true;
		}
	}
	// $47918
	via2->set( VIA_REG::RB, PB_TRANS_READ_MODE );
	
	if( ! msc ) {
		via2->write( VIA_REG::DDRA, 0 );
	}
	return ret;
}

// $47928
std::optional<uint8_t> recv_from_pb(bool* is_msc) {
	std::optional<uint8_t> ret;
	bool msc = motherboard->rom_flg.test( ROM_FLG_T::MSC );
	if( is_msc ) {
		*is_msc = msc;
	}
	auto via2 = machine->get_via2();
	if( msc ) {
		machine->via1->clear( VIA_REG::ACR, 4 ); // SR is shift in mode
		machine->via1->read( VIA_REG::SR ); // skip old value
		via2->clear( VIA_REG::RB, PB_TRANS_READ_MODE ); // disable read mode
		if( ! via2->bit( VIA_REG::RB, PB_TRANS_READY ) ) {
			via2->set( VIA_REG::RB, PB_TRANS_READ_MODE ); // set read mode
			if( via2->bit( VIA_REG::RB, PB_TRANS_READY ) ) {
				ret = machine->via1->read( VIA_REG::SR );
			}
		}
	} else {
		// $47976
//		via2->write( VIA_REG::DDRA, 0xff ); // all bits are write
//		via2->write( VIA_REG::RA_H, data );
	}
	return ret;
}
uint16_t _B35EC(uint16_t v) {
	
	return v;
}
uint16_t _B358C(uint16_t d1) {
	d1 &= 0xff;
	d1 = (int16_t)d1 >> 3;
	d1 = (d1 & 0xff00) | ROR_B(d1&0xff, 1);
	d1 = ROR_W(d1, 8);
	d1 |= 0xb8;
	d1 = _B35EC(d1);
	return d1 & 0xff;
}
uint16_t _B375E(uint16_t d1) {
	uint32_t d3 = ((d1 & 0xff) << 16) | 0x0100023a; // $01'XX'02'3A
	send_to_pb_dword( d3 );
	auto d3_v = recv_from_pb();
	if( ! d3_v )
		return 0;
	if( *d3_v == 0x3a ) { // $47928
		if( ! recv_from_pb() ) {
			return 0;
		}
	}
	return recv_from_pb().value_or(0);
}
uint16_t _B381E(uint16_t d1) { return d1; }
uint16_t _B3AB8(uint16_t d1) { return d1; }
uint16_t rom650_B3566(uint16_t d1) { return d1; }

uint16_t _4B53C(uint16_t d1) {
	if( model->unknown_52->handler4 ) {
		return model->unknown_52->handler4(d1);
	}
	return d1;
}
// A0, A2, D1
uint8_t _8ADA6(uint32_t base, const uint8_t* o2, uint8_t v) {
	uint8_t d2 = read_b(base);
	for(int i = 0; i < v; ++i ) {
		write_b(base, o2[i]);
	}
	return d2;
}
static const uint8_t _8AD4C[] = {	
	0x9, 0xC0, 0x9, 0x40, 0x04, 0x4C, 0x02, 0x00, 0x03, 0xC0, 0x0F, 0x00, 0x00, 0x1, 0x01, 0x00
};
void _8AD6C() {
	// Nubus STD $E?
	if( read_l( 0xFEFFFFE4 ) != OSTYPE("Russ") ) { // 0x52757373
		return;
	}
	if( read_l( 0xFEFFFFE8 ) != OSTYPE("SWC2") ) { // 0x53574321
		return;
	}
	write_b( 0xFEE00021, read_b( 0xFEE00021) | 2 );
	DR(2) = _8ADA6(0xFEE08000,  _8AD4C, 18 );
}
// $8AE4A
void read_check_1(uint32_t a0) {
	read_b(a0+1);
}
// $8AC00
void init_powerbook() {
	if( ! HWCFG_FLAGS.test(HWCFG_T::POWERBOOK) ) {
		return;
	} 
	uint32_t a2 = read_l(0xD18);	 // unknwon low level global
	if( read_l(a2+0x112) == OSTYPE("SARA") ) { // 0x53415241 
		write_l(a2+0x112, 0);
		run_rom(a2+0x12);
	}
	init_pb_duo();				// $B29C4
	getHWInfo(0);				// $02F64
	uint8_t bpp = 0;			// %D1
	if( _4B53C(0x10) == 0xA8 ) {
		bpp = _4B53C( model->unknown_60 ? *model->unknown_60 + 3 : 0x73 ) & 2;
	} 
	// $8AC96
	switch( GEN ) {
	case 0xA :					// MSC
	{
		// $8ACF4
		// set BPP
		char d2 = machine->pb->read16( 0x44) & 7; 
		if( (bpp & 2) || (d2 != 5 && d2 != 2 ) ) {
			bpp |= 1;
		}
		machine->rbv->write( rMonP, ( machine->rbv->read( rMonP) & (~7)) | bpp);
		send_to_pb_dword( 0x171 );
		INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
		try {
			_8AD6C();
		} catch( ... ) {
		}
		INIT_FLAGS.reset( INIT_FLAG_T::FAULT_SKIP );
		break;
	}
	case 0x9:					// Jaws
		if( ! machine->pb->bit( 0x34000, 0 ) ) {
			break;
		}
		/* fall through */
	case 0xC: 					// Niagara
		if( ( machine->pb->read( 0x4000 ) & 2 ) == bpp ) {
			break;
		}
		machine->pb->write( 0x4000, bpp );
		send_to_pb_dword( 0xD0 );
		break;
	default :
		break;
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
		send_to_pb_dword( 0xEA0003E0 );
		send_to_pb_byte( 0xA );
	}
	// $8AE14
	try {
		read_check_1( motherboard->vdac_addr );
	} catch( ... ) {
	}
	return;
}


// $B29C4
void init_pb_duo() {
	// $B2A7C
	constexpr uint16_t V1 = 0x000A; /* gen 0A(powerBook Duo) */
	constexpr uint32_t V2 = 0x8004;
	FILE* fp = fopen(DATA_PATH "pb_firmware.bin", "rb");
	std::vector<uint8_t> C_AA9C0(V2);
	fread(C_AA9C0.data(), V2, 1, fp);
	fclose(fp);
	constexpr uint32_t V3 = 0x408AA9C0;
	getHWInfo(0);				// $02F64
	if( GEN != V1 ) {
		return;
	}

	DR(1) = V2;
	AR(1) = V3;
	send_to_pb_dword( 0xE2 ); // $47840
	// recv from ???(if already initalized $62)
	if( recv_from_pb().value_or(0) == 0x62 ) { // $47928
		return;
	}
	send_to_pb_dword( 0xE1 ); // $47840

	// send firmware?
	for( uint8_t d3 : C_AA9C0 ) {
		send_to_pb_byte( d3 );	 // $478C2
	}
	
	send_to_pb_dword( 0xE2 ); // $47840

	if( recv_from_pb().value_or(0) != 0x62 ) {
		AR(7) = 0x2600;
		EXCEPTION_STATUS = 16;
		sad_mac(0);
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
	cpu.PC = AR(6);
}

void run_B29C4() {
	init_pb_duo();
	cpu.PC = AR(6);
}


}
