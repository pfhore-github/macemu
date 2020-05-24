#include "common.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "io.hpp"
#include "iop.hpp"
#include "rbv.hpp"
#include "oss.hpp"
#include "via1.hpp"
#include "rbv.hpp"
#include "jaws.hpp"
#include "sonic.hpp"
#include "scc.hpp"
#include "mcu.hpp"
#include "swim.hpp"
#include <assert.h>
template<class T>
bool check_chip(const std::shared_ptr<T>& p, int offset = 0) {
	try {
		p->read(offset);
	} catch( ... ) {
		return false;
	}
	return true;
}
namespace ROM {
MB_TYPE mb_type;

// $30AC(GLU); II/IIx/IIcx/SE30; VIA1 is mirrored at $20000
bool check_glu() {
	return has_via( motherboard->via1_addr + 0x1c00, 0x20000 ); // $046AC
}
// $30CA(MDU); IIci/IIsi; VIA1 is mirrored at $40000 but not $20000
bool check_mdu() {
	uint32_t base = motherboard->via1_addr + 0x1c00;
	if( ! has_via( base, 0x40000 ) ) { // $046AC
		return false;
	}

	if( has_via( base, 0x20000 ) ) { // $046AC
		return false;
	}
	if( ! check_chip( machine->rbv ) ) {
		return false;
	}
	if( ! check_chip( machine->vdac ) ) {
		return false;
	}
	return true;
}

// $310A(OSS); IIfx
bool check_oss() {
	uint32_t base = motherboard->via1_addr + 0x1c00;
	if( ! has_via( base, 0x40000 ) ) { // $046AC
		return false;
	}
	if( has_via(base, 0x20000 ) ) { // $046AC
		return false;
	}
	if( ! check_chip( machine->oss ) ) {
		return false;
	}
	return true;
}

// $3166(V8); LC/IIvx/IIvi
// V8 is less documented...; no mirror @ $40000 & has RBV/VDAC
bool check_V8() {
	uint32_t base = motherboard->via1_addr + 0x1c00; /* vIER */
	if( ! has_via( base ) ) {	// $046AA
		return false;
	}
	if( has_via( base, 0x40000 ) ) { // $046AC
		return false;
	}
	if( ! check_chip( machine->rbv ) ) {
		return false;
	}
	if( ! check_chip( machine->vdac ) ) {
		return false;
	}
	return true;
}

// $31A0(MCU); Quadra
/* no document for MCU; has special register at $5000E000 */ 
bool check_mcu() {
	if( ! check_chip( machine->mcu ) ) {
		return false;
	}
	return true;
}

//$31AE(jaws); PowerBook
/* mirror at $100000 but neither $80000 nor $40000 and special register $50F80000 */
bool check_jaws() {
	uint32_t base = motherboard->via1_addr + 0x1c00; /* vIER */
	if( ! has_via(base, 0x100000) ) {				 // $046AC
		return false;
	}
	if( has_via(base, 0x80000) ) { // $046AC
		return false;
	}
	if( has_via(base, 0x40000) ) { // $046AC
		return false;
	}
 	if( ! check_chip( machine->pb)) {
		return false;
	}
	return true;
}

// $31F2(Niagara); powerbook 16x/180
// no mirror $80000/$40000 and special register $50F80000
bool check_niagara() {
	uint32_t base = motherboard->via1_addr + 0x1c00; /* vIER */
	if( has_via( base, 0xFF200000 ) ) { // $046AC
		return false;
	}
	if( has_via( base, 0x80000) ) { // $046AC
		return false;
	}
	if( has_via( base, 0x40000) ) { // $046AC
		return false;
	}
 	if( ! check_chip( machine->pb)) {
		return false;
	}
	return true;
}
// ??? $323A
bool check_null() {
	return true;
}
// $4640
// test FPU and return Z=1 if exist or Z=0 else
bool has_fpu() {
	return dynamic_cast<NoFPU*>(fpu) == nullptr;
}
// $4672
bool test_iifx_exp1(uint32_t base) {
	if(! test_machine_register( machine->exp1, base ) ) {
		return false;
	}

	try {
		// wait?
		for(int i = 0; i < 4; ++i ) {
			machine->exp1->read(0);
		}
		machine->exp1->write(0x10, 0xff);
		uint8_t d2 = machine->exp1->read(0);
		machine->exp1->write(0x10, 0xff);
		machine->exp1->write(0, 0xff);
		machine->exp1->write(0x10, 0);
		uint8_t d1 = machine->exp1->bit(0, 0);
		if( ! d1 ) {
			d1 = 0xff;
		} else {
			machine->exp1->write(0x10, 0xff);
			machine->exp1->write(0, 0);
			machine->exp1->write(0x10, 0xff);
			d1 = machine->exp1->bit(0, 0);
		}
		machine->exp1->write(0x10, 0xff);
		machine->exp1->write(0, d2);
		return d1 == 0;
	} catch( ... ) {
		return false;
	}
}
// $46AA(offset=0)
// $46AC; Z=1 iff w/ VIA @A2(D2)
bool has_via(uint32_t base, uint32_t offset) {
	return test_machine_register(machine->via1, base ) &&
		test_machine_register(machine->via1, base + offset );
}
// $46D8(A2=addr); Z=1 iff w/ RBV @A2
bool has_rbv(uint32_t base) {
	return test_machine_register(machine->rbv, base );
}
// $4700
bool test_scsi_io(uint32_t base) {	
	return test_machine_register(machine->scsi, base );
}
// $04708
bool test_scsi_cmd(uint32_t scsi_base) {
	if( ! test_machine_register(machine->scsi, scsi_base ) ) {
		return false;
	}
	return machine->scsi->read(0x4) == 4;
}
// $477C
bool test_iop(uint32_t base) {
	if( test_machine_register( machine->scc, base ) ) {
		return !!std::dynamic_pointer_cast<IOP>(machine->scc);
	} else if( test_machine_register<SWIM>( machine->floppy, base ) ) { 
		return !!std::dynamic_pointer_cast<IOP>(machine->floppy);
	}
	return false;
}
// $47B0
bool has_ether(uint32_t base) {
	return test_machine_register(machine->ether, base );
}

// $47BA(); I don't know about this routine, undocument low memory global?
bool check_LMG1(uint32_t addr) {
	try {
		read_b(addr);
		return read_l(addr+4) == 0x726f6d70 ; // "romp"
	} catch( Exception::AccessFault&) {
		return false;
	}
}

// $4A96
void test_scsi() {
	motherboard = model->motherboard;
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) ) {
		if( ! test_scsi_cmd( motherboard->quadra_scsi_int_addr ) ) { // $04708
			INIT_HW_FLG.reset( INIT_HW_FLG_T::SCSI_040_INT );
		} else {
			INIT_HW_FLG.reset( INIT_HW_FLG_T::SCSI_DMA );
			INIT_HW_FLG.reset( 8 );
			INIT_HW_FLG.reset( 9 );
			INIT_HW_FLG.reset( 10 );
			if( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_EXT ) ) {
				if( ! test_scsi_cmd( motherboard->quadra_scsi_ext_addr ) ) { // $04708
					INIT_HW_FLG.reset( INIT_HW_FLG_T::SCSI_040_EXT );
				}
			}
		}
	}
	if( INIT_HW_FLG.test(INIT_HW_FLG_T::IIFX_EXP1) ) {
		if( ! test_iifx_exp1( motherboard->iifx_exp1_addr ) ) { // $04672
			INIT_HW_FLG.reset( INIT_HW_FLG_T::IIFX_EXP1);
		}
	}
	return;
}


}

