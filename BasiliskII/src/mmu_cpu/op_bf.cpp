#include "cpu_emulation.h"
#include "main.h"
#include "sysdeps.h"
#include <bit>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

#include "ea.h"
#include "exception.h"
#include "intop.h"
#include "newcpu.h"
template <class T> T get_bf(uint32_t v, uint8_t offset, uint8_t width) {
    return static_cast<T>(std::rotl<uint32_t>(v, offset)) >> (32 - width);
}

template <class T> struct ext_tx { using type = void; };
template <> struct ext_tx<uint32_t> { using type = uint64_t; };
template <> struct ext_tx<int32_t> { using type = int64_t; };
template <class T> using ext_t = typename ext_tx<T>::type;
template <class T> T get_bf_m(uint32_t &addr, int &offset, uint8_t width) {
    // normalize
    addr += offset >> 3;
    offset = offset & 7;
    int len = offset + width;
    int cnt = len >> 3;
    ext_t<T> v = 0;
    // left aligned
    for(int i = 0; i <= cnt; ++i) {
        v |= static_cast<uint64_t>(read8(addr + i)) << (56 - 8 * i);
    }
    v <<= offset;
    return v >> (64 - width);
}

std::pair<int, int> parse_bf(uint16_t op2) {
    int offset = op2 >> 6 & 0x1f;
    if(op2 >> 11 & 1) {
        offset = regs.d[offset & 0xf];
    }
    int width = op2 & 0x1f;
    if(op2 >> 5 & 1) {
        width = (regs.d[width & 0xf]) & 31;
    }
    if(width == 0) {
        width = 32;
    }
    return {offset, width};
}
inline void bf_r_common(int reg, int offset, int width) {
    regs.v = false;
    regs.c = false;
    int32_t ret = get_bf<int32_t>(regs.d[reg], offset, width);
    regs.n = ret < 0;
    regs.z = ret == 0;
}

inline void bf_m_common(uint32_t &addr, int &offset, int width) {
    regs.v = false;
    regs.c = false;
    int32_t ret = get_bf_m<int32_t>(addr, offset, width);
    regs.n = ret < 0;
    regs.z = ret == 0;
}
OP(bftst) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    if(type == 0) {
        bf_r_common(reg, offset, width);
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        bf_m_common(addr, offset, width);
    }
}
OP(bfchg) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    if(type == 0) {
        bf_r_common(reg, offset, width);
        regs.d[reg] ^= std::rotr<uint32_t>(
            0xffffffff >> (32 - width) << (32 - width), offset);
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        bf_m_common(addr, offset, width);
        int sz = offset + width;
        if(sz < 8) {
            write8(addr, read8(addr) ^ (0xff >> offset << (8 - sz)));
            return;
        }
        write8(addr, read8(addr) ^ (0xff >> offset));
        sz -= 8;
        for(int i = 1; sz > 0; sz -= 8, ++i) {
            if(sz < 8) {
                write8(addr + i, read8(addr + i) ^ (0xff << (8 - sz)));
            } else {
                write8(addr + i, ~read8(addr + i));
            }
        }
    }
}

OP(bfclr) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    if(type == 0) {
        bf_r_common(reg, offset, width);
        regs.d[reg] &= ~std::rotr<uint32_t>(
            0xffffffff >> (32 - width) << (32 - width), offset);
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        bf_m_common(addr, offset, width);
        int sz = offset + width;
        if(sz < 8) {
            write8(addr, read8(addr) & ~(0xff >> offset << (8 - sz)));
            return;
        }
        write8(addr, read8(addr) & ~(0xff >> offset));
        sz -= 8;
        for(int i = 1; sz > 0; sz -= 8, ++i) {
            if(sz < 8) {
                write8(addr + i, read8(addr + i) & ~(0xff << (8 - sz)));
            } else {
                write8(addr + i, 0);
            }
        }
    }
}
OP(bfset) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    if(type == 0) {
        bf_r_common(reg, offset, width);
        regs.d[reg] |= std::rotr<uint32_t>(
            0xffffffff >> (32 - width) << (32 - width), offset);
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        bf_m_common(addr, offset, width);
        int sz = offset + width;
        if(sz < 8) {
            write8(addr, read8(addr) | (0xff >> offset << (8 - sz)));
            return;
        }
        write8(addr, read8(addr) | (0xff >> offset));
        sz -= 8;
        for(int i = 1; sz > 0; sz -= 8, ++i) {
            if(sz < 8) {
                write8(addr + i, read8(addr + i) | (0xff << (8 - sz)));
            } else {
                write8(addr + i, 0xff);
            }
        }
    }
}

