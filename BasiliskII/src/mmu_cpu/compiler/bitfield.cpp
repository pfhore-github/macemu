#include "compiler/compiler.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "intop.h"
#include "newcpu.h"
template <class T> T get_bf(uint32_t v, uint8_t offset, uint8_t width) {
    return static_cast<T>(std::rotl<uint32_t>(v, offset)) >> (32 - width);
}
inline int CLZ(unsigned int v, int width) {
    return std::countl_zero(v) - (32 - width);
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
inline void bf_r_common(int reg, int offset, int width) {
    int32_t ret = get_bf<int32_t>(regs.d[reg], offset, width);
    TEST_ALL32(ret);
}

inline void bf_m_common(uint32_t &addr, int &offset, int width) {
    int32_t ret = get_bf_m<int32_t>(addr, offset, width);
    TEST_ALL32(ret);
}

inline std::pair<int, int> get_offset_width(int offset, bool offset_r,
                                            int width, bool width_r) {
    if(offset_r) {
        offset = regs.d[offset & 0xf];
    }
    if(width_r) {
        width = (regs.d[width & 0xf]) & 31;
    }
    if(width == 0) {
        width = 32;
    }
    return {offset, width};
}

std::function<void()> compile_bftst(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_r_common(reg, offset_v, width_v);
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return [offset, width, offset_r, width_r, ea_f = std::move(ea_f)]() {
            uint32_t addr = regs.i_ea = ea_f();
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_m_common(addr, offset_v, width_v);
        };
    }
}
std::function<void()> compile_bfchg(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_r_common(reg, offset_v, width_v);
            regs.d[reg] ^= std::rotr<uint32_t>(
                0xffffffff >> (32 - width_v) << (32 - width_v), offset_v);
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return [offset, width, offset_r, width_r, ea_f = std::move(ea_f)]() {
            uint32_t addr = regs.i_ea = ea_f();
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_m_common(addr, offset_v, width_v);
            int sz = offset_v + width_v;
            if(sz <= 8) {
                write8(addr, read8(addr) ^ (0xff >> offset_v << (8 - sz)));
                return;
            }
            write8(addr, read8(addr) ^ (0xff >> offset_v));
            sz -= 8;
            for(int i = 1; sz > 0; sz -= 8, ++i) {
                if(sz < 8) {
                    write8(addr + i, read8(addr + i) ^ (0xff << (8 - sz)));
                } else {
                    write8(addr + i, ~read8(addr + i));
                }
            }
        };
    }
}
std::function<void()> compile_bfset(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_r_common(reg, offset_v, width_v);
            regs.d[reg] |= std::rotr<uint32_t>(
                0xffffffff >> (32 - width_v) << (32 - width_v), offset_v);
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return [offset, width, offset_r, width_r, ea_f = std::move(ea_f)]() {
            uint32_t addr = regs.i_ea = ea_f();
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_m_common(addr, offset_v, width_v);
            int sz = offset_v + width_v;
            if(sz <= 8) {
                write8(addr, read8(addr) | (0xff >> offset_v << (8 - sz)));
                return;
            }
            write8(addr, read8(addr) | (0xff >> offset_v));
            sz -= 8;
            for(int i = 1; sz > 0; sz -= 8, ++i) {
                if(sz < 8) {
                    write8(addr + i, read8(addr + i) | (0xff << (8 - sz)));
                } else {
                    write8(addr + i, 0xff);
                }
            }
        };
    }
}
std::function<void()> compile_bfclr(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_r_common(reg, offset_v, width_v);
            regs.d[reg] &= ~std::rotr<uint32_t>(
                0xffffffff >> (32 - width_v) << (32 - width_v), offset_v);
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return [offset, width, offset_r, width_r, ea_f = std::move(ea_f)]() {
            uint32_t addr = regs.i_ea = ea_f();
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_m_common(addr, offset_v, width_v);
            int sz = offset_v + width_v;
            if(sz <= 8) {
                write8(addr, read8(addr) & ~(0xff >> offset_v << (8 - sz)));
                return;
            }
            write8(addr, read8(addr) & ~(0xff >> offset_v));
            sz -= 8;
            for(int i = 1; sz > 0; sz -= 8, ++i) {
                if(sz < 8) {
                    write8(addr + i, read8(addr + i) & ~(0xff << (8 - sz)));
                } else {
                    write8(addr + i, 0);
                }
            }
        };
    }
}

