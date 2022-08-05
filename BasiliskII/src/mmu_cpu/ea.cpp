#include "ea.h"
#include "exception.h"
#include "intop.h"
#include "memory.h"
#include "newcpu.h"
#include <stdint.h>
const static uint32_t ZERO = 0;
std::function<uint32_t()> ea_ext(int base) {
    uint16_t nextw = FETCH();
    bool full = (nextw >> 8 & 1);
    int reg2 = nextw >> 12 & 15;
    bool wl = nextw >> 11 & 1;
    int scale = nextw >> 9 & 3;
    if(!full) {
        int8_t disp = nextw & 0xff;
        if(wl) {
            return [base, reg2, scale, disp]() {
                return regs.a[base] + (regs.r[reg2] << scale) + disp;
            };
        } else {
            return [base, reg2, scale, disp]() {
                return regs.a[base] + (DO_EXT_L(regs.r[reg2]) << scale) + disp;
            };
        }
    }
    if(nextw & 1 << 3) {
        ILLEGAL_INST();
    }
    bool bs = nextw >> 7 & 1;
    bool is = nextw >> 6 & 1;
    int bd_c = nextw >> 4 & 3;
    bool post = nextw >> 2 & 1;
    int od_c = nextw & 3;
    int32_t bd = 0;
    int32_t od = 0;
    switch(bd_c) {
    case 0:
        ILLEGAL_INST();
    case 1:
        break;
    case 2:
        bd = (int16_t)FETCH();
        break;
    case 3:
        bd = FETCH32();
        break;
    }
    const uint32_t *basep = (bs ? &ZERO : &regs.a[base]);
    switch(od_c) {
    case 0:
        if(post) {
            ILLEGAL_INST();
        }
        if(!is) {
            return [bd, basep, reg2, scale]() {
                return bd + *basep + (regs.r[reg2] << scale);
            };
        } else {
            return [bd, basep]() { return bd + *basep; };
        }
    case 1:
        break;
    case 2:
        od = (int16_t)FETCH();
        break;
    case 3:
        od = FETCH32();
        break;
    }
    if(is) {
        return [bd, od, basep]() {
            uint32_t vx = read32(bd + *basep);
            return vx + od;
        };
    }
    if(!post) {
        return [bd, od, basep, reg2, scale]() {
            uint32_t vx = read32(bd + *basep + (regs.r[reg2] << scale));
            return vx + od;
        };
    } else {
        return [bd, od, basep, reg2, scale]() {
            uint32_t vx = read32(bd + *basep);
            return vx + od + (regs.r[reg2] << scale);
        };
    }
    ILLEGAL_INST();
}

std::function<uint32_t()> ea_ext_pc(uint32_t pc) {
    uint16_t nextw = FETCH();
    bool full = (nextw >> 8 & 1);
    int reg2 = nextw >> 12 & 15;
    bool wl = nextw >> 11 & 1;
    int scale = nextw >> 9 & 3;
    if(!full) {
        uint32_t pc_disp = pc + static_cast<int8_t>(nextw & 0xff);
        if(wl) {
            return [pc_disp, reg2, scale]() {
                return pc_disp + (regs.r[reg2] << scale);
            };
        } else {
            return [pc_disp, reg2, scale]() {
                return pc_disp + (DO_EXT_L(regs.r[reg2]) << scale);
            };
        }
    }
    if(nextw & 1 << 3) {
        ILLEGAL_INST();
    }
    bool bs = nextw >> 7 & 1;
    bool is = nextw >> 6 & 1;
    int bd_c = nextw >> 4 & 3;
    bool post = nextw >> 2 & 1;
    int od_c = nextw & 3;
    uint32_t base = (bs ? 0 : pc);
    int32_t bd = 0;
    int32_t od = 0;
    switch(bd_c) {
    case 0:
        ILLEGAL_INST();
    case 1:
        break;
    case 2:
        bd = (int16_t)FETCH();
        break;
    case 3:
        bd = FETCH32();
        break;
    }
    base += bd;
    switch(od_c) {
    case 0:
        if(post) {
            ILLEGAL_INST();
        }
        if(!is) {
            return [base, reg2, scale]() {
                return base + (regs.r[reg2] << scale);
            };
        } else {
            return [base]() { return base; };
        }
    case 1:
        break;
    case 2:
        od = (int16_t)FETCH();
        break;
    case 3:
        od = FETCH32();
        break;
    }
    if(is) {
        return [bd, od, base]() {
            uint32_t vx = read32(base);
            return vx + od;
        };
    }
    if(!post) {
        return [od, base, reg2, scale]() {
            uint32_t vx = read32(base + (regs.r[reg2] << scale));
            return vx + od;
        };
    } else {
        return [od, base, reg2, scale]() {
            uint32_t vx = read32(base);
            return vx + od + (regs.r[reg2] << scale);
        };
    }
    ILLEGAL_INST();
}
uint8_t EA_READ8(int type, int reg, bool override) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return regs.d[reg];
    case EA_OP::AR:
        ILLEGAL_INST();
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        if(reg == 4 && !override) {
            return FETCH();
        } else {
            ILLEGAL_INST();
        }
    default:
        break;
    }
    regs.ea_rw = override;
    return read8(regs.i_ea = EA_Addr(type, reg, 1, override));
}

