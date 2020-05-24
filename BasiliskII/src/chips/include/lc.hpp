#pragma once
#include "io.hpp"
// @50F18000
class LC_REG : public IO_BASE {
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
