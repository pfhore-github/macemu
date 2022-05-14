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
#include "mbus.h"
#include "newcpu.h"
#include "op.h"
uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s);
OP(moves_b) {
    uint16_t op2 = FETCH();
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    uint32_t vaddr = EA_Addr(type, reg, 1, true);
    uint32_t addr = do_mmu(vaddr, false, w, true);
    try {
        if(w) {
            b_write8(addr, regs.r[rg2]);
        } else {
            WRITE_D8(rg2, b_read8(addr));
        }
    } catch(BUS_ERROR_EX &) {
        paddr pa{vaddr, 0, SZ::BYTE, TT::LFC, TM(w ? regs.dfc : regs.sfc), w};
        BUSERROR(pa, true);
    }
}

OP(moves_w) {
    uint16_t op2 = FETCH();
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    uint32_t vaddr = EA_Addr(type, reg, 2, true);
    uint32_t addr = do_mmu(vaddr, false, w, true);
    try {
        if(w) {
            b_write16(addr, regs.r[rg2]);
        } else {
            WRITE_D16(rg2, b_read16(addr));
        }
    } catch(BUS_ERROR_EX &) {
        paddr pa{vaddr, 0, SZ::WORD, TT::LFC, TM(w ? regs.dfc : regs.sfc), w};
        BUSERROR(pa, true);
    }
}

OP(moves_l) {
    uint16_t op2 = FETCH();
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    uint32_t vaddr = EA_Addr(type, reg, 4, true);
    uint32_t addr = do_mmu(vaddr, false, w, true);
    try {
        if(w) {
            b_write32(addr, regs.r[rg2]);
        } else {
            regs.r[rg2] = b_read32(addr);
        }
    } catch(BUS_ERROR_EX &) {
        paddr pa{vaddr, 0, SZ::LONG, TT::LFC, TM(w ? regs.dfc : regs.sfc), w};
        BUSERROR(pa, true);
    }
}

void op_movep_w_from(int dm, int reg) {
    int16_t disp = FETCH();
    uint32_t addr = regs.a[reg] + disp;
    WRITE_D16(dm, read8(addr) << 8 | read8(addr + 2));
}

void op_movep_l_from(int dm, int reg) {
    int16_t disp = FETCH();
    uint32_t addr = regs.a[reg] + disp;
    regs.d[dm] = read8(addr) << 24 | read8(addr + 2) << 16 |
                 read8(addr + 4) << 8 | read8(addr + 6);
}

void op_movep_w_to(int dm, int reg) {
    int16_t disp = FETCH();
    uint32_t addr = regs.a[reg] + disp;
    uint16_t v = regs.d[dm];
    write8(addr, v >> 8);
    write8(addr + 2, v);
}

void op_movep_l_to(int dm, int reg) {
    int16_t disp = FETCH();
    uint32_t addr = regs.a[reg] + disp;
    uint32_t v = regs.d[dm];
    write8(addr, v >> 24);
    write8(addr + 2, v >> 16);
    write8(addr + 4, v >> 8);
    write8(addr + 6, v);
}

OP(move_b) {
    uint8_t v = EA_READ8(type, reg);
    regs.i_ea = 0;
    TEST_NZ8(v);
    regs.c = false;
    regs.v = false;
    EA_WRITE8(xop >> 6 & 7, dm, v);
}

OP(move_w) {
    uint16_t v = EA_READ16(type, reg);
    regs.i_ea = 0;
    TEST_NZ16(v);
    regs.c = false;
    regs.v = false;
    EA_WRITE16(xop >> 6 & 7, dm, v);
}

OP(move_l) {
    uint32_t v = EA_READ32(type, reg);
    regs.i_ea = 0;
    TEST_NZ32(v);
    regs.c = false;
    regs.v = false;
    EA_WRITE32(xop >> 6 & 7, dm, v);
}

OP(movea_w) {
    int16_t v = EA_READ16(type, reg);
    regs.a[dm] = v;
}

OP(movea_l) {
    uint32_t v = EA_READ32(type, reg);
    regs.a[dm] = v;
}

OP(move_from_sr) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    EA_WRITE16(type, reg, GET_SR());
}