uint16_t EA_READ16(int type, int reg, bool override) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return regs.d[reg];
    case EA_OP::AR:
        return regs.a[reg];
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        if(reg == 4 && !override) {
            return FETCH();
        } else {
            ILLEGAL_INST();
        }
    default:
        break;
    }
    regs.ea_rw = override;
    return read16(regs.i_ea = EA_Addr(type, reg, 2, override));
}

uint32_t EA_READ32(int type, int reg, bool override) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return regs.d[reg];
    case EA_OP::AR:
        return regs.a[reg];
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        if(reg == 4 && !override) {
            return FETCH32();
        } else {
            ILLEGAL_INST();
        }
    default:
        break;
    }
    regs.ea_rw = override;
    return read32(regs.i_ea = EA_Addr(type, reg, 4, override));
}

void EA_UPDATE8(int type, int reg, uint8_t v) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        regs.d[reg] = (regs.d[reg] & 0xffffff00) | v;
        return;
    case EA_OP::AR:
        ILLEGAL_INST();
    default:
        write8(regs.i_ea, v);
    }
}
void EA_WRITE8(int type, int reg, uint8_t v) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        regs.d[reg] = (regs.d[reg] & 0xffffff00) | v;
        return;
    case EA_OP::AR:
        ILLEGAL_INST();
    default:
        regs.i_ea = EA_Addr(type, reg, 1, true);
        write8(regs.i_ea, v);
    }
}
void EA_UPDATE16(int type, int reg, uint16_t v) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        regs.d[reg] = (regs.d[reg] & 0xffff0000) | v;
        return;
    case EA_OP::AR:
        regs.a[reg] = DO_EXT_L(v);
        return;
    default:
        write16(regs.i_ea, v);
    }
}
void EA_WRITE16(int type, int reg, uint16_t v) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        regs.d[reg] = (regs.d[reg] & 0xffff0000) | v;
        return;
    case EA_OP::AR:
        regs.a[reg] = DO_EXT_L(v);
        return;
    default:
        regs.i_ea = EA_Addr(type, reg, 2, true);
        write16(regs.i_ea, v);
    }
}

void EA_UPDATE32(int type, int reg, uint32_t v) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        regs.d[reg] = v;
        return;
    case EA_OP::AR:
        regs.a[reg] = v;
        return;
    default:
        write32(regs.i_ea, v);
    }
}
void EA_WRITE32(int type, int reg, uint32_t v) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        regs.d[reg] = v;
        return;
    case EA_OP::AR:
        regs.a[reg] = v;
        return;
    default:
        regs.i_ea = EA_Addr(type, reg, 4, true);
        write32(regs.i_ea, v);
    }
}

std::function<uint32_t()> ea_addr(int type, int reg, int sz, bool w) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
    case EA_OP::AR:
        ILLEGAL_INST();
    case EA_OP::MEM:
        return [reg]() { return regs.a[reg]; };
    case EA_OP::INCR:
        if(sz) {
            if(reg == 7 && sz == 1) {
                sz = 2;
            }
            return [reg, sz]() {
                return std::exchange(regs.a[reg], regs.a[reg] + sz);
            };
        } else {
            ILLEGAL_INST();
        }
    case EA_OP::DECR:
        if(sz) {
            if(reg == 7 && sz == 1) {
                sz = 2;
            }
            return [reg, sz]() { return regs.a[reg] -= sz; };
        } else {
            ILLEGAL_INST();
        }
    case EA_OP::OFFSET: {
        int16_t offset = FETCH();
        return [reg, offset]() { return regs.a[reg] + offset; };
    }
    case EA_OP::EXT:
        return ea_ext(reg);
    case EA_OP::EXT2:
        switch(reg) {
        case 0: {
            int16_t addr = FETCH();
            return [addr]() { return addr; };
        }
        case 1: {
            uint32_t addr = FETCH32();
            return [addr]() { return addr; };
        }
        case 2:
            if(!w) {
                int16_t offset = FETCH();
                return [pc = regs.pc - 2 + offset]() { return pc; };
            } else {
                ILLEGAL_INST();
            }
        case 3:
            if(!w) {
                return ea_ext_pc(regs.pc);
            } else {
                ILLEGAL_INST();
            }
        }
    }
    ILLEGAL_INST();
}