std::function<void()> compile_bfextu(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    int dst = op2 >> 12 & 7;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg, dst]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_r_common(reg, offset_v, width_v);
            regs.d[dst] = get_bf<uint32_t>(regs.d[reg], offset_v, width_v);
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return
            [offset, width, offset_r, width_r, dst, ea_f = std::move(ea_f)]() {
                uint32_t addr = regs.i_ea = ea_f();
                auto [offset_v, width_v] =
                    get_offset_width(offset, offset_r, width, width_r);
                bf_m_common(addr, offset_v, width_v);
                regs.d[dst] = get_bf_m<uint32_t>(addr, offset_v, width_v);
            };
    }
}
std::function<void()> compile_bfexts(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    int dst = op2 >> 12 & 7;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg, dst]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            bf_r_common(reg, offset_v, width_v);
            regs.d[dst] = get_bf<int32_t>(regs.d[reg], offset_v, width_v);
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return
            [offset, width, offset_r, width_r, dst, ea_f = std::move(ea_f)]() {
                uint32_t addr = regs.i_ea = ea_f();
                auto [offset_v, width_v] =
                    get_offset_width(offset, offset_r, width, width_r);
                bf_m_common(addr, offset_v, width_v);
                regs.d[dst] = get_bf_m<int32_t>(addr, offset_v, width_v);
            };
    }
}
std::function<void()> compile_bfffo(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    int dst = op2 >> 12 & 7;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg, dst]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            int xf = offset_v;
            bf_r_common(reg, offset_v, width_v);
            uint32_t vv = get_bf<uint32_t>(regs.d[reg], offset_v, width_v);
            regs.d[dst] = CLZ(vv, width) + xf;
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return
            [offset, width, offset_r, width_r, dst, ea_f = std::move(ea_f)]() {
                uint32_t addr = regs.i_ea = ea_f();
                auto [offset_v, width_v] =
                    get_offset_width(offset, offset_r, width, width_r);
                int xf = offset_v;
                bf_m_common(addr, offset_v, width_v);
                uint32_t vv = get_bf_m<uint32_t>(addr, offset_v, width_v);
                regs.d[dst] = CLZ(vv, width_v) + xf;
            };
    }
}
inline uint8_t bit_ins(uint8_t oldv, int offset, int width, uint8_t newv) {
    uint8_t mask = 0xff >> (8 - width);
    return (oldv & ~(mask << (8 - offset - width))) |
           ((newv & mask) << (8 - offset - width));
}

std::function<void()> compile_bfins(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int offset = op2 >> 6 & 0x1f;
    int width = op2 & 0x1f;
    bool offset_r = op2 >> 11 & 1;
    bool width_r = op2 >> 5 & 1;
    int src = op2 >> 12 & 7;
    if(type == 0) {
        return [offset, width, offset_r, width_r, reg, src]() {
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            uint32_t v =
                std::rotr<uint32_t>(regs.d[src] << (32 - width_v), offset_v);
            regs.d[reg] &= ~std::rotr<uint32_t>(
                0xffffffff >> (32 - width_v) << (32 - width_v), offset_v);
            regs.d[reg] |= v;
            regs.z = v == 0;
            regs.n = v & 1 << (31 - offset_v);
        };
    } else {
        auto ea_f = ea_addr(type, reg, 0, false);
        return [offset, width, offset_r, width_r, src,
                ea_f = std::move(ea_f)]() {
            uint32_t addr = regs.i_ea = ea_f();
            auto [offset_v, width_v] =
                get_offset_width(offset, offset_r, width, width_r);
            addr += offset_v >> 3;
            offset_v = offset_v & 7;
            uint32_t v = regs.d[src] << (32 - width_v) >> (32 - width_v);
            regs.z = v == 0;
            regs.n = v & 1 << width_v;
            int sz = offset_v + width_v;
            switch((sz - 1) >> 3) {
            case 0:
                // OOO VVVVV X
                write8(addr, bit_ins(read8(addr), offset_v, width_v, v));
                break;
            case 1:
                // OOO VVVVV-VVV XXXXX
                write8(addr,
                       bit_ins(read8(addr), offset_v, 8 - offset_v, v >> (sz - 8)));
                write8(addr + 1, bit_ins(read8(addr + 1), 0, sz - 8, v));
                break;
            case 2:
                // OOO VVVVV-VVVVVVVV-VVV XXXXX
                write8(addr,
                       bit_ins(read8(addr), offset_v, 8 - offset_v, v >> (sz - 8)));
                write8(addr + 1, v >> (sz - 16));
                write8(addr + 2, bit_ins(read8(addr + 2), 0, sz - 16, v));
                break;
            case 3:
                // OOO VVVVV-VVVVVVVV-VVVVVVVV-VVV XXXXX
                write8(addr, (read8(addr) & ~(0xff >> offset_v)) | v >> (sz - 8));
                write8(addr + 1, v >> (sz - 16));
                write8(addr + 2, v >> (sz - 24));
                write8(addr + 3, bit_ins(read8(addr + 3), 0, sz - 24, v));
                break;
            case 4:
                // OOO VVVVV-VVVVVVVV-VVVVVVVV-VVVVVVVV-VVV XXXXX
                write8(addr, (read8(addr) & ~(0xff >> offset_v)) | v >> (sz - 8));
                write8(addr + 1, v >> (sz - 16));
                write8(addr + 2, v >> (sz - 24));
                write8(addr + 3, v >> (sz - 32));
                write8(addr + 4, bit_ins(read8(addr + 4), 0, sz - 32, v));
                break;
            }
        };
    }
}