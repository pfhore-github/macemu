#include <vector>
int asm_m68k(const char* a, int offset = 0);
void m68k_do_execute ();
extern std::vector<uint8_t> RAM;

uint8_t raw_read8(uint32_t addr);
uint16_t raw_read16(uint32_t addr);
uint32_t raw_read32(uint32_t addr);
void raw_write8(uint32_t addr, uint8_t v);
void raw_write16(uint32_t addr, uint16_t v);
void raw_write32(uint32_t addr, uint32_t v);
