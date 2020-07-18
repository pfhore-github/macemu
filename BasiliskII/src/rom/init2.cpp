#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "mac_via.h"
#include "via.hpp"
#include "via1.hpp"
#include "asc.hpp"
#include "prototype.hpp"
#include "assert.h"
#include "adb.h"
#include "scc.hpp"
#include "machine.hpp"
constexpr uint8_t ADB_CMD = 0;
constexpr uint8_t ADB_DATA_EVEN = 1;
constexpr uint8_t ADB_DATA_ODD = 2;
constexpr uint8_t ADB_IDLE = 3;
extern std::atomic<bool> adb_interrupt;
extern std::atomic<uint8_t> state ;
void adb_set_state(int b);
namespace ROM {
void run_rom(uint32_t addr);
// $47752
void getHWInfo2_(int d2) {
	cpu.intmask = 7;
	return getHWInfo(d2);
}
struct scc_data_t {
	uint16_t cnt;
	uint16_t addr;
	uint8_t data2[17];
};
scc_data_t scc_data = {
	17, 0x7fee,
	{
		0xa9, 0x81, 0x8d, 0x30, 0xf0, 0xa9, 0x44, 0x8d,
		0x31, 0xf0, 0x80, 0xfe, 0xee, 0x7f, 0xee, 0x7f,
		0xee
	}
};


void _4AA98();
bool _AA0E0();
// $47EF6
int test_scc_iop() {
	if( ! std::dynamic_cast<IOP*>(&*machine->scc) ) {
		return 1;
	} else {
		return 0;
	}
#if 0
	auto scc_iop = get_machine_register<IOP>(AR(3));
	scc_iop->write( IOP_REG::CTRL, IOP_INT0 | IOP_INT1 | IOP_AUTOINC );
	scc_iop->write( IOP_REG::ADDR_HI, scc_data.addr >> 8 );
	scc_iop->write( IOP_REG::ADDR_LO, scc_data.addr & 0xff );
	for( int i = 0; i < scc_data.cnt; ++i ) {
		scc_iop->write( IOP_REG::RAM, scc_data.data2[i]);
	}
	scc_iop->write( IOP_REG::ADDR_HI, scc_data.addr >> 8 );
	scc_iop->write( IOP_REG::ADDR_LO, scc_data.addr & 0xff );
	for(int i = 0; i < scc_data.cnt; ++i) {
		if( scc_data.data2[i] != scc_iop->read( IOP_REG::RAM ) ) {
			return scc_data.cnt -i + 1;
		}
	}
	scc_iop->write( IOP_REG::CTRL, IOP_INT0 | IOP_INT1 | IOP_AUTOINC | IOP_RUN);
	return 0;
#endif
}




void _478A4();
// $A9FFC
void init_adb2(uint16_t r0_1, uint16_t r0_2, int r1, int r2);
// $470F4
bool ex_handler(int vec) {
	switch( e.vec) {
	case 0 :
		// RESET; $4727A
		boot_second();
		return false;
	case 1 :
		// Accessfault; $470F4
		if( INIT_FLAGS.test( INIT_FLAG_T:FAULT_SKIP ) ) {
			AR(7) = AR(5);
			return true;
		}
		EXCEPTION_STATUS |= 0x100;
		break;
	case 2 : // AddressError; $47104
	case 3 : // IllegalInst; $4710A
	case 4 : // DivBy0; $47110
	case 5 : // CHECK; $47116
	case 6 : // TRAPxx; $4711C
	case 7 : // Privilegeviolation; $47122
	case 8 : // Trace; $47128
	case 9 : // A-Trap; $4712E
	case 10 : // F-Trap; $47134
	case 12 : // Coop-Error; $47140
	case 13 : // Format Error; $47146
		EXCEPTION_STATUS |= (e.vec << 8 );
		break;
		// Uninitialized/Sperious IRQ; $4714C
	case 14 :
	case 23 :
		EXCEPTION_STATUS |= 0x0E00;
		break;
		// TRAP #0-#15; $47152
	case 31 : case 32 : case 33: case 34: case 35: case 36:
	case 37 : case 38 : case 39: case 40: case 41: case 42:
	case 43 : case 44 : case 45: case 46:
		EXCEPTION_STATUS |= 0x1000;
		break;							
	case 24 : // IRQ #1; $47158
	case 25 : // IRQ #2; $4715E
	case 26 : // IRQ #3; $47164
	case 27 : // IRQ #4; $4716A
	case 28 : // IRQ #5; $47170
		EXCEPTION_STATUS |= ( e.vec - 8);
		break;
	case 29 : // IRQ #6; $47178
		EXCEPTION_STATUS |= 0x1500;
		getHWInfo2(0);
		machine->via2->set( VIA_R::DDRB, VIA2_PORT_B::POWER_OFF);
		machine->via2->clear( VIA_R::DDRB, VIA2_PORT_B::POWER_OFF);
		for(;;) {
			SDL_Delay(0);
		}
		break;
	case 30 : // NMI IRQ; #47198
		if( INIT_FLAGS.test( INIT_FLAG_T::FAULT_SKIP ) ) {
			if( DR(0) == 0x50474370 ) { // some magic code?
				AR(7) = AR(5);
				DR(1) = machine->via1->read( VIA_REG::ORB);
				bool z = machine->via1->test( VIA_REG::DIRB, VIA1_PORTB::SOUND_ENABLE );
				machine->via1->clear( VIA_REG::DIRB, VIA1_PORTB::SOUND_ENABLE ) ;
				if( z ) {
					DR(1) = machine->via1->write( VIA_REG::ORB );
					machine->via1->set( VIA_REG::DIRB, VIA1_PORTB::SOUND_ENABLE ) ;
				}
				if( DR(1) & 1 << VIA1_PORTB::SOUND_ENABLE ) {
					machine->via1->set( VIA_REG::ORB, VIA1_PORTB::IICI_PARITY_ENABLE );
					machine->via1->read( VIA_REG::ORB );
					machine->via1->reset( VIA_REG::ORB, VIA1_PORTB::IICI_PARITY_ENABLE );
					machine->via1->read( VIA_REG::ORB );
					break;										
				} 
			} else if( DR(0) == 0x52505570 ) {
				// D471D2
				AR(7) = AR(5);
				// OSS?
				if( machine->oss ) {
					DR(1) = machine->oss->read16( 0x202 );
					if( DR(1) & 1 << 14 ) {
						machine->exp1->write16( 0x10, 0xffff );
						DR(1) = machine->exp1->read32(  );
						DR(1) = machine->exp1->write32( machine->exp1->read32() | 1 << 16 );
						break;
					}
				}
			}
		}
		// $471FC
		EXCEPTION_STATUS &= 0xff;
		EXCEPTION_STATUS |= 0x1600;
		INIT_FLAGS.set( 25 - 16);
		break;
	case 47 : // FP Unnorder; $4720A
	case 48 : // FP Inexact; $47214
	case 49 : // FP Div0; $4721E
	case 50 : // FP Underflow; $47228
	case 51 : // FP Invalid; $47232
	case 52 : // FP Overflow; $4723C
	case 53 : // FP SNAN; $47246
	case 55 : // 68030 MMU ERROR#1; $47250
	case 56 : // 68030 MMU ERROR#1; $4725A
	case 57 : // 68030 MMU ERROR#1; $47264
		EXCEPTION_STATUS &= 0xff;
		EXCEPTION_STATUS |= (e.vec - 24) << 8;
		break;
		// (unused;VEC12, 16-23, 59-63); $4713A
	default:
		EXCEPTION_STATUS |= 0x0A00;
		break;
							
	}
	// $4726C
	INIT_FLAGS.set( 24 - 16 );
	uint32_t sp = AR(7);
	AR(7) = 0x2600;
	sad_mac( sp );
	return false;
}

// $4727A
void boot_second() {
	AR(7) = 0x2600;
	uint32_t a1 = rom_base == 0x40A00000 ? 0x46FF0 : 0x470F;
	// vbr_ptr2
	cpu.VBR = rom_base | a1;
	getHWInfo2(0);				// $47752
	// $472A4
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::MASTER2 )) {
		try {
			// TODO
			uint32_t a1 = motherboard->secret;
			uint32_t d3 = read_l(a1);
			uint32_t a0 = read_l(a1+4);
			if( d3 == 0xaaaa5555 ) {
				AR(3) = 0x40800000 + 0xBAA4E;
				run_rom(a0);
				// $470DA
				getHWInfo2(0);
			}
		} catch( ... ) {}
	}
	
	// $472E0
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::IOP_SCC) ) {
		if( test_scc_iop() ) {				  // $47EF6
			// $4AA98
			return sad_mac(0);
		}
	}
	// $47304
	getHWInfo2(0);
	uint8_t f = (ROM_FLG.to_ulong() >> 24) & 7;
	switch( f ) {
	case 1 :
	case 2 :
		// $47358 (unknown machine?)
		run_rom(0x14C50);
		cpu.R[0] = 1;
		cpu.R[1] = 0;
		cpu.R[2] = 0;
		run_rom(0x14DC6);
		if( cpu.R[0] == 0) {
			cpu.R[0] = 0x001C0001;
			cpu.R[2] = 1;
			cpu.R[1] = 1;
			run_rom(0x14CCC);
		}
		break;
	case 3:
		// $47330; Cuda ADB?
		init_adb();  // $85714
		init_adb2(0x0026,0x0001, 1, 1);
		break;
	}
	// $47398
	getHWInfo2(0);
	cpu.R[8+2] = motherboard->via1_addr;
	uint32_t via_base = motherboard->via1_addr;
	if( GEN == 6 ) {
		// IIfx only
		if( read_b(motherboard->oss_addr + 0x0206) & 2) {
			//		goto _47490;
		} else {
			// goto _4746E;
		}
	} else {
		// $473BE
		cpu.R[2] >>= 8;
		uint8_t gest_id = GID;
		if( gest_id > 13 ) {
			// newer then PB140
			do {
				uint32_t a3 = self + 0x14e + read_rom16(self + 0x14e);
				cpu.R[3] = read_rom8(a3++);
				if( cpu.R[3] == 0xfd ) {
					goto _473EC;
				}
			} while( gest_id != cpu.R[3] );
		}
		// $473D6
		// ???
		machine->via1->clear(vDirA, 0);
		machine->via1->clear(vBufA, 1);
		if( ! machine->via1->bit(vBufA, 0) ) {
			goto _4746E;
		}
	  _473EC:
		cpu.R[3] = 0x007800f8;
		do {
			_478A4();
			if( cpu.R[4] != 0x53434249 ) {
				cpu.R[3] >>= 16;
			} else {
				// $4740C
				cpu.R[2] = 0;
				run_rom(0x47752);
				cpu.R[8+2] = via_base;
				if( INIT_HW_FLG.test( 8) ) {
					cpu.R[3] = read_b(motherboard->scsi_addr + 64) & 0x28;
					if( cpu.R[3] != 0x10 ) {
						// $47450
					} else {
						// $4746E
					}
				} else {
					// $47434
					
				}
			}
		} while( cpu.R[3] );
		goto _47490;
	  _4746E: {}
		// $4746E
	  _47490:
		// goto 474BA
		// $474BA
		cpu.R[2] = 0;
		getHWInfo2(0);
		cpu.R[3] = model->unknown_48; // TODO: ???
		if( cpu.R[3] ) {
			// newer machine
		}
		// $474DA
		cpu.PC = 0x408474BA;
		return;
	}
}

