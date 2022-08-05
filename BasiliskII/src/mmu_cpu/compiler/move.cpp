
#include "compiler/compiler.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "exception.h"
#include "intop.h"
#include "mbus.h"
#include "newcpu.h"
void PRIV_ERROR();
std::function<void()> compile_moves_b(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    auto ea_f = ea_addr(type, reg, 1, true);
    if(w) {
        return [f = std::move(ea_f), rg2]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.i_ea = f();
            try {
                b_write8(regs.i_ea, regs.r[rg2]);
            } catch(BUS_ERROR_EX &) {
                throw BUS_ERROR_EX{
                    .addr = regs.i_ea,
                    .ma = false,
                    .atc = false,
                    .lk = false,
                    .rw = false,
                    .size = SZ::BYTE,
                    .tt = TT::LFC,
                    .tm = TM(regs.dfc),
                };
            }
        };
    } else {
        return [f = std::move(ea_f), rg2]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.i_ea = f();
            try {
                WRITE_D8(rg2, b_read8(regs.i_ea));
            } catch(BUS_ERROR_EX &) {
                throw BUS_ERROR_EX{
                    .addr = regs.i_ea,
                    .ma = false,
                    .atc = false,
                    .lk = false,
                    .rw = true,
                    .size = SZ::BYTE,
                    .tt = TT::LFC,
                    .tm = TM(regs.sfc),
                };
            }
        };
    }
}

std::function<void()> compile_moves_w(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    auto ea_f = ea_addr(type, reg, 2, true);
    if(w) {
        return [f = std::move(ea_f), rg2]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.i_ea = f();
            try {
                b_write16(regs.i_ea, regs.r[rg2]);
            } catch(BUS_ERROR_EX &) {
                throw BUS_ERROR_EX{
                    .addr = regs.i_ea,
                    .ma = false,
                    .atc = false,
                    .lk = false,
                    .rw = false,
                    .size = SZ::WORD,
                    .tt = TT::LFC,
                    .tm = TM(regs.dfc),
                };
            }
        };
    } else {
        return [f = std::move(ea_f), rg2]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.i_ea = f();
            try {
                WRITE_D16(rg2, b_read16(regs.i_ea));
            } catch(BUS_ERROR_EX &) {
                throw BUS_ERROR_EX{
                    .addr = regs.i_ea,
                    .ma = false,
                    .atc = false,
                    .lk = false,
                    .rw = true,
                    .size = SZ::WORD,
                    .tt = TT::LFC,
                    .tm = TM(regs.sfc),
                };
            }
        };
    }
}

std::function<void()> compile_moves_l(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    auto ea_f = ea_addr(type, reg, 4, true);
    if(w) {
        return [f = std::move(ea_f), rg2]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.i_ea = f();
            try {
                b_write32(regs.i_ea, regs.r[rg2]);
            } catch(BUS_ERROR_EX &) {
                throw BUS_ERROR_EX{
                    .addr = regs.i_ea,
                    .ma = false,
                    .atc = false,
                    .lk = false,
                    .rw = false,
                    .size = SZ::LONG,
                    .tt = TT::LFC,
                    .tm = TM(regs.dfc),
                };
            }
        };
    } else {
        return [f = std::move(ea_f), rg2]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.i_ea = f();
            try {
                regs.d[rg2] = b_read32(regs.i_ea);
            } catch(BUS_ERROR_EX &) {
                throw BUS_ERROR_EX{
                    .addr = regs.i_ea,
                    .ma = false,
                    .atc = false,
                    .lk = false,
                    .rw = true,
                    .size = SZ::LONG,
                    .tt = TT::LFC,
                    .tm = TM(regs.sfc),
                };
            }
        };
    }
}

std::function<void()> compile_move_from_sr(uint16_t xop, int type, int reg,
                                           int dm) {
    return EA_Write16(type, reg, []() {
        if(!regs.S) {
            PRIV_ERROR();
        }
        return GET_SR();
    });
}

std::function<void()> compile_move_from_ccr(uint16_t xop, int type, int reg,
                                            int dm) {
    return EA_Write16(type, reg, []() { return GET_CCR(); });
}

std::function<void()> compile_move_to_ccr(uint16_t xop, int type, int reg,
                                          int dm) {
    return EA_Read16(type, reg, [](uint16_t v) { return SET_CCR(v & 0xff); });
}

std::function<void()> compile_move_to_sr(uint16_t xop, int type, int reg,
                                         int dm) {
    return EA_Read16(type, reg, [](uint16_t v) {
        if(!regs.S) {
            PRIV_ERROR();
        }
        return SET_SR(v);
    });
}

