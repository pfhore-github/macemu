#include "sysdeps.h"
#include <SDL.h>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"

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

bool rom_overlay;
bool b_read_impl(uint32_t addr, void *v, int sz) {
    uint32_t base = addr & 0xfffffff;
    switch(addr >> 28) {
    case 0:
        if(rom_overlay) {
            goto ROM;
        }
        /* fall through */
    case 1:
    case 2:
    case 3:
        if(addr + sz < RAM.size()) {
            readBE(v, &RAM[addr], sz);
            return true;
        }
        break;
    case 4:
        rom_overlay = false;
    ROM:
        if(base < ROMSize) {
            readBE(v, &ROMBaseHost[base], sz);
            return true;
        }
        break;
    case 5:
        readIO(addr, v, sz);
        return true;
    }
    return false;
}
bool b_read8(uint32_t addr, void *v) { return b_read_impl(addr, v, 1); }
bool b_read16(uint32_t addr, void *v) {
    if(addr & 1) {
        uint8_t vp[2];
        bool ret = b_read8(addr, vp) && b_read8(addr + 1, vp + 1);
        if(ret) {
            uint16_t *vv = static_cast<uint16_t *>(v);
            *vv = vp[0] << 8 | vp[1];
        }
        return ret;
    }
    return b_read_impl(addr, v, 2);
}
bool b_read32(uint32_t addr, void *v) {
    if(addr & 2) {
        uint16_t vp[2];
        bool ret = b_read16(addr, vp) && b_read16(addr + 2, vp + 1);
        if(ret) {
            uint32_t *vv = static_cast<uint32_t *>(v);
            *vv = vp[0] << 16 | vp[1];
        }
        return ret;
    }
    return b_read_impl(addr, v, 4);
}

bool b_readline(uint32_t addr, void *v) { return b_read_impl(addr, v, 16); }

bool b_write_impl(uint32_t addr, const void *v, int sz) {
    uint32_t base = addr & 0xfffffff;
    switch(addr >> 28) {
    case 0:
        if(rom_overlay) {
            return false;
        }
        /* fall through */
    case 1:
    case 2:
    case 3:
        if(addr < RAM.size()) {
            readBE(&RAM[addr], v, sz);
            return true;
        }
        break;
    case 5:
        writeIO(addr, v, sz);
        return true;
    }
    return false;
}
bool b_write8(uint32_t addr, const void *v) {
    return b_write_impl(addr, v, 1);
}
bool b_write16(uint32_t addr, const void *v) {
    if(addr & 1) {
        uint16_t vp = *static_cast<const uint16_t*>(v);
        uint8_t v2[2] = {static_cast<uint8_t>(vp >> 8),
                         static_cast<uint8_t>(vp & 0xff)};
        return b_write8(addr, v2) && b_write8(addr + 1, v2 + 1);
    }
    return b_write_impl(addr, v, 2);
}
bool b_write32(uint32_t addr, const void *v) {
    uint32_t base = addr & 0xfffffff;
    if(addr & 2) {
        uint32_t vp = *static_cast<const uint32_t*>(v);
        uint16_t v2[2] = {static_cast<uint16_t>(vp >> 16),
                          static_cast<uint16_t>(vp & 0xffff)};
        return b_write16(addr, v2) && b_write16(addr + 2, v2 + 1);
    }
    return b_write_impl(addr, v, 4);
}

bool b_writeline(uint32_t addr, const void *v) { return b_write_impl(addr, v, 16); }
