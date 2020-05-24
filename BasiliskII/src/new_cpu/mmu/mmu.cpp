#include "../registers.hpp"
#include "../exceptions.hpp"
#include "../inst_cmn.hpp"
#include <stddef.h>
constexpr uint32_t TTR_ENABLE = 1 << 15;
constexpr uint32_t PAGE_MASK = 0xFFF;
constexpr uint32_t MSR_R = 1;
constexpr uint32_t MSR_T = 1 << 1;
constexpr uint32_t MSR_W = 1 << 2;
constexpr uint32_t MSR_M = 1 << 4;
constexpr uint32_t MSR_CM_MASK = 3 << 5;
constexpr uint32_t MSR_S = 1 << 7;
constexpr uint32_t MSR_U_MASK = 3 << 8;
constexpr uint32_t MSR_G = 1 << 10;
constexpr uint32_t MSR_B = 1 << 11;
									  
uint32_t MC68040_MMU::to_real(uint32_t addr, int attr) {
	uint32_t raddr = ptest((addr &~ PAGE_MASK) | attr);
	uint16_t ssw =
		((attr >> 3) & 0x1f) | // TT&TM
		((attr <<5) & 3) |	 // SIZE
		(ATTR_IS_WRITE(attr) << 8) | // RW 
		1 << 10 |					 // ATC
		cpu.misaligned << 11;		 // MA
	cpu.misaligned = false;
	if(
		(raddr & MSR_B)   || 		// BUS ERROR
		! (raddr & MSR_R) ||		// memory fault
		(( raddr & MSR_W) && ATTR_IS_WRITE(attr)) || // WRITE ERROR
		(( raddr & MSR_S) && !ATTR_IS_SYS(attr))	   // ACCESS VIOLATION
		) {
		throw Exception::AccessFault(addr, ssw, ATTR_IS_INST(attr));
	}
	return (raddr &~ PAGE_MASK) | (addr & PAGE_MASK);
}

uint32_t MC68040_MMU::match_ttr(uint32_t ttr, uint32_t va, bool s) {
	if( ! (ttr & TTR_ENABLE) )
		return 0;
	int ttr_S = (ttr >>13) & 3;
	if( ttr_S < 2 && ttr_S != s )
		return 0;
	uint8_t mask = ttr >> 16;
	if( ((va >> 24) &~ mask ) != ((ttr >> 24) &~ mask) )
		return 0;
	return (va &~ PAGE_MASK) | 3;
}
static uint32_t check_td(uint32_t adr, bool inst) {
	uint32_t uu;
	bus_read(adr, ATTR_ATC(inst), &uu);
	uu |= 8;
	bus_write(adr, ATTR_ATC(inst), &uu);	
	if( !(uu & 2) ) {
		return 0;
	}
	return uu | 1;
}

