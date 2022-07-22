#include "sysdeps.h"
#include <SDL.h>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
bool rom_overlay = true;
uint8_t readIO8(uint32_t addr);
uint16_t readIO16(uint32_t addr);
uint32_t readIO32(uint32_t addr);
void writeIO8(uint32_t addr, uint8_t v);
void writeIO16(uint32_t addr, uint16_t v);
void writeIO32(uint32_t addr, uint32_t v);
inline void readIO(uint32_t addr, void *v, int sz) {
    switch(sz) {
    case 1:
        *static_cast<uint8_t *>(v) = readIO8(addr);
        break;
    case 2:
        *static_cast<uint16_t *>(v) = readIO16(addr);
        break;
    case 4:
        *static_cast<uint32_t *>(v) = readIO32(addr);
        break;
    default:
        for(int i = 0; i < sz; ++i) {
            static_cast<uint8_t *>(v)[i] = readIO8(addr + i);
        }
    }
}
inline void writeIO(uint32_t addr, const void *v, int sz) {
    switch(sz) {
    case 1:
        writeIO8(addr, *static_cast<const uint8_t *>(v));
        break;
    case 2:
        writeIO16(addr, *static_cast<const uint16_t *>(v));
        break;
    case 4:
        writeIO32(addr, *static_cast<const uint32_t *>(v));
        break;
    default:
        for(int i = 0; i < sz; ++i) {
            writeIO8(addr + i, static_cast<const uint8_t *>(v)[i]);
        }
    }
}
inline void readBE(void *dst, const void *src, int sz) {
    switch(sz) {
    case 2:
        *static_cast<uint16_t *>(dst) =
            SDL_SwapBE16(*static_cast<const uint16_t *>(src));
        break;
    case 4:
        *static_cast<uint32_t *>(dst) =
            SDL_SwapBE32(*static_cast<const uint32_t *>(src));
        break;
    default:
        memcpy(dst, src, sz);
    }
}

void b_read_impl(uint32_t addr, void *v, int sz) {
    uint32_t base = addr & 0xfffffff;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        if(rom_overlay) {
            return readBE(v, &ROMBaseHost[base & (ROMSize - 1)], sz);
        } else if(base + sz < RAM.size()) {
            return readBE(v, &RAM[base], sz);
        }
        break;
    case 4:
        rom_overlay = false;
        return readBE(v, &ROMBaseHost[base & (ROMSize - 1)], sz);
    case 5:
        return readIO(addr, v, sz);
    }
    throw BUS_ERROR_EX{
        .addr = addr,
        .rw = true,
        .size = sz == 1 ? SZ::BYTE :
        sz == 2 ? SZ::WORD : 
        sz == 4 ? SZ::LONG :
        SZ::LINE
    };
}
uint8_t b_read8(uint32_t addr_v) {
    uint8_t v;
    b_read_impl(addr_v, &v, 1);
    return v;
}
uint16_t b_read16(uint32_t addr) {
    uint16_t v;
    b_read_impl(addr, &v, 2);
    return v;
}
uint32_t b_read32(uint32_t addr) {
    uint32_t v;
    b_read_impl(addr, &v, 4);
    return v;
}

void b_readline(uint32_t addr, std::byte *v) { b_read_impl(addr, v, 16); }

void b_write_impl(uint32_t addr, const void *v, int sz) {
    uint32_t base = addr & 0xfffffff;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        if(base < RAM.size()) {
            return readBE(&RAM[base], v, sz);
        }
        break;
    case 5:
        return writeIO(base, v, sz);
    }
    throw BUS_ERROR_EX{
         .addr = addr,
        .rw = false,
        .size = sz == 1 ? SZ::BYTE :
        sz == 2 ? SZ::WORD : 
        sz == 4 ? SZ::LONG :
        SZ::LINE
    };
}
void b_write8(uint32_t addr, uint8_t v) { b_write_impl(addr, &v, 1); }
void b_write16(uint32_t addr, uint16_t v) { b_write_impl(addr, &v, 2); }
void b_write32(uint32_t addr, uint32_t v) { b_write_impl(addr, &v, 4); }
void b_writeline(uint32_t addr, const std::byte *v) {
    return b_write_impl(addr, v, 16);
}