// $7052
void play_asc_0() {
	cpu.R[8+4] = 0x40807158;
	play_asc();
}

void _45C64();


#if 0
// $40000-
void _7052(uint32_t asc_base, uint32_t via_base) {
	return init_asc(_7178);
}


void _4A646();

// $470F4-070FC
static void access_fault_v(uint32_t d7) {
	if( INIT_FLAGS.fault_skip ) {
		return;
	}
	INIT_FLAGS.ex_num = 1;
	INIT_FLAGS._24 = true;
	cpu->R[15] = 0x9728;
	return _4A616();
}
// $47178
static void irq6_v(uint32_t d7) {
	INIT_FLAGS.ex_num = 31;
	getHWInfo2(0);
	// power off
	BSET(GEN_MAP->via2_base + vdirB, VIA2B_vPower);
	BCLR(GEN_MAP->via2_base + vdirB, ~VIA2B_vPower);
	return;
}
void _4A616() {
	AR(7) = 0x2600;
	do {
		getHWInfo2(0);
	} while( INIT_FLAGS._26 );
	_45C1C(GEN_MAP->asc_base, GEN_MAP->via_base1);
	return _B9886();
}
// $47198
static void nmi_v() {
	if( INIT_FLAGS.fault_skip ) {
		if( *(uint32_t*)(&ADDR_FLG) == 0x50474370 ) {
			uint8_t d1 = read_b(GEN_MAP->via1_base + vbufB);
			if( BCLR(GEN_MAP->via1_base + vdirB, VIA1B_vSound)) {
				d1 = read_b(GEN_MAP->via1_base + vbufB);
				BSET(GEN_MAP->via1_base + vdirB, VIA1B_vSound);
			}
			if( !(d1 & VIA1B_vSound )) {
				BSET(GEN_MAP->via1_base + vbufB, VIA1B_vMystery);
				read_b(GEN_MAP->via1_base + vbufB);
				BCLR(GEN_MAP->via1_base + vbufB, VIA1B_vMystery);
				read_b(GEN_MAP->via1_base + vbufB);
				return;
			}
		} else if( ADDR_FLG.value == 0x52505570 ) {
			uint32_t a1 = GEN_MAP->oss_base;
			uint16_t d1 = read_w(GEN_MAP->oss_base + offsetof(mac_oss, irq_pending));
			if( d1 & OSS_IP_PARITY ) {
				a1 = GEN_MAP->via6_addr;
				write_b(a1+0x10, 0xff);
				write_l(a1, read_l(a1) | 1 << 16);
				return;
			}
		}
	}
	// $47004
	INIT_FLAGS.ex_2 = 0xff;
	INIT_FLAGS.ex_num = 22;
	INIT_FLAGS._25 = true;
	// $47074
	INIT_FLAGS._24 = true;
	cpu->A[7] = 0x9728;
	return _4A616();
}



