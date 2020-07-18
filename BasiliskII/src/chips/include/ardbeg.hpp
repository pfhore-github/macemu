#pragma once
#include "io.hpp"
#include <memory>
#include "machine.hpp"
class Ardbeg : public Machine {
public:
	enum MODEL {
		LC520, LC550
	};
	Ardbeg(MODEL model = MODEL::LC550);	
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};
