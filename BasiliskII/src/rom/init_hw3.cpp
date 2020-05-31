#include "data.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "rbv.hpp"
#include "mcu.hpp"
#include "jaws.hpp"
#include "memcjr.hpp"
#include "prototype.hpp"
namespace ROM {
// $4866
void init_hw3() {
	const std::bitset<32> rom_flg = model->rom_flg;
	if( rom_flg.test( ROM_FLG_T::SONOR_F108) ) {
		// Sonora/F108
		uint32_t mc_v = machine->model_id;
		machine->rbv->write(4, 0);	// rMonP
		machine->rbv->write(1, 0x40); // rEXP?
		machine->rbv->write(5, 0x3);	 // rChpT?
		if((mc_v & 0xff00) < 0x2000 &&
		   ( (mc_v & 0x07ff) != 3) ) {
			// not LCIII+
			machine->rbv->write(5, 0x2); 
		}
	}
	// Quadra 700/900/950(MCU); $48BC; init NuBus?
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::MCU ) ) {
		uint32_t d4 = 0x124f0810;
		machine->via2->clear(VIA_REG::DDRB, 5); // VIA2B_vTfr1;
		if( machine->via2->bit(VIA_REG::RB, 5 ) ) {
			d4 = 0x138b0810;
		}
		for(uint32_t i = 0, addr = 0; i < 32; ++i, d4 >>= 1, addr += 4 ) {
			machine->mcu->write32(addr, d4);
		}
		machine->mcu->write32(0x80, 0);
		machine->mcu->write32(0xa0, 0xffff);
		machine->mcu->write32(0xa4, 0xffff);
		machine->mcu->write32(0xa8, 0xffff);
		machine->mcu->write32(0xac, 0xffff);
		machine->mcu->write32(0xb0, 0xffff);
		machine->mcu->write32(0xb4, 0xffff);
		d4 = ! (machine->mcu->read32(0x48) & 1);
		machine->mcu->write32(0x84, d4);
		machine->mcu->write32(0xb8, 0xffff);
	}
	// PB14x/16x/170/180(Jaws variant); $4920
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::POWERBOOK ) ) {
		machine->pb->write(0, machine->pb2->read( 0) & 1);
	}
	
	// Quadra605/LC475/630/580; $493A
	if( rom_flg.test( ROM_FLG_T::MEMCRjr_F108 ) ) { 
		struct machine_map_t {
			uint16_t d0, d1;
			uint8_t d2;
			uint16_t d3;
		};
		if( (uint16_t)(machine->model_id) != 0x2bad ) {
			// Quadra605/LC475/LC630/LC580
			static machine_map_t machine_map[4] = {
				// $49BC-$49C2, $4A28-$4A2E, $4A30-$4A33, $4A34-$4A3B
				{ 0x0101, 0x011D, 1, 0x0280 }, // Quadra 605/LC475 20MHz, LC630-3, LC630-6, LC580-3
				{ 0x001A, 0x016B, 1, 0x01E0 }, // Quadra 605/LC475 25MHz(original), ???
				{ 0x00DC, 0x01E7, 0, 0x00D5 }, // Quadra 605/LC475 33MHz, ???, LC630-1, LC630-4, LC580-1
				{ 0x02DC, 0x0255, 0, 0x0000 }, // LC630-2, LC580-2
			};
			uint16_t mid = machine->model_id & 3;
			
			uint32_t d0 = 0xa55a0000 | machine_map[mid].d0;
			machine->reg1->write32(0x7c, d0 >> 6 | d0 << 26);
			machine->reg1->write32(0x30, d0);

			d0 = 0xa55a0000 | machine_map[mid].d1;
			machine->reg1->write32(0x7c, d0 >> 6 | d0 << 26);
			machine->reg1->write32(0x34, d0);

			machine->reg2->write(0, machine_map[mid].d2);
			machine->reg2->write16(0x600, machine_map[mid].d3);
			uint16_t d3_x = machine->model_id >> 2 | machine->model_id << 14;
			if( (d3_x & 0x01ff) == 0x8c ) {
				// unreleased machine?( MID = #2231/#2232; gestalt ID=96/97 )
				machine->reg2->set(0, 4);
				machine->reg2->set(0x100, 3);
			}
		} else {
			// Quadra/Centris 610/650/800 ?
			// $49C4
			machine->via1->write(VIA_REG::DDRA, 0x28);
			// get CPUID
			uint8_t cpu_id = 0x14 &	machine->via1->read(VIA_REG::RA);
			cpu_id >>= 2;
			if( cpu_id & (1<<2) ) {
				cpu_id |= 1 << 1;
			}
			cpu_id &= 3;
			// D3 = PA4|PA2
			static machine_map_t machine_map2[4] = {
				{ 0x0101, 0x011D, 1, 0x0280 }, // Centris 610?
				{ 0x001A, 0x016B, 1, 0x01E0 }, // Quadra 610, Centris (25Mhz) 650 
				{ 0x00A3, 0x01E7, 0, 0x00D5 }, // Quadra 800, Quadra (33Mhz) 650 
				{ 0x02F4, 0x0255, 0, 0x0000 }, // ???
			};
			machine->reg1->write32(0x30, 0xa55a0000 | machine_map2[cpu_id].d0);
			machine->reg1->write32(0x34, 0xa55a0000 | machine_map2[cpu_id].d1);
			uint32_t v = machine->reg2->read32(0);
			machine->reg2->write32(0, (v & 0xfffffff7)	| machine_map2[cpu_id].d2);
			machine->reg2->write32(0x600, (v & 0xffff0000) | machine_map2[cpu_id].d3);			
		
		}
		// $04A3C
		if( rom_flg.test(ROM_FLG_T::F108_Ardbeg) ) { // LCxxx?
			machine->reg2->write16(0x500, 0x90b7);
		}
	}
	return;
}

}
namespace ROMWrapper {
void run_04866() {
	ROM::get_motherboard(AR(0));
	ROM::get_model(AR(1));
	ROM::INIT_HW_FLG = DR(0);
	ROM::init_hw3();
	cpu.PC = AR(4);
}
}