inline int CLZ(unsigned int v, int width) {
    return std::countl_zero(v) - (32 - width);
}
OP(bfextu) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    if(type == 0) {
        bf_r_common(reg, offset, width);
        regs.d[op2 >> 12 & 7] = get_bf<uint32_t>(regs.d[reg], offset, width);
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        bf_m_common(addr, offset, width);
        regs.d[op2 >> 12 & 7] = get_bf_m<uint32_t>(addr, offset, width);
    }
}
OP(bfexts) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    if(type == 0) {
        bf_r_common(reg, offset, width);
        regs.d[op2 >> 12 & 7] = get_bf<int32_t>(regs.d[reg], offset, width);
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        bf_m_common(addr, offset, width);
        regs.d[op2 >> 12 & 7] = get_bf_m<int32_t>(addr, offset, width);
    }
}
OP(bfffo) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    int xf = offset;
    if(type == 0) {
        bf_r_common(reg, offset, width);
        uint32_t vv = get_bf<uint32_t>(regs.d[reg], offset, width);
        regs.d[op2 >> 12 & 7] = CLZ(vv, width) + xf;
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        bf_m_common(addr, offset, width);
        uint32_t vv = get_bf_m<uint32_t>(addr, offset, width);
        regs.d[op2 >> 12 & 7] = CLZ(vv, width) + xf;
    }
}

uint8_t bit_ins(uint8_t oldv, int offset, int width, uint8_t newv) {
    uint8_t mask = 0xff >> (8 - width);
    return (oldv & ~(mask << (8 - offset - width))) |
           ((newv & mask) << (8 - offset - width));
}

OP(bfins) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    regs.v = false;
    regs.c = false;
    if(type == 0) {
        uint32_t v =
            std::rotr<uint32_t>(regs.d[op2 >> 12 & 7] << (32 - width), offset);
        regs.d[reg] &= ~std::rotr<uint32_t>(
            0xffffffff >> (32 - width) << (32 - width), offset);
        regs.d[reg] |= v;
        regs.z = v == 0;
        regs.n = v & 1 << (31 - offset);
    } else {
        uint32_t addr = EA_Addr(type, reg, 0, false);
        addr += offset >> 3;
        offset = offset & 7;
        uint32_t v = regs.d[op2 >> 12 & 7] << (32 - width) >> (32 - width);
        regs.z = v == 0;
        regs.n = v & 1 << width;
        int sz = offset + width;
        switch(sz >> 3) {
        case 0:
            // OOO VVVVV X
            write8(addr, bit_ins(read8(addr), offset, width, v));
            break;
        case 1:
            // OOO VVVVV-VVV XXXXX
            write8(addr,
                   bit_ins(read8(addr), offset, 8 - offset, v >> (sz - 8)));
            write8(addr + 1, bit_ins(read8(addr + 1), 0, sz - 8, v));
            break;
        case 2:
            // OOO VVVVV-VVVVVVVV-VVV XXXXX
            write8(addr,
                   bit_ins(read8(addr), offset, 8 - offset, v >> (sz - 8)));
            write8(addr + 1, v >> (sz - 16));
            write8(addr + 2, bit_ins(read8(addr + 2), 0, sz - 16, v));
            break;
        case 3:
            // OOO VVVVV-VVVVVVVV-VVVVVVVV-VVV XXXXX
            write8(addr, (read8(addr) & ~(0xff >> offset)) | v >> (sz - 8));
            write8(addr + 1, v >> (sz - 16));
            write8(addr + 2, v >> (sz - 24));
            write8(addr + 3, bit_ins(read8(addr + 3), 0, sz - 24, v));
            break;
        case 4:
            // OOO VVVVV-VVVVVVVV-VVVVVVVV-VVVVVVVV-VVV XXXXX
            write8(addr, (read8(addr) & ~(0xff >> offset)) | v >> (sz - 8));
            write8(addr + 1, v >> (sz - 16));
            write8(addr + 2, v >> (sz - 24));
            write8(addr + 3, v >> (sz - 32));
            write8(addr + 4, bit_ins(read8(addr + 4), 0, sz - 32, v));
            break;
        }
    }
}
