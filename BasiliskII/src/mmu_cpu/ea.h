#ifndef EA_H__
#define EA_H__
#include "newcpu.h"
enum class EA_OP { DR, AR, MEM, INCR, DECR, OFFSET, EXT, EXT2 };
uint32_t EA_Addr(int type, int reg, int sz, bool w);

std::function<void()> EA_Read8(int type, int reg,
                               const std::function<void(uint8_t)> &f);
std::function<void()> EA_Read16(int type, int reg,
                                const std::function<void(uint16_t)> &f);
std::function<void()> EA_Read32(int type, int reg,
                                const std::function<void(uint32_t)> &f);
std::function<void()> EA_Write8(int type, int reg,
                                const std::function<uint8_t()> &f);
std::function<void()> EA_Write16(int type, int reg,
                                 const std::function<uint16_t()> &f);
std::function<void()> EA_Write32(int type, int reg,
                                 const std::function<uint32_t()> &f);
std::function<uint32_t()> ea_addr(int type, int reg, int sz, bool w);
std::function<void()> EA_Update8(int type, int reg,
                                 const std::function<uint8_t(uint8_t)> &f);
std::function<void()> EA_Update16(int type, int reg,
                                  const std::function<uint16_t(uint16_t)> &f);
std::function<void()> EA_Update32(int type, int reg,
                                  const std::function<uint32_t(uint32_t)> &f);

inline void WRITE_D8(int reg, uint8_t v) {
    regs.d[reg] = (regs.d[reg] & 0xffffff00) | v;
}
inline void WRITE_D16(int reg, uint16_t v) {
    regs.d[reg] = (regs.d[reg] & 0xffff0000) | v;
}
inline void WRITE_A16(int reg, int16_t v) { regs.a[reg] = v; }



#endif