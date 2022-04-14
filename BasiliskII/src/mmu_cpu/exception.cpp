#include <vector>
#include "SDL.h"
#include "sysdeps.h"

#include "cpu_emulation.h"
#include "main.h"
#include "newcpu.h"
void SET_SR(uint16_t v) ;
jmp_buf ex_jmp;
static void __attribute__((noreturn))
RAISE(int e, int f, const std::vector<uint16_t>& data, bool next) {
	if( regs.exception ) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "double fault occurerd at %08x\n", regs.pc);
		abort();
	}
	regs.exception = true;
	uint16_t sr = GET_SR();
	SET_SR( sr &~ 0xE0 );
	for( auto v : data ) {
		PUSH16(v);
	}
	if( next ) {
		PUSH32( regs.pc );
	} else {
		PUSH32( regs.opc );
	}
	PUSH16( f << 12 | e << 2 ); 
	PUSH16(sr);
	regs.pc = read32(regs.vbr + (e << 2));
	regs.exception = false;
	longjmp(ex_jmp, 1);
}
void BUSERROR(uint32_t addr, uint16_t ssw, uint32_t ea) {
	RAISE(2, 7, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0 ,
			uint16_t(addr & 0xffff),
			uint16_t(addr >> 16), 0, 0, 0,
			// SSW
			ssw,
			uint16_t(ea & 0xffff),
			uint16_t(ea >> 16)}, false);
}
void ILLEGAL_INST() {
	RAISE(4, 0, {}, false);
}

void PRIV_ERROR() {
	RAISE(8, 0, {}, false);
}

void CHK2_ERROR() {
    RAISE( 6, 2, { (uint16_t)(regs.opc >> 16), (uint16_t)( regs.opc & 0xffff)}, true);
}