std::function<void()> compile_movem_to_w(uint16_t op, int type, int reg,
                                         int dm) {
    if(type == 0) {
        // EXT.W
        return [reg]() {
            uint16_t v = DO_EXT_W(regs.d[reg]);
            regs.v = regs.c = false;
            TEST_NZ16(v);
            WRITE_D16(reg, v);
        };
    } else {
        uint16_t reg_list = FETCH();
        if(type == 4) {
            return [ea_f = ea_addr(type, reg, 2, true), reg_list, reg]() {
                try {
                    if(!regs.i_ea) {
                        // start movem
                        regs.i_ea = ea_f();
                    }
                    uint32_t vv = regs.i_ea + 2;
                    for(int i = 0; i < 15; ++i) {
                        if(reg_list & (1 << i)) {
                            vv -= 2;
                            write16(vv, regs.r[15 - i]);
                        }
                    }
                    regs.a[reg] = vv;
                    regs.i_ea = 0;
                } catch(BUS_ERROR_EX &e) {
                    e.cm = true;
                    throw e;
                }
            };
        } else {
            return [ea_f = ea_addr(type, reg, 2, true), reg_list]() {
                try {
                    if(!regs.i_ea) {
                        // start movem
                        regs.i_ea = ea_f();
                    }
                    uint32_t vv = regs.i_ea;
                    for(int i = 0; i < 15; ++i) {
                        if(reg_list & (1 << i)) {
                            write16(vv, regs.r[i]);
                            vv += 2;
                        }
                    }
                    regs.i_ea = 0;
                } catch(BUS_ERROR_EX &e) {
                    e.cm = true;
                    throw e;
                }
            };
        }
    }
}
std::function<void()> compile_movem_to_l(uint16_t op, int type, int reg,
                                         int dm) {
    if(type == 0) {
        // EXT.l
        return [reg]() {
            uint32_t v = DO_EXT_L(regs.d[reg]);
            regs.v = regs.c = false;
            TEST_NZ32(v);
            regs.d[reg] = v;
        };
    } else {
        uint16_t reg_list = FETCH();
        if(type == 4) {
            return [ea_f = ea_addr(type, reg, 4, true), reg_list, reg]() {
                try {
                    if(!regs.i_ea) {
                        // start movem
                        regs.i_ea = ea_f();
                    }
                    uint32_t vv = regs.i_ea + 4;
                    for(int i = 0; i < 15; ++i) {
                        if(reg_list & (1 << i)) {
                            vv -= 4;
                            write32(vv, regs.r[15 - i]);
                        }
                    }
                    regs.a[reg] = vv;
                    regs.i_ea = 0;
                } catch(BUS_ERROR_EX &e) {
                    e.cm = true;
                    throw e;
                }
            };
        } else {
            return [ea_f = ea_addr(type, reg, 4, true), reg_list]() {
                try {
                    if(!regs.i_ea) {
                        // start movem
                        regs.i_ea = ea_f();
                    }
                    uint32_t vv = regs.i_ea;
                    for(int i = 0; i < 15; ++i) {
                        if(reg_list & (1 << i)) {
                            write32(vv, regs.r[i]);
                            vv += 4;
                        }
                    }
                    regs.i_ea = 0;
                } catch(BUS_ERROR_EX &e) {
                    e.cm = true;
                    throw e;
                }
            };
        }
    }
}

std::function<void()> compile_movem_from_w(uint16_t op, int type, int reg,
                                           int dm) {
    uint16_t reg_list = FETCH();
    if(type == 3) {
        return [ea_f = ea_addr(type, reg, 2, false), reg_list, reg]() {
            try {
                if(!regs.i_ea) {
                    // start movem
                    regs.i_ea = ea_f();
                }
                uint32_t v = regs.i_ea;
                for(int i = 0; i < 16; ++i) {
                    if(reg_list & (1 << i)) {
                        regs.r[i] = DO_EXT_L(read16(v));
                        v += 2;
                    }
                }
                regs.a[reg] = v;
                regs.i_ea = 0;
            } catch(BUS_ERROR_EX &e) {
                e.cm = true;
                throw e;
            }
        };
    } else {
        return [ea_f = ea_addr(type, reg, 2, false), reg_list]() {
            try {
                if(!regs.i_ea) {
                    // start movem
                    regs.i_ea = ea_f();
                }
                uint32_t v = regs.i_ea;
                for(int i = 0; i < 16; ++i) {
                    if(reg_list & (1 << i)) {
                        regs.r[i] = DO_EXT_L(read16(v));
                        v += 2;
                    }
                }
                regs.i_ea = 0;
            } catch(BUS_ERROR_EX &e) {
                e.cm = true;
                throw e;
            }
        };
    }
}
std::function<void()> compile_movem_from_l(uint16_t op, int type, int reg,
                                           int dm) {
    uint16_t reg_list = FETCH();
    if(type == 3) {
        return [ea_f = ea_addr(type, reg, 4, false), reg_list, reg]() {
            try {
                if(!regs.i_ea) {
                    // start movem
                    regs.i_ea = ea_f();
                }
                uint32_t v = regs.i_ea;
                for(int i = 0; i < 16; ++i) {
                    if(reg_list & (1 << i)) {
                        regs.r[i] = read32(v);
                        v += 4;
                    }
                }
                regs.a[reg] = v;
                regs.i_ea = 0;
            } catch(BUS_ERROR_EX &e) {
                e.cm = true;
                throw e;
            }
        };
    } else {
        return [ea_f = ea_addr(type, reg, 4, false), reg_list]() {
            try {
                if(!regs.i_ea) {
                    // start movem
                    regs.i_ea = ea_f();
                }
                uint32_t v = regs.i_ea;
                for(int i = 0; i < 16; ++i) {
                    if(reg_list & (1 << i)) {
                        regs.r[i] = read32(v);
                        v += 4;
                    }
                }
                regs.i_ea = 0;
            } catch(BUS_ERROR_EX &e) {
                e.cm = true;
                throw e;
            }
        };
    }
}

