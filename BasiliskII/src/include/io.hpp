#pragma once
#include <stdint.h>
class IO_BASE {
public:
	virtual uint8_t read(int addr) = 0;
	virtual void write(int addr, uint8_t v) = 0;
	virtual uint16_t read16(int addr) {
		return read(addr) << 8 | read(addr+1);
	}
	virtual uint32_t read32(int addr) {
		return read16(addr) << 16 | read16(addr+2);
	}
	virtual void write16(int addr, uint16_t v) {
		write(addr, v >> 8);
		write(addr+1, v);
	}
	virtual void write32(int addr, uint32_t v) {
		write(addr, v >> 24);
		write(addr+1, v >> 16);
		write(addr+2, v >> 8);
		write(addr+3, v);
	}
	bool bit(int addr, int i) { return read(addr) & (1 << i); }
	void set(int addr, int i) {
		write(addr, read(addr) | (1 << i));
	}
	void clear(int addr, int i) {
		write(addr, read(addr) &~ ( 1 << i));
	}
	virtual ~IO_BASE();
};