namespace ROMWrapper {
using namespace ROM;
static constexpr uint32_t TEST_SUCCESS = 0x02FB0;
static constexpr uint32_t TEST_FAILURE = 0x02FA4;
void run_030AC() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_glu() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_030CA() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_mdu() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_0310A() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_oss() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_03166() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_V8() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_031A0() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_mcu() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_031AE() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_jaws() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_031F2() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_niagara() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_0323A() {
	get_motherboard( AR(0) );
	cpu.PC = rom_base | ( check_null() ? TEST_SUCCESS : TEST_FAILURE );
}
void run_04640() {
	cpu.Z = has_fpu();
	cpu.PC = rom_base | 0x4B18;
}
void run_04672() {
	INIT_FLAGS = DR(7) >> 16;
	cpu.Z = test_iifx_exp1( AR(2) );
	cpu.PC = AR(6);
}
void run_046AA() {
	cpu.Z = has_via(AR(2));
	cpu.PC = AR(6);
}
void run_046AC() {
	cpu.Z = has_via(AR(2), DR(2));
	cpu.PC = AR(6);
}
void run_046D8() {
	cpu.Z = has_rbv( AR(2) );
	cpu.PC = AR(6);
}
void run_04700() {
	cpu.Z = test_scsi_io(AR(2));
	cpu.PC = AR(6);
}
void run_04708() {
	cpu.Z = test_scsi_cmd(AR(2));
	cpu.PC = AR(6);
}
void run_0477C() {
	cpu.Z = test_iop(AR(2));
	cpu.PC = AR(6);
}

void run_047B0() {
	cpu.Z = has_ether( AR(1) );
	cpu.PC = AR(6);
}

void run_047BA() {
	cpu.Z = check_LMG1( AR(1) );
	cpu.PC = AR(6);
}

void run_04A96() {
	get_model(AR(1));
	INIT_HW_FLG = DR(0);
	test_scsi();
	DR(0) = INIT_HW_FLG.to_ulong();
	AR(6) = AR(0);
	AR(0) = AR(1);
	cpu.PC = rom_base | 0x3096;
}
}
