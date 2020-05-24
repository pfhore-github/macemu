#pragma once
#include "io.hpp"
/** Sonic DP83932 Ethernet */
class SONIC : public IO_BASE {
	uint16_t cmd_reg;
public:
	uint8_t read(int n) override { return 0; }
	void write(int n, uint8_t v) override {}
	uint16_t read16(int n) override;
	void write16(int n, uint16_t v) override;
};