OP(move_from_ccr) { EA_WRITE16(type, reg, GET_CCR()); }

OP(move_to_ccr) { SET_CCR(EA_READ16(type, reg)); }

OP(move_to_sr) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }

    SET_SR(EA_READ16(type, reg));
}

OP(movem_to_w) {
    if(type == 0) {
        op_ext_w(reg);
    } else {
        uint16_t reg_list = FETCH();
        try {
            if(!regs.i_ea) {
                // start movem
                regs.i_ea = EA_Addr(type, reg, 2, true);
            }
            uint32_t vv = regs.i_ea + (type == 4 ? 2 : 0);
            if(type == 4) {
                for(int i = 0; i < 15; ++i) {
                    if(reg_list & (1 << i)) {
                        vv -= 2;
                        write16(vv, regs.r[15 - i]);
                    }
                }
                regs.a[reg] = vv;
            } else {
                for(int i = 0; i < 15; ++i) {
                    if(reg_list & (1 << i)) {
                        write16(vv, regs.r[i]);
                        vv += 2;
                    }
                }
            }
            regs.i_ea = 0;
        } catch(BUS_ERROR_EX &e) {
            regs.err_ssw.cm = true;
            throw e;
        }
    }
}
OP(movem_to_l) {
    if(type == 0) {
        op_ext_l(reg);
    } else {
        uint16_t reg_list = FETCH();
        try {
            if(!regs.i_ea) {
                // start movem
                regs.i_ea = EA_Addr(type, reg, 4, true);
            }
            uint32_t vv = regs.i_ea + (type == 4 ? 4 : 0);
            if(type == 4) {
                for(int i = 0; i < 15; ++i) {
                    if(reg_list & (1 << i)) {
                        vv -= 4;
                        write32(vv, regs.r[15 - i]);
                    }
                }
                regs.a[reg] = vv;
            } else {
                for(int i = 0; i < 15; ++i) {
                    if(reg_list & (1 << i)) {
                        write32(vv, regs.r[i]);
                        vv += 4;
                    }
                }
            }
            regs.i_ea = 0;
        } catch(BUS_ERROR_EX &e) {
            regs.err_ssw.cm = true;
            throw e;
        }
    }
}
OP(movem_from_w) {
    uint16_t reg_list = FETCH();
    try {
        if(!regs.i_ea) {
            // start movem
            regs.i_ea = EA_Addr(type, reg, 2, false);
        }
        uint32_t v = regs.i_ea;
        for(int i = 0; i < 16; ++i) {
            if(reg_list & (1 << i)) {
                EA_WRITE16(i >> 3, i & 7, read16(v));
                v += 2;
            }
        }
        if(type == 3) {
            regs.a[reg] = v;
        }
        regs.i_ea = 0;
    } catch(BUS_ERROR_EX &e) {
        regs.err_ssw.cm = true;
        throw e;
    }
}

OP(movem_from_l) {
    uint16_t reg_list = FETCH();
    try {
        if(!regs.i_ea) {
            // start movem
            regs.i_ea = EA_Addr(type, reg, 4, false);
        }
        uint32_t v = regs.i_ea;
        for(int i = 0; i < 16; ++i) {
            if(reg_list & (1 << i)) {
                regs.r[i] = read32(v);
                v += 4;
            }
        }
        if(type == 3) {
            regs.a[reg] = v;
        }
        regs.i_ea = 0;
    } catch(BUS_ERROR_EX &e) {
        regs.err_ssw.cm = true;
        throw e;
    }
}

void op_move_to_usp(int reg) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    regs.usp = regs.a[reg];
}

void op_move_from_usp(int reg) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    regs.a[reg] = regs.usp;
}

