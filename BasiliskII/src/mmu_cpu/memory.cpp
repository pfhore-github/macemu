/*
 * UAE - The Un*x Amiga Emulator
 *
 * Memory management
 *
 * (c) 1995 Bernd Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "sysdeps.h"

#include "cpu_emulation.h"
#include "main.h"
#include "video.h"

#include "memory.h"
#include "readcpu.h"
#include "newcpu.h"
#include <vector>
extern uint8* ROMBaseHost;
extern std::vector<uint8_t> RAM;
bool rom_overlay;

void* get_real_addr(uint32_t addr, const ssw_t& ssw) {
	uint32_t base = addr & 0xfffffff;
	switch( addr >> 28 ) {
	case 0 :
		if( rom_overlay ) {
			if( ssw.read ) {
				return &ROMBaseHost[base];
			} else {
				BUSERROR(addr, ssw.to_value(), regs.i_ea);
			}
		}
		/* fall through */
	case 1 :
	case 2 :
	case 3 :
		if( base < RAM.size() ) {
			return (&RAM[base]);
		} else {
			BUSERROR(addr, ssw.to_value(), regs.i_ea);
		}
	case 4 :
		if( ! ssw.read ) {
			return (&ROMBaseHost[base]);
		} else {
			BUSERROR(addr, ssw.to_value(), regs.i_ea);
		}
	default :
		return nullptr;
	}
}
// MMU enabled access
uint8_t read8(uint32_t addr) {
	uint8_t v;
	paccess(paddr{ addr, 0, SZ::BYTE, TT::NORMAL, 
	regs.S ? TM::SUPER_DATA : TM::USER_DATA, false}, &v);
	return v;
}
uint16_t read16(uint32_t addr) {
	uint16_t v;
	paccess(paddr{ addr, 0, SZ::WORD, TT::NORMAL, 	
		regs.S ? TM::SUPER_DATA : TM::USER_DATA, false}, &v);
	return v;
}

uint16_t FETCH() {
	uint16_t v;
	paccess(paddr{ regs.pc, 0, SZ::WORD, TT::NORMAL, 
		regs.S ? TM::SUPER_CODE : TM::USER_CODE, false}, &v);
	regs.pc += 2;
	return v;
}
uint32_t read32(uint32_t addr) {
	uint32_t v;
	paccess(paddr{ addr, 0, SZ::LONG, TT::NORMAL, 
	regs.S ? TM::SUPER_DATA : TM::USER_DATA, false}, &v);
	return v;
}
uint32_t FETCH32() {
	uint32_t v;
	paccess(paddr{ regs.pc, 0, SZ::LONG, TT::NORMAL, 
		regs.S ? TM::SUPER_CODE : TM::USER_CODE, false}, &v);
	regs.pc += 4;
	return v;
}
void write8(uint32_t addr, uint8_t v) {
	paccess(paddr{ addr, 0, SZ::BYTE, TT::NORMAL, 
	regs.S ? TM::SUPER_DATA : TM::USER_DATA, true}, &v);
}
void write16(uint32_t addr, uint16_t v) {
	paccess(paddr{ addr, 0, SZ::WORD, TT::NORMAL, 
	regs.S ? TM::SUPER_DATA : TM::USER_DATA, true}, &v);
}
void write32(uint32_t addr, uint32_t v) {
	paccess(paddr{ addr, 0, SZ::LONG, TT::NORMAL, 
	regs.S ? TM::SUPER_DATA : TM::USER_DATA, true}, &v);
}



uint8_t readIO8(uint32_t addr) { return 0; }
uint16_t readIO16(uint32_t addr) { return 0; }
uint32_t readIO32(uint32_t addr) { return 0; }
void writeIO8(uint32_t addr, uint8_t v) {}
void writeIO16(uint32_t addr, uint16_t v) {}
void writeIO32(uint32_t addr, uint32_t v) {}

