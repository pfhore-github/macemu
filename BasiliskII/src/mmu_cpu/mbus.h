#ifndef MBUS_H__
#define MBUS_H__
bool b_read8(uint32_t addr, void* v);
bool b_read16(uint32_t addr, void* v);
bool b_read32(uint32_t addr, void* v);
bool b_readline(uint32_t addr, void* v);
bool b_write8(uint32_t addr, const void* v);
bool b_write16(uint32_t addr, const void* v);
bool b_write32(uint32_t addr, const void* v);
bool b_writeline(uint32_t addr, const void* v);
#endif