void emul_reset(uint32_t* d, uint32_t* a);
void _45C64(int d0);
extern std::atomic<uint8_t> state ;

void _45C2E(uint32_t a3, uint32_t a5) {
	d0 = 2;
	if( read_b(a3 + ASC_REG::VERSION ) == 0 ) {
		// $705E
		cpu.R[8+2] = 0x7198;
	}
	_45C64(2);
}
void _45C40(uint32_t a3, uint32_t a5) {
	if( read_b(a3 + ASC_REG::VERSION ) == 0 ) {
		// $7064
		return init_asc(_71AC);
	}
	_45C64(3);
}

#endif

bool _AA0E0() {
	// reset ADB state
	adb_set_state(ADB_IDLE);
	cpu.R[7] = 48;
	cpu.R[6] = 1;
	 _4AA98();
	 return true;
//	cpu.PC = 0x408aa0f2;
}

// $AA1A2
void test_vbufB() {
	machine->via1->read(vBufB);
	SDL_Delay(0);
}

void _A9FFC();
bool _AA11E() {
	test_vbufB();
	machine->via1->clear(vACR, 4);
	machine->via1->read(vSR);
	adb_set_state(1);
	for(;;) {
		if( machine->via1->read(vSR) ) {
			break;
		}
		adb_set_state(ADB_CMD);
	}
	adb_set_state(ADB_IDLE);		
	bool v = machine->via1->read(vSR);
	_A9FFC();
	return v;
}
#include <stdio.h>

