#ifndef MBUS_H__
#define MBUS_H__
#include <cstdint>
#include "memory.h"
uint8_t b_read8(uint32_t addr);
uint16_t b_read16(uint32_t addr);
uint32_t b_read32(uint32_t addr);
void b_readline(uint32_t addr, std::byte* v);
void b_write8(uint32_t addr, uint8_t v);
void b_write16(uint32_t addr, uint16_t v);
void b_write32(uint32_t addr, uint32_t v);
void b_writeline(uint32_t addr, const std::byte* v);
#endif