uint32_t EA_Addr(int type, int reg, int sz, bool w) {
    return ea_addr(type, reg, sz, w)();
}

std::function<void()> EA_Read8(int type, int reg,
                               const std::function<void(uint8_t)> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() { f(regs.d[reg] & 0xff); };
    case EA_OP::AR:
        ILLEGAL_INST();
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        } else if(reg == 4) {
            uint8_t v = FETCH();
            return [v, f]() { f(v); };
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 1, false)]() {
        regs.i_ea = addr_f();
        f(read8(regs.i_ea));
    };
}

std::function<void()> EA_Read16(int type, int reg,
                                const std::function<void(uint16_t)> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() { f(regs.d[reg] & 0xffff); };
    case EA_OP::AR:
        return [reg, f]() { f(regs.a[reg] & 0xffff); };
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        } else if(reg == 4) {
            uint16_t v = FETCH();
            return [v, f]() { f(v); };
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 2, false)]() {
        regs.i_ea = addr_f();
        f(read16(regs.i_ea));
    };
}

std::function<void()> EA_Read32(int type, int reg,
                                const std::function<void(uint32_t)> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() { f(regs.d[reg]); };
    case EA_OP::AR:
        return [reg, f]() { f(regs.a[reg]); };
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        } else if(reg == 4) {
            uint32_t v = FETCH32();
            return [v, f]() { f(v); };
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 4, false)]() {
        regs.i_ea = addr_f();
        f(read32(regs.i_ea));
    };
}

std::function<void()> EA_Write8(int type, int reg,
                                const std::function<uint8_t()> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() { regs.d[reg] = (regs.d[reg] & 0xffffff00) | f(); };
    case EA_OP::AR:
        ILLEGAL_INST();
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 1, true)]() {
        regs.i_ea = addr_f();
        write8(regs.i_ea, f());
    };
}

std::function<void()> EA_Write16(int type, int reg,
                                 const std::function<uint16_t()> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() { regs.d[reg] = (regs.d[reg] & 0xffff0000) | f(); };
    case EA_OP::AR:
        return [reg, f]() { regs.a[reg] = DO_EXT_L(f()); };
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 2, true)]() {
        regs.i_ea = addr_f();
        write16(regs.i_ea, f());
    };
}

std::function<void()> EA_Write32(int type, int reg,
                                 const std::function<uint32_t()> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() { regs.d[reg] = f(); };
    case EA_OP::AR:
        return [reg, f]() { regs.a[reg] = f(); };
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 4, true)]() {
        regs.i_ea = addr_f();
        write32(regs.i_ea, f());
    };
}

std::function<void()> EA_Update8(int type, int reg,
                                 const std::function<uint8_t(uint8_t)> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() {
            regs.d[reg] = (regs.d[reg] & 0xffffff00) | f(regs.d[reg] & 0xff);
        };
    case EA_OP::AR:
        ILLEGAL_INST();
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 1, true)]() {
        regs.ea_rw = true;
        regs.i_ea = addr_f();
        write8(regs.i_ea, f(read8(regs.i_ea)));
    };
}

std::function<void()> EA_Update16(int type, int reg,
                                  const std::function<uint16_t(uint16_t)> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() {
            regs.d[reg] = (regs.d[reg] & 0xffff0000) | f(regs.d[reg] & 0xffff);
        };
    case EA_OP::AR:
        return [reg, f]() { regs.a[reg] = DO_EXT_L(f(regs.a[reg] & 0xffff)); };
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 2, true)]() {
        regs.ea_rw = true;
        regs.i_ea = addr_f();
        write16(regs.i_ea, f(read16(regs.i_ea)));
    };
}

std::function<void()> EA_Update32(int type, int reg,
                                  const std::function<uint32_t(uint32_t)> &f) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        return [reg, f]() { regs.d[reg] = f(regs.d[reg]); };
    case EA_OP::AR:
        return [reg, f]() { regs.a[reg] = f(regs.a[reg]); };
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        ILLEGAL_INST();
    default:
        break;
    }
    return [f, addr_f = ea_addr(type, reg, 4, true)]() {
        regs.ea_rw = true;
        regs.i_ea = addr_f();
        write32(regs.i_ea, f(read32(regs.i_ea)));
    };
}

