#include "data.hpp"
#include "scc.hpp"
#include "prototype.hpp"
namespace ROM {
// $B9F46
RW_offset get_scc_addr() {
	getHWInfo2(0);				// $47752
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::IOP_SCC ) ) {
		return { 0, 0};
	} else {
		return { 0, motherboard->scc_write_addr - motherboard->scc_read_addr};
	}
}

}
#include "data.hpp"
#include "scc.hpp"
#include "prototype.hpp"
namespace ROMWrapper {
void run_B9F46() {
	using namespace ROM;
	auto [ via, scc ] = get_scc_addr();
	AR(2) = motherboard->via1_addr;
	DR(2) = via;
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::IOP_SCC ) ) {
		AR(3) = motherboard->scc_iop_addr;
	} else {
		AR(3) = motherboard->scc_read_addr;
	}
	DR(3) = scc;
	cpu.PC = AR(6);
	
}
}
