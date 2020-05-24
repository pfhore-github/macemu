#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
uint32_t rom_base = 0;
// $04AFE
uint32_t remap_rom(uint32_t base) {
	uint32_t b = motherboard->rom_base - AR(2);
	rom_base = motherboard->rom_base;
	AR(0) += rom_base;
	AR(1) += rom_base;
	return b;
}
}

#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
namespace ROMWrapper {
void run_04AFE() {
	DR(3) = ROM::remap_rom(AR(2));
	cpu.PC = rom_base | 0x2E38;
}
}
