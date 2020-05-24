#ifndef MEM_HPP_
#define MEM_HPP_ 
#include "SDL_endian.h"
constexpr uint32_t ATTR_SZ_B = 1;
constexpr uint32_t ATTR_SZ_W = 2;
constexpr uint32_t ATTR_SZ_L = 0;
constexpr uint32_t ATTR_SZ_LN = 3;
constexpr inline uint32_t ATTR_SZ(uint32_t v) { return v & 3; }
constexpr uint32_t ATTR_MOVE16 = 1 << 6;
constexpr inline uint32_t ATTR_FC(uint32_t v) { return 2 << 6 | v << 3; }
constexpr inline uint32_t ATTR(bool sys, bool inst) {
	return (sys << 5)|(inst+1)<<3;
}
constexpr inline uint32_t ATTR_ATC(bool inst) {
	return (3 + inst) << 3;
}
inline bool ATTR_IS_WRITE(uint32_t v) { return v & 1 << 2; }
inline bool ATTR_IS_SYS(uint32_t v) { return v & 1 << 5; }
inline bool ATTR_IS_INST(uint32_t v) { return !(v & 1 << 3); } 
// <TT:2><TM:3><RW:1><SZ:2>
void bus_read(uint32_t addr, int attr, void* dst);
void bus_write(uint32_t addr, int attr, const void* src);
void bus_error(uint32_t addr, int attr) __attribute__((noreturn));
uint8_t read_b(uint32_t va);
uint16_t read_w(uint32_t va);
uint32_t read_l(uint32_t va);
void read_16(uint32_t va, uint8_t* dst);

void write_b(uint32_t va, uint8_t b);
void write_w(uint32_t va, uint16_t w);
void write_l(uint32_t va, uint32_t l);
void write_16(uint32_t va, const uint8_t* src);


#endif