// <vaddr:> FC-SZ-RW
uint32_t MC68040_MMU::ptest(uint32_t in) {
	bool code =  ATTR_IS_INST(in);
	bool super = ATTR_IS_SYS(in);
	uint32_t va = in;
	if( code ) {
		for(int i = 0;i < 2; ++i ) {
			if( uint32_t v = match_ttr(ITTR[i], in, super); v & 3 ) {
				return v;
			}
		}
	} else { 
		for(int i = 0;i < 2; ++i ) {
			if( uint32_t v = match_ttr(DTTR[i], in, super); v & 3 ) {
				return v;
			}
		}
	}
	if( ! enable() ) {
		return (in &~ PAGE_MASK) | 3;
	}
	uint32_t key = super << 12 | va >> (large_page() ? 13 : 12);
	auto entry = g_caches.find(key);
	if( entry == g_caches.end() ) {
		entry = ng_caches.find(key);
	}
	if ( entry != ng_caches.end() ) {
		return entry->second;
	}
	try {
		uint32_t e = 0;
		bool w = false;
		uint32_t root = super ? SRP : URP;
		uint8_t ri = va >> 25;
		uint32_t uu = check_td(root |(ri<<2), code);
		if( uu  == 0 ) {
			return 0;
		}		
		w = w || (uu & 4);
		uint32_t uu2 = check_td((uu &~ 0x1ff)|((va >> 18) & 0x7f)<<2, code);
		if( uu2 == 0 ) {
			return 0;
		}
		w = w || (uu2 & 4);
		uint8_t pgi = large_page() ? (va >> 13) & 0x3f
			: (va >> 12) & 0x7f;
		uint32_t addr3 = (uu2 &~ (large_page() ? 0x7F : 0xFF))|(pgi<<2);
		uint32_t uu3;
		bus_read(addr3, ATTR_ATC(code), &uu3);
		if((uu3 & 3) == 2 ) {
			addr3 = uu3&~3;
			bus_read(addr3, ATTR_ATC(code), &uu3);
		}
		uint32_t v = uu3 | 8;
		if( ATTR_IS_WRITE(in) && ! (uu3 & 4) ) {
			v |= 16;
		}
		bus_write(addr3, ATTR_ATC(code), &v);
		w = w || (uu3 & 4);
		if( uu3 == 0 ) {
			return 0;
		}

		//  G U1 U0 S C-M M O W T R
		
		uint32_t paddr = (uu3 &~  (large_page() ? 0x100F: 0xF)) | w << 2 | 1;
		
		e |= w << 2;
		auto q = (uu3 & 1 << 10) ? g_caches.insert( std::make_pair(key, paddr) ) :
			ng_caches.insert( std::make_pair(key, paddr) );
		
		entry = q.first;
		return entry->second;
	} catch( Exception::AccessFault& e) {
		return MSR_B;
	}
}
void MC68040_MMU::movec_to(int v, uint32_t value) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	switch(v) {
	case 0 : cpu.SFC = value; return;
	case 1 : cpu.DFC = value; return;
	case 2 : cpu.CACR = value & 0x80008000; return;
	case 3 : TCR = value & 0xc000; return;
	case 4 : ITTR[0] = value &~ 0x1C9B ; return;
	case 5 : ITTR[1] = value &~ 0x1C9B; return;
	case 6 : DTTR[0] = value &~ 0x1C9B; return;
	case 7 : DTTR[1] = value &~ 0x1C9B; return;
	case 0x800 : cpu.USP = value; return;
	case 0x801 : cpu.VBR = value; return;
	case 0x803 : cpu.MSP = value; return;
	case 0x804 : cpu.ISP = value; return;
	case 0x805 : MMU_SR = value &~ 8; return;
	case 0x806 : URP = value &~ 0x1ff; return;
	case 0x807 : SRP = value &~ 0x1ff; return;
	}
}
uint32_t MC68040_MMU::movec_from(int v) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	switch(v) {
	case 0 : return cpu.SFC;
	case 1 : return cpu.DFC;
	case 2 : return cpu.CACR & 0x80008000;
	case 3 : return TCR & 0xc000;
	case 4 : return ITTR[0] &~ 0x1C9B;
	case 5 : return ITTR[1] &~ 0x1C9B;
	case 6 : return DTTR[0] &~ 0x1C9B;
	case 7 : return DTTR[1] &~ 0x1C9B;
	case 0x800 : return cpu.USP;
	case 0x801 : return cpu.VBR;
	case 0x803 : return cpu.MSP;
	case 0x804 : return cpu.ISP;
	case 0x805 : return MMU_SR &~ 8;
	case 0x806 : return URP &~ 0x1ff;
	case 0x807 : return SRP &~ 0x1ff;
	}
	return 0;
}
void MC68040_MMU::exec(uint16_t op, int mode, int reg) {
	bool P = (TCR & 1 << 14);
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	switch((op>> 6)&7) {
	case 0 :
		// CINV/CPUSH NOP
		break;
	case 1 :
		// CINV/PUSH DATA
		break;
	case 2 :
	case 3 :
		// CINV/PUSH INST & DATA
		// TODO: erase jit cache
		break;
	case 4 :
		// PFLUSH
		switch(mode) {
		case 0 : 
			// PFLUSHN 
			ng_caches.erase( cpu.DFC << 12 | cpu.R[8+reg] >> (P ? 13 : 12) );
			break;
		case 1 :
			// PFLUSH
			g_caches.erase( cpu.DFC << 12 | cpu.R[8+reg] >> (P ? 13 : 12) );
			ng_caches.erase( cpu.DFC << 12 | cpu.R[8+reg] >> (P ? 13 : 12) );
			break;
		case 2 :
			// PFLUSHAN
			ng_caches.clear();
			break;
		case 3 :
			// PFLUSHA
			g_caches.clear();
			ng_caches.clear();
			break;
		}
		test_trace_branch();
		break;
	case 5 :
		MMU_SR = ptest((cpu.R[8+reg] &~ PAGE_MASK) |  ATTR_FC(cpu.DFC));
		test_trace_branch();
		break;
	default:
		throw Exception::IllegalInst();
	}
}