#if 0
bool _AA00C() {
	adb_set_state(3);
	via1->read(vSR);
	cpu.R[4] = cpu.R[4] << 16 | cpu.R[4] >> 16;
	if( _AA0B0() ) {
		return _AA106();
	}
	cpu.R[0] = cpu.R[0] << 16 | cpu.R[0] >> 16;
	talk_adb();
	cpu.R[0] = cpu.R[1];
	if( (uint16_t)--cpu.R[2] <= 3 ) {
		do {
		talk_adb();
		cpu.R[0] >>= 8;
		} while( (int16_t)(--cpu.R[2]) >= 0 );
	}
	via1->clear(vACR, 4);
	adb_set_state(3);
	skip_sr();
	adb_set_state(1);
	_AA182();
	cpu.R[0] &= 0xffff00ff;
	_AA182();
	cpu.R[0] &= 0xffff00ff;
	cpu.R[0] = cpu.R[0] << 16 | cpu.R[0] >> 16;
	uint32_t tmp = cpu.R[1];
	cpu.R[1] = cpu.R[0];
	cpu.R[0] = tmp;
	_AA182();
	adb_set_state(3);
	assert(via1->bit(vIFR, 2));
	cpu.R[0] = cpu.R[1];
	if( cpu.R[0] != 2 ) {
		cpu.R[0] = 0;
	}
	cpu.Z = true;
	return true;
}
#endif
bool adb_skip_1_byte(uint16_t r0, uint16_t r4);
void _AA0B0() {
	cpu.Z = adb_skip_1_byte(cpu.R[4], cpu.R[4] >> 16);
}
// $AA0B0
bool adb_skip_1_byte(uint16_t r0, uint16_t r4) {
	adb_talk(r0);
	adb_set_state(1);
	machine->via1->read(vBufB);
	machine->via1->read(vSR);
	return true;
}

// $AA15C
void talk_adb(uint8_t v) {
	adb_talk(v);
	state = 0;
}

// $AA194
void skip_sr() {
	machine->via1->read(vSR);
}
void _AA15C() { talk_adb(cpu.R[0]); }
uint8_t read_adb1();
void _AA182() {
	cpu.R[0] = read_adb1();
}
void _A9FFC() {
	init_adb2(cpu.R[0] >> 16, cpu.R[0], cpu.R[1], cpu.R[2]);
}

namespace ROMWrapper {
using namespace ROM;

}
