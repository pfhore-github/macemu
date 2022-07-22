#include "ea.h"
#include "exception.h"
#include "intop.h"
#include "memory.h"
#include "newcpu.h"
#include <stdint.h>
uint32_t get_eaext(uint32_t base) {
    uint16_t nextw = FETCH();

    bool full = (nextw >> 8 & 1);
    int reg = nextw >> 12 & 15;
    bool wl = nextw >> 11 & 1;
    int scale = nextw >> 9 & 3;
    uint32_t xn = regs.r[reg];
    if(!wl) {
        xn = DO_EXT_L(xn);
    }
    if(!full) {
        int8_t disp = nextw & 0xff;
        return base + (xn << scale) + disp;
    } else {
        if(nextw & 1 << 3) {
            ILLEGAL_INST();
        }
        bool bs = nextw >> 7 & 1;
        bool is = nextw >> 6 & 1;
        int bd_c = nextw >> 4 & 3;
        bool pre = nextw >> 2 & 1;
        int od_c = nextw & 3;
        int32_t bd = 0;
        int32_t od = 0;
        if(pre && bs && is) {
            ILLEGAL_INST();
        }
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
        uint32_t ret = bd;
        if(!bs) {
            ret += base;
        }
        switch(od_c) {
        case 0:
            if(pre) {
                ILLEGAL_INST();
            }
            if(!is) {
                ret += xn << scale;
            }
            return ret;
        case 1:
            break;
        case 2:
            od = (int16_t)FETCH();
            break;
        case 3:
            od = FETCH32();
            break;
        }
        if(!pre) {
            if(!is) {
                ret += xn << scale;
            }
            uint32_t vx = read32(ret);
            return vx + od;
        } else {
            uint32_t vx = read32(ret);
            return vx + (xn << scale) + od;
        }
    }
}

uint32_t EA_Addr(int type, int reg, int sz, bool w) {
    uint32_t &base = regs.a[reg];
    switch(EA_OP{type}) {
    case EA_OP::DR:
        ILLEGAL_INST();
    case EA_OP::AR:
        ILLEGAL_INST();
    case EA_OP::MEM:
        return base;
    case EA_OP::INCR:
        if(sz) {
            if(reg == 7 && sz == 1) {
                return std::exchange(base, base + 2);
            } else {
                return std::exchange(base, base + sz);
            }
        } else {
            ILLEGAL_INST();
        }
    case EA_OP::DECR:
        if(sz) {
            if(reg == 7 && sz == 1) {
                return base -= 2;
            } else {
                return base -= sz;
            }
        } else {
            ILLEGAL_INST();
        }
    case EA_OP::OFFSET:
        return base + (int16_t)FETCH();
    case EA_OP::EXT:
        return get_eaext(base);
    case EA_OP::EXT2:
        switch(reg) {
        case 0:
            return DO_EXT_L(FETCH());
        case 1:
            return FETCH32();
        case 2:
            if(!w) {
                return regs.pc + (int16_t)FETCH();
            }
            break;
        case 3:
            if(!w) {
                return get_eaext(regs.pc);
            }
            break;
        }
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