void op_movec_from() {
    uint16_t next = FETCH();
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    int dst = next >> 12 & 15;
    switch(next & 0xfff) {
    case 0x000:
        regs.r[dst] = regs.sfc;
        return;
    case 0x001:
        regs.r[dst] = regs.dfc;
        return;
    case 0x800:
        regs.r[dst] = regs.usp;
        return;
    case 0x801:
        regs.r[dst] = regs.vbr;
        return;
    case 0x002:
        regs.r[dst] = regs.cacr_de << 31 | regs.cacr_ie << 15;
        return;
    case 0x803:
        regs.r[dst] = regs.msp;
        return;
    case 0x804:
        regs.r[dst] = regs.isp;
        return;
    case 0x003:
        regs.r[dst] = regs.tcr_e << 15 | regs.tcr_p << 14;
        return;
    case 0x004:
        regs.r[dst] = regs.ITTR[0].address_base << 24 |
                      regs.ITTR[0].address_mask << 16 | regs.ITTR[0].E << 15 |
                      regs.ITTR[0].S << 13 | regs.ITTR[0].U << 8 |
                      regs.ITTR[0].CM << 5 | regs.ITTR[0].W << 2;
        return;
    case 0x005:
        regs.r[dst] = regs.ITTR[1].address_base << 24 |
                      regs.ITTR[1].address_mask << 16 | regs.ITTR[1].E << 15 |
                      regs.ITTR[1].S << 13 | regs.ITTR[1].U << 8 |
                      regs.ITTR[1].CM << 5 | regs.ITTR[1].W << 2;
        return;
    case 0x006:
        regs.r[dst] = regs.DTTR[0].address_base << 24 |
                      regs.DTTR[0].address_mask << 16 | regs.DTTR[0].E << 15 |
                      regs.DTTR[0].S << 13 | regs.DTTR[0].U << 8 |
                      regs.DTTR[0].CM << 5 | regs.DTTR[0].W << 2;
        return;
    case 0x007:
        regs.r[dst] = regs.DTTR[1].address_base << 24 |
                      regs.DTTR[1].address_mask << 16 | regs.DTTR[1].E << 15 |
                      regs.DTTR[1].S << 13 | regs.DTTR[1].U << 8 |
                      regs.DTTR[1].CM << 5 | regs.DTTR[1].W << 2;
        return;
    case 0x805:
        regs.r[dst] = regs.MMUSR.PA | regs.MMUSR.B << 11 | regs.MMUSR.G << 10 |
                      regs.MMUSR.U << 8 | regs.MMUSR.S << 7 |
                      regs.MMUSR.CM << 5 | regs.MMUSR.M << 4 |
                      regs.MMUSR.W << 2 | regs.MMUSR.T << 1 | regs.MMUSR.R;
        return;
    case 0x806:
        regs.r[dst] = regs.urp;
        return;
    case 0x807:
        regs.r[dst] = regs.srp;
        return;
    default:
        ILLEGAL_INST();
    }
}

