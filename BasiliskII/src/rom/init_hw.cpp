#include <stdint.h>
#include "sysdeps.h"
#include "main.h"
#include "registers.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "inst_cmn.hpp"
#include "rbv.hpp"
#include "oss.hpp"
#include "jaws.hpp"
#include "intel.h"
#include "mem.hpp"
void emul_reset(uint32_t* d, uint32_t* a);
void ex_handler() {
	cpu.Z = ! ROM::INIT_FLAGS.test( ROM::INIT_FLAG_T::FAULT_SKIP);
	AR(7) = AR(5);
}
namespace ROM {

// $2E00
void init_hw() {

	AR(7) = 0x2600;
	for( unsigned int i = 0; i < 4; ++i ) {
		if( rom_base == ( vbr_table[i] & 0xffff0000 ) ) {
			cpu.VBR =  vbr_table[i] - 4*i+4;
			break;
		}
	}
	
	getHWInfo(0);			// $2F64
	SDL_assert( INIT_HW_FLG.test( INIT_HW_FLG_T::MASTER ) );
	
	uint32_t v = remap_rom(rom_base);				// $04AFE
	DR(3) = v;
	AR(4) += v;
	init_hw2( v );			// $04A5E
	
	
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::OSS ) ) {
		machine->oss->write( 0x14, 0x0D); // ROM CTRL
	}
	
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP0) ) {
		uint16_t p = 0xf3ff;
		for(int i = 0; i < 16; ++i, p >>= 1 ) {
			machine->exp0->write(0, (uint8_t)p);
		}
		machine->exp0->write(0x10, p);
	}
	init_hw3();					// $04866
}
// $A06E6
uint32_t get_memory_size(uint16_t sc, uint32_t value);
// $A01C0
ram_result check_simm_slots(const RAM_SLOT_T* ram_slots) {
	ram_result result = { 0, 0, false };
	std::bitset<32> simm_err_flg = 0;	// %D6
	constexpr uint32_t CHECK_DATA = 0x54696E61;
	int i = -4;
	for( auto [ base, end ] : ram_slots->ranges ) {
		// $A0286
		i += 4;
		auto ret = check_simm(base, CHECK_DATA);
		if( ret.none() ) {
			// memory OK; $A0208
			auto [ size, s ] = check_simm_size(base, end, CHECK_DATA, ram_slots->unit); // $A0242
			result.sum_size += size;
			result.mem_size |= size ? ( 32-__builtin_clz(size/ram_slots->unit) ) << i : 0;
		} else if( ! ret.all() ) {
			for(int j = 0; j < 4; ++j ) {
				simm_err_flg[i+j] = ret[j];
			}
		}
	}
	AR(7) = 0x2600;
	getHWInfo(0);
	result.ok = ! simm_err_flg.any();
	return result;
}
uint32_t _A04DC(uint32_t mem_size, uint32_t unit) {
	uint32_t ret = 0;
	for(int i = 0; i < 32; i += 4 ) {
		ret += get_memory_size((mem_size >> i)&0xf, unit);
	}
	ret >>= 20;
	if( ret == 8 )
		return ret - 1;
	return ret;
}

