#pragma once
#include "machine.hpp"
#include "io.hpp"
class V8 : public Machine {
public:
	enum class MODEL {
		LC, LC2, IIvx, IIvi, ColorClassic, UNKNOWN
	};
	V8(MODEL model = MODEL::LC);
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};
