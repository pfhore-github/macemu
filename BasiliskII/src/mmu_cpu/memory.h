/* -*- c++ -*- */
#ifndef MEMORY_H
#define MEMORY_H
#include "SDL.h"
#include "newcpu.h"
#include <vector>
extern std::vector<std::byte> RAM;



struct presult {
    bool R : 1;
    bool T : 1;
    bool W : 1;
    int : 1;
    bool M : 1;
    uint8_t CM : 2;
    bool S : 1;
    uint8_t U : 2;
    bool G : 1;
    bool B : 1;
    unsigned int addr : 20;
};

uint8_t read8(uint32_t addr, bool imem = false);
uint16_t read16(uint32_t addr, bool imem = false);
uint32_t read32(uint32_t addr, bool imem = false);
void write8(uint32_t addr, uint8_t v);
void write16(uint32_t addr, uint16_t v);
void write32(uint32_t addr, uint32_t v);

extern ptrdiff_t MEMBaseDiff;

uint16_t FETCH();
uint32_t FETCH32();
inline void PUSH16(uint16_t v) { write16(regs.a[7] -= 2, v); }
inline void PUSH32(uint32_t v) { write32(regs.a[7] -= 4, v); }
inline uint16_t POP16() {
    return read16(std::exchange(regs.a[7], regs.a[7] + 2));
}
inline uint16_t POP32() {
    return read32(std::exchange(regs.a[7], regs.a[7] + 4));
}

void init_mmu_opc();


#endif /* MEMORY_H */