std::tuple<bool, uint32_t, uint32_t> _A0760(const std::vector<std::pair<uint32_t, uint32_t>>& ram_results);
// $A0000
bool ram_check() {
	static const uint8_t VIA_RAMSIZE[] = { 0xff, 0x3f, 0xff, 0x7f, 0xff, 0xbf, 0xff, 0xff }; // $A0326
	std::vector<uint32_t> simm_size_cnt; // %D5
	ram_result ret ;
			
	getHWInfo(0);
	const RAM_SLOT_T* ram_slots = &ram_tables[model->ram_id];
	int offset = 20;
	uint32_t base = ram_slots->ranges[0].first;
	bool check_rom2 = true;
	switch( GEN ) {
	case 4 : 
		// GLUE;  $A0040
		machine->via2->write( VIA_REG::RA, machine->via2->read( VIA_REG::RA ) | 0xC0 );
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A030A
		machine->via2->write( VIA_REG::RA, machine->via2->read( VIA_REG::RA )
							  & VIA_RAMSIZE[ ret.mem_size & 0xf ] );
		offset = 28;
		break;
	case 6 : {
		// OSS;  $A004E
		// XXXXXX
		uint16_t d2 = 0xF3EF;
		for(int i =0 ; i < 16; ++i, d2 >>= 1 ) { 
			machine->exp0->write( 0, d2 );
		}
		machine->exp0->write( 0x10, d2 );
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A0368
		uint16_t exp0_c = INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) ? 0xF3FF : 0xF3EF;
		static const uint8_t OSS_RAMSIZE[] = { 0xff, 0xff, 0xff, 0x3f, 0x7f, 0xbf, 0xff, 0xff }; // $A039E
		exp0_c &= ( 0xff00 | OSS_RAMSIZE[ ret.mem_size & 0xf ] );
		for(int i = 0; i < 16; ++i, exp0_c >>= 1 ) {
			machine->exp0->write( 0, exp0_c );
		}
		machine->exp0->write( 0x10, 0xff );
		if( (ret.mem_size & 0xf) == 0 ) {
			base = ram_slots->ranges[1].first;
			ret.mem_size >>= 4;
		}
		offset = 28;
		break;
	}
	case 8 : {
		// MCU; $A0068
		uint32_t d2 = 0x30810;
		for(int i =0 ; i < 18; ++i, d2 >>= 1 ) { 
			machine->mcu->write32( i*4, d2 );
		}
		machine->mcu->write32( 0xA0, d2 );
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A03A6
		uint32_t size = 0;
		for(unsigned int i = 0; i < ram_slots->ranges.size()-1; ++i ) {
			// $A03E6
			size += get_memory_size( ret.mem_size >> (4*i) & 0xf, ram_slots->unit );
			int addr = i * 6*4;
			uint32_t d1 = size >> 22;
			for(int j = 0; j < 5; ++j ) {
				machine->mcu->write32( addr + 4 *j, d1 );
				d1 >>= 1;
			}
			machine->mcu->write( 0xa0, 0xff );
		}
		offset = 44;
		break;
	}
	case 7 :
	{
		// V8
		// $A0086
		// disable external SIMM
		machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) & 0x1f );
		write_l( 0x1FFFFE, 0x50616E44 );
		if( read_l( 0x1FFFFE ) == 0x50616E44 ) {
			// LC2(4MB+x)
			machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) |  0xC0 );
 			ram_slots = &ram_tables[2];
			auto [ size, end ] = check_simm_size(0, 0x800000, 0x54696E61, ram_tables[2].unit);
			simm_size_cnt.push_back( size );
			if( end >= 0x800000 ) {
				ram_slots = &ram_tables[1];
			} else {
				machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) | 0x80 );
				machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) & 0x9f );
			}
		} else {
			// LC(2MB+x)
			machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) |  0xE0 );
			ram_slots = &ram_tables[1];
		}
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A0458
		static uint8_t V8_RAMSIZE[] = { 0x00, 0xff, 0x40, 0x80, 0xc0, 0, 0x78, 0x00 };
		uint8_t d1 = V8_RAMSIZE[ ret.mem_size & 0xf ];
		machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) & 0x3f );
		machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) | d1 );
		if( machine->rbv->bit( RBV_REG::EXP, 5 ) ) {
			ram_slots = &ram_tables[2];
		}
		offset = 28;
		break;
	}
	case 9 :
	case 12 :
	{
		// Jaws/Niagara; $A00FA
		machine->pb->write( 0x10000, 0 );
		machine->pb->write( 0x12000, 0 );
		machine->pb->write( 0x14000, 0 );
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A048A
		DR(4) = 0;
		if( ret.mem_size == 3 ) {
			DR(4) = 0;			
		} else {
			if( ret.mem_size == 0x30000 ) {
				DR(4) = 32;
			} else {
				uint32_t x = _A04DC(ret.mem_size, ram_slots->unit);
				DR(4) = ((x << 13) & 0xffff) >> 13;
			}
		}
		// A04B6
		static const uint8_t JAWS_RAMSIZE[] = {
			0, 1, 1, 2, 0, 0, 0, 0,
			31, 19, 3, 10, 9, 6, 8, 15,
			7, 3, 18, 9 ,0, 6, 8, 29,
			4, 4, 24, 11, 0, 14, 24, 23,
			9
		}; // $A0508
		uint8_t d0 = JAWS_RAMSIZE[DR(4)];
		machine->pb->write( 0x10000, d0 );
		machine->pb->write( 0x12000, d0 >> 2);
		machine->pb->write( 0x14000, d0 >> 4);
		offset = 76;
		break;
	}
	case 10 :
	{
		// MSC; $A0118
		machine->rbv->write( RBV_REG::MSC_RBV_X16, machine->rbv->read( RBV_REG::MSC_RBV_X16) | 0xf0 );
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A0548
		int d1 = (((ret.mem_size ? 32 - __builtin_clz(ret.mem_size) : 0) << 3) & 0xff) | 0x1f;
		uint32_t d0 = ((0xf0000 & ret.mem_size) >> 16) - 3;
		if( d0  ) {
			d1 &= ~(1 << 4);
		}
		machine->rbv->write( RBV_REG::MSC_RBV_X16, machine->rbv->read( RBV_REG::MSC_RBV_X16) & d1 );
		offset = 76;
		break;
	}
	case 11 : {
		// SONOR; $A013E
		machine->rbv->write( RBV_REG::EXP, machine->rbv->read( RBV_REG::EXP) | 0x7f );
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A040C
		AR(5) = ram_slots->unit;
		uint8_t d6 = 0x40;
		uint32_t mem_v = ret.mem_size; // %D1
		if( (INIT_HW_FLG.to_ulong() & 0xf) == 0){
			d6 &= ~(1<<6);
		}
		mem_v >>= 4;			// first memory always exists
		for(int k = 0; k < 2; ++k, mem_v >>= 8 ) {
			uint8_t mem_i = mem_v & 0xf;
			if( mem_i ) {
				if( mem_v & 0xf0 ) {
					 mem_i++;
				}
				d6 |= mem_i << (3*k);
			}
			// $A0434
		}
		machine->rbv->write( RBV_REG::EXP, (machine->rbv->read( RBV_REG::EXP) & 0x80) | d6 );
		offset = 52;
		break;
	}
	case 13 :
		// MEMCJr; $A014A
		if( machine->model_id != 0x2BAD ) {
			
		} else {
			// $A0184
		}
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A0576
		if( machine->model_id != 0x2BAD ) { // Quadra 610/650/800?
			DR(4) = 4;
			DR(0) = ret.mem_size & 0xf;
			ret.mem_size = ROR(ret.mem_size, 4);
			if( DR(0) == 15 ) {
				DR(0) = machine->reg1->read32(4);
				SWAP( DR(0) );
			}				
		}
		// $A0642
		break;
	case 5 :
	{
		// MDU; $A01C0
		if( ! ( ret = check_simm_slots(ram_slots)) ) {
			return false;
		}
		// $A032E
		uint32_t base = 0;
		for( int i = 1; i >= 0; --i ) {
			uint32_t sz = get_memory_size((ret.mem_size >> (4*i) & 0xf), ram_slots->unit);
			if( sz ) {
				if( base == 0 ) {
					base = sz + ram_slots->ranges[i].first;
					write_l( base -= 4, 0 ); 
					write_l( base -= 4, 0 ); 
					write_l( base -= 4, 0xffffffff );
				}
				write_l( base -= 4, sz ); 
				write_l( base -= 4, ram_slots->ranges[i].first ); 
			}
		}
		check_rom2 = false;
	}
	}
	// $A06F4
	uint32_t a5 = base + ret.sum_size - offset;
	uint32_t mem_addr = a5 - 4;
	std::vector<std::pair<uint32_t, uint32_t>> ram_results;
	if( check_rom2 ) {
		for(int i = 0; i < 32 && (ret.mem_size >> i) & 0xf; i += 4 ) {
			uint32_t size = get_memory_size((ret.mem_size >> i) & 0xf, ram_slots->unit);  // $A06E6
			// $A0700
			if( size ) {
				// has SIMM
				ram_results.push_back( { base, size } );
				write_l( mem_addr += 4, base );
				write_l( mem_addr += 4, size );
				base += size;
			}
		}
		write_l( mem_addr += 4, 0xFFFFFFFF );
		while( mem_addr & 0xffff ) {
			write_l( mem_addr += 4, 0 );
		}
	} 
	// $A072A
	AR(7) = ram_slots->ranges[0].first + 0x8000;
	write_l( AR(7) -= 4, a5 );
	getHWInfo(0);
	AR(6) = a5;
	if( auto [ ret, a0, a1] = _A0760(ram_results); ret ) {
		write_l( AR(7), a5 - 0x2000 );
		for(int i = 1; i <= offset; ++i ) {
			write_b( a0-i, read_b(a1-i));
		}
	}
	// $A075A
	DR(6) = 0;
	return true;
}
// CPU.Z(D0!=0), A0, A1
std::tuple<bool, uint32_t, uint32_t> _A0760(const std::vector<std::pair<uint32_t, uint32_t>>& ram_results) {
	if( GEN != 10 ) {
		return { false, 0, 0};
	}
	// $A0770
	uint32_t base = ram_results.back().first + ram_results.back().second;
	return  { true, base- 0x2000, base };
	
}
// $A06E6
uint32_t get_memory_size(uint16_t sc, uint32_t value) {
	if( sc ) {
		return value << (sc-1);
	} else {
		return 0;
	}
}

}