void op_movec_to() {
    uint16_t next = FETCH();
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    int src = next >> 12 & 15;
    switch(next & 0xfff) {
    case 0x000:
        regs.sfc = regs.r[src] & 7;
        return;
    case 0x001:
        regs.dfc = regs.r[src] & 7;
        return;
    case 0x800:
        regs.usp = regs.r[src];
        return;
    case 0x801:
        regs.vbr = regs.r[src];
        return;
    case 0x002:
        regs.cacr_de = regs.r[src] >> 31 & 1;
        regs.cacr_ie = regs.r[src] >> 15 & 1;
        return;
    case 0x803:
        regs.msp = regs.r[src];
        return;
    case 0x804:
        regs.isp = regs.r[src];
        return;
    case 0x003:
        regs.tcr_e = regs.r[src] >> 15 & 1;
        regs.tcr_p = regs.r[src] >> 14 & 1;
        return;
    case 0x004:
        regs.ITTR[0].address_base = regs.r[src] >> 24 & 0xff;
        regs.ITTR[0].address_mask = regs.r[src] >> 16 & 0xff;
        regs.ITTR[0].E = regs.r[src] >> 15 & 1;
        regs.ITTR[0].S = regs.r[src] >> 13 & 3;
        regs.ITTR[0].U = regs.r[src] >> 8 & 3;
        regs.ITTR[0].CM = regs.r[src] >> 5 & 3;
        regs.ITTR[0].W = regs.r[src] >> 2 & 1;
        return;
    case 0x005:
        regs.ITTR[1].address_base = regs.r[src] >> 24 & 0xff;
        regs.ITTR[1].address_mask = regs.r[src] >> 16 & 0xff;
        regs.ITTR[1].E = regs.r[src] >> 15 & 1;
        regs.ITTR[1].S = regs.r[src] >> 13 & 3;
        regs.ITTR[1].U = regs.r[src] >> 8 & 3;
        regs.ITTR[1].CM = regs.r[src] >> 5 & 3;
        regs.ITTR[1].W = regs.r[src] >> 2 & 1;
        return;
    case 0x006:
        regs.DTTR[0].address_base = regs.r[src] >> 24 & 0xff;
        regs.DTTR[0].address_mask = regs.r[src] >> 16 & 0xff;
        regs.DTTR[0].E = regs.r[src] >> 15 & 1;
        regs.DTTR[0].S = regs.r[src] >> 13 & 3;
        regs.DTTR[0].U = regs.r[src] >> 8 & 3;
        regs.DTTR[0].CM = regs.r[src] >> 5 & 3;
        regs.DTTR[0].W = regs.r[src] >> 2 & 1;
        return;
    case 0x007:
        regs.DTTR[1].address_base = regs.r[src] >> 24 & 0xff;
        regs.DTTR[1].address_mask = regs.r[src] >> 16 & 0xff;
        regs.DTTR[1].E = regs.r[src] >> 15 & 1;
        regs.DTTR[1].S = regs.r[src] >> 13 & 3;
        regs.DTTR[1].U = regs.r[src] >> 8 & 3;
        regs.DTTR[1].CM = regs.r[src] >> 5 & 3;
        regs.DTTR[1].W = regs.r[src] >> 2 & 1;
        return;
    case 0x805:
        regs.MMUSR.PA = regs.r[src] & 0xFFFFF000;
        regs.MMUSR.B = regs.r[src] >> 11 & 1;
        regs.MMUSR.G = regs.r[src] >> 10 & 1;
        regs.MMUSR.U = regs.r[src] >> 8 & 3;
        regs.MMUSR.S = regs.r[src] >> 7 & 1;
        regs.MMUSR.CM = regs.r[src] >> 5 & 3;
        regs.MMUSR.M = regs.r[src] >> 4 & 1;
        regs.MMUSR.W = regs.r[src] >> 2 & 1;
        regs.MMUSR.T = regs.r[src] >> 1 & 1;
        regs.MMUSR.R = regs.r[src] & 1;
        return;
    case 0x806:
        regs.urp = regs.r[src];
        return;
    case 0x807:
        regs.srp = regs.r[src];
        return;
    default:
        ILLEGAL_INST();
    }
}

OP(moveq) {
    auto v = static_cast<int8_t>(xop & 0x7f);
    v <<= 1;
    v >>= 1;
    TEST_NZ8(v);
    regs.c = false;
    regs.v = false;
    EA_WRITE32(0, dm, int32_t(v));
}

void op_exg_d(int dm, int reg) { std::swap(regs.d[reg], regs.d[dm]); }

void op_exg_a(int dm, int reg) { std::swap(regs.a[reg], regs.a[dm]); }

void op_exg_da(int dm, int reg) { std::swap(regs.a[reg], regs.d[dm]); }

void read_line(uint32_t addr, std::byte *dst) { b_readline(addr, dst); }
void write_line(uint32_t addr, const std::byte *src) { b_writeline(addr, src); }

OP(move16) {
    std::byte buf[16];
    uint32_t addr;
    switch(type) {
    case 0:
        addr = FETCH32();
        read_line(regs.a[reg], buf);
        write_line(addr, buf);
        regs.a[reg] += 16;
        break;
    case 1:
        addr = FETCH32();
        read_line(addr, buf);
        write_line(regs.a[reg], buf);
        regs.a[reg] += 16;
        break;
    case 2:
        addr = FETCH32();
        read_line(regs.a[reg], buf);
        write_line(addr, buf);
        break;
    case 3:
        addr = FETCH32();
        read_line(addr, buf);
        write_line(regs.a[reg], buf);
        break;
    case 4: {
        uint16_t op2 = FETCH();
        int ay = op2 >> 12 & 7;
        read_line(regs.a[reg], buf);
        write_line(regs.a[ay], buf);
        regs.a[reg] += 16;
        regs.a[ay] += 16;
        break;
    }
    default:
        ILLEGAL_INST();
    }
}