std::function<void()> compile_movea_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v) { regs.a[dm] = DO_EXT_L(v); });
}
std::function<void()> compile_movea_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v) { regs.a[dm] = v; });
}

std::function<void()> compile_lea(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [reg]() {
            uint32_t v = DO_EXTB_L(regs.d[reg]);
            regs.v = regs.c = false;
            TEST_NZ32(v);
            regs.d[reg] = v;
        };
    } else {
        return [dm, ea_f = ea_addr(type, reg, 0, false)]() {
            regs.i_ea = ea_f();
            regs.a[dm] = regs.i_ea;
        };
    }
}

std::function<void()> compile_moveq(uint16_t op, int type, int reg, int dm) {
    auto v = static_cast<int8_t>(op & 0xff);
    return [dm, v]() {
        TEST_ALL8(v);
        regs.d[dm] = v;
    };
}

std::function<void()> compile_move_b(uint16_t op, int type, int reg, int dm) {
    int t_type = op >> 6 & 7;
    if(t_type == 0) {
        return EA_Read8(type, reg, [dm](uint8_t v) {
            TEST_ALL8(v);
            WRITE_D8(dm, v);
        });
    } else if(t_type == 7 && dm > 1) {
        ILLEGAL_INST();
    } else {
        return EA_Read8(type, reg,
                        [addr_f = ea_addr(t_type, dm, 1, true)](uint8_t v) {
                            regs.i_ea = addr_f();
                            TEST_ALL8(v);
                            write8(regs.i_ea, v);
                        });
    }
}

std::function<void()> compile_move_w(uint16_t op, int type, int reg, int dm) {
    int t_type = op >> 6 & 7;
    if(t_type == 0) {
        return EA_Read16(type, reg, [dm](uint16_t v) {
            TEST_ALL16(v);
            WRITE_D16(dm, v);
        });
    } else if(t_type == 7 && dm > 1) {
        ILLEGAL_INST();
    } else {
        return EA_Read16(type, reg,
                         [addr_f = ea_addr(t_type, dm, 2, true)](uint16_t v) {
                             regs.i_ea = addr_f();
                             TEST_ALL16(v);
                             write16(regs.i_ea, v);
                         });
    }
}

std::function<void()> compile_move_l(uint16_t op, int type, int reg, int dm) {
    int t_type = op >> 6 & 7;
    if(t_type == 0) {
        return EA_Read32(type, reg, [dm](uint32_t v) {
            TEST_ALL32(v);
            regs.d[dm] = v;
        });
    } else if(t_type == 7 && dm > 1) {
        ILLEGAL_INST();
    } else {
        return EA_Read32(type, reg,
                         [addr_f = ea_addr(t_type, dm, 4, true)](uint32_t v) {
                             regs.i_ea = addr_f();
                             TEST_ALL32(v);
                             write32(regs.i_ea, v);
                         });
    }
}

void read_line(uint32_t addr, std::byte *dst) { b_readline(addr, dst); }
void write_line(uint32_t addr, const std::byte *src) { b_writeline(addr, src); }
std::function<void()> compile_move16(uint16_t op, int type, int reg, int dm) {
    uint32_t addr;
    switch(type) {
    case 0:
        addr = FETCH32();
        return [addr, reg]() {
            std::byte buf[16];
            read_line(regs.a[reg] & ~0xf, buf);
            write_line(addr & ~0xf, buf);
            regs.a[reg] += 16;
        };
    case 1:
        addr = FETCH32();
        return [addr, reg]() {
            std::byte buf[16];
            read_line(addr & ~0xf, buf);
            write_line(regs.a[reg] & ~0xf, buf);
            regs.a[reg] += 16;
        };
    case 2:
        addr = FETCH32();
        return [addr, reg]() {
            std::byte buf[16];
            read_line(regs.a[reg] & ~0xf, buf);
            write_line(addr & ~0xf, buf);
        };
    case 3:
        addr = FETCH32();
        return [addr, reg]() {
            std::byte buf[16];
            read_line(addr & ~0xf, buf);
            write_line(regs.a[reg] & ~0xf, buf);
        };
    case 4: {
        uint16_t op2 = FETCH();
        int ay = op2 >> 12 & 7;
        return [ay, reg]() {
            std::byte buf[16];
            read_line(regs.a[reg] & ~0xf, buf);
            write_line(regs.a[ay] & ~0xf, buf);
            regs.a[reg] += 16;
            regs.a[ay] += 16;
        };
    }
    default:
        ILLEGAL_INST();
    }
}