namespace ROMWrapper {
using namespace ROM;
void run_02E00() {
	uint32_t ret = AR(6);
	INIT_FLAGS = DR(7) >> 16;
	init_hw();
	cpu.PC = rom_base | ret;
	AR(0) = rom_base | motherboard->BASE;
	AR(1) = rom_base | model->BASE;
	DR(0) = INIT_HW_FLG.to_ulong();
	DR(1) = ROM_FLG.to_ulong();
}

void run_A0000() {
	uint32_t fp = AR(6);
	cpu.Z = ram_check();
	cpu.PC = fp;
}
static void run_A01CX(uint32_t base) {
	RAM_SLOT_T slot;
	uint32_t ret = AR(2);
	uint32_t v = 0;
	slot.unit = read_rom32(base);
	for(v = 0, base += 4; (v = read_rom32(base)) != 0xffffffff; base += 4 ) {
		uint32_t w = read_rom32(base += 4);
		slot.ranges.push_back( { v, w } );
	}
	auto [ sum, mem, ok ] = check_simm_slots(&slot);
	DR(0) = sum;
	DR(5) = mem;
	cpu.Z = ok;
	if( ok ) {
		cpu.PC = rom_base | 0xA02BA;
	} else {
		cpu.PC = ret;
	}
}
void run_A01C6() {
	return run_A01CX( AR(5) );
}
void run_A01C0() {	
	uint32_t mb = AR(1);
	return run_A01CX( mb+read_rom32(mb+4) );
}


}
