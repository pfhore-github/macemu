#pragma once
#include "io.hpp"
#include <memory>
#include "machine.hpp"
class Sonora : public Machine {
public:
	enum MODEL {
		UNUSED1, LCIII, LCIII_p, UNUSED2, UNUSED3, UNUSED4, UNUSED5
	};
	Sonora(MODEL model = MODEL::LCIII_p);	
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};
