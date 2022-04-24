#include "cpu_emulation.h"
#include "main.h"
#include "sysdeps.h"
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
inline uint32_t ROL(uint32_t v, uint8_t offset) {
    return v << offset | v >> (32 - offset);
}
inline uint32_t ROR(uint32_t v, uint8_t offset) {
    return v >> offset | v << (32 - offset);
}
template <class T> T get_bf(uint32_t v, uint8_t offset, uint8_t width) {
    return static_cast<T>(ROL(v, offset)) >> (32 - width);
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
OP(bftst_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    bf_r_common(reg, offset, width);
}
OP(bfchg_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    bf_r_common(reg, offset, width);
    regs.d[reg] ^= ROR(0xffffffff >> (32 - width) << (32 - width), offset);
}
OP(bfclr_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    bf_r_common(reg, offset, width);
    regs.d[reg] &= ~ROR(0xffffffff >> (32 - width) << (32 - width), offset);
}
OP(bfset_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    bf_r_common(reg, offset, width);
    regs.d[reg] |= ROR(0xffffffff >> (32 - width) << (32 - width), offset);
}
OP(bftst_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
    bf_m_common(addr, offset, width);
}
OP(bfchg_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
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
OP(bfclr_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
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
OP(bfset_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
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
inline int CLZ(unsigned int v, int width) {
#if defined(__GNUC__) || defined(__clang__)
    if(v == 0) {
        return width;
    } else {
        return __builtin_clz(v) - (32 - width);
    }
#else
    for(int i = 0; i < width; ++i) {
        if(v & 1 << (width - i - 1)) {
            return i;
        }
    }
    return width;
#endif
}
OP(bfextu_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    bf_r_common(reg, offset, width);
    regs.d[op2 >> 12 & 7] = get_bf<uint32_t>(regs.d[reg], offset, width);
}
OP(bfexts_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    bf_r_common(reg, offset, width);
    regs.d[op2 >> 12 & 7] = get_bf<int32_t>(regs.d[reg], offset, width);
}
OP(bfffo_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    int xf = offset;
    bf_r_common(reg, offset, width);
    uint32_t vv = get_bf<uint32_t>(regs.d[reg], offset, width);
    regs.d[op2 >> 12 & 7] = CLZ(vv, width) + xf;
}
OP(bfins_d) {
    uint16_t op2 = FETCH();
    auto [offset, width] = parse_bf(op2);
    uint32_t v = ROR(regs.d[op2 >> 12 & 7] << (32 - width), offset);
    regs.d[reg] &= ~ROR(0xffffffff >> (32 - width) << (32 - width), offset);
    regs.d[reg] |= v;
    regs.v = false;
    regs.c = false;
    int32_t ret = get_bf<int32_t>(regs.d[reg], offset, width);
    regs.z = v == 0;
    regs.n = v & 1 << (31 - offset);
}
OP(bfextu_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
    bf_m_common(addr, offset, width);
    regs.d[op2 >> 12 & 7] = get_bf_m<uint32_t>(addr, offset, width);
}
OP(bfexts_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
    bf_m_common(addr, offset, width);
    regs.d[op2 >> 12 & 7] = get_bf_m<int32_t>(addr, offset, width);
}
OP(bfffo_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
    int xf = offset;
    bf_m_common(addr, offset, width);
    uint32_t vv = get_bf_m<uint32_t>(addr, offset, width);
    regs.d[op2 >> 12 & 7] = CLZ(vv, width) + xf;
}
uint8_t bit_ins(uint8_t oldv, int offset, int width, uint8_t newv) {
    uint8_t mask = 0xff >> (8 - width);
    return (oldv & ~(mask << (8 - offset - width))) |
           ((newv & mask) << (8 - offset - width));
}
OP(bfins_m) {
    uint16_t op2 = FETCH();
    uint32_t addr = EA_Addr(type, reg, 0, false);
    auto [offset, width] = parse_bf(op2);
    addr += offset >> 3;
    offset = offset & 7;
    int offset_8n = 8 - offset;
    regs.v = false;
    regs.c = false;
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
        write8(addr, bit_ins(read8(addr), offset, 8 - offset, v >> (sz - 8)));
        write8(addr + 1, bit_ins(read8(addr + 1), 0, sz - 8, v));
        break;
    case 2:
        // OOO VVVVV-VVVVVVVV-VVV XXXXX
        write8(addr, bit_ins(read8(addr), offset, 8 - offset, v >> (sz - 8)));
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