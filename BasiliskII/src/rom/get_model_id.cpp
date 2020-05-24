#include <stdint.h>
#include <optional>
#include "data.hpp"
#include "prototype.hpp"
// $4BE4
namespace ROM {
std::optional<uint16_t> get_model_id() {
	uint32_t v = machine->model_id;
	if( v >> 16 == 0xa55a ) {
		return uint16_t(v);
	} else {
		return {};
	}
}
}
namespace ROMWrapper {
void run_04BE4() {
	auto ret = ROM::get_model_id();
	if( ret ) {
		cpu.R[0] = 0xA55A0000 | *ret;
		cpu.Z = true;
	} else {
		cpu.Z = false;
	}
	cpu.PC = AR(6);
}
}
