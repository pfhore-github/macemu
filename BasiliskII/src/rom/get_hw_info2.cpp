#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
// $47752
void getHWInfo2(uint8_t gestaltId) {
	cpu.intmask = 7;
	getHWInfo(gestaltId);		// $02F64
}
}
namespace ROMWrapper {
void run_47752() {
	using namespace ROM;
	INIT_FLAGS = DR(7);
	getHWInfo2( DR(2) );
	AR(0) = rom_base | motherboard->BASE;
	AR(1) = rom_base | model->BASE;
	DR(0) = INIT_HW_FLG.to_ulong();
	DR(1) = ROM_FLG.to_ulong();
	DR(2) = GET_D2();
	DR(7) = INIT_FLAGS.to_ulong() << 16 | EXCEPTION_STATUS;
}
}
