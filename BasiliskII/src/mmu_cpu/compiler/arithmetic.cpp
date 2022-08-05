#include "compiler/compiler.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "exception.h"
#include "intop.h"
#include "newcpu.h"
void CHK_ERROR();

void chk2_s(int32_t low, int32_t high, int32_t v) {
    regs.z = v == low || v == high;
    regs.c = v < low || v > high;
}

void chk2_u(uint32_t low, uint32_t high, uint32_t v) {
    regs.z = v == low || v == high;
    regs.c = v < low || v > high;
}

std::function<void()> compile_cmp2_chk2_b(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int vx_i = op2 >> 12 & 7;
    auto addr_f = ea_addr(type, reg, 0, false);
    switch(op2 & 0x8800) {
    case 0x8800:
        // CHK2(An)
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_s(DO_EXTB_L(read8(regs.i_ea)), DO_EXTB_L(read8(regs.i_ea + 1)),
                   regs.a[vx_i]);
            if(regs.c) {
                CHK_ERROR();
            }
        };
    case 0x8000:
        // CMP2(An)
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_s(DO_EXTB_L(read8(regs.i_ea)), DO_EXTB_L(read8(regs.i_ea + 1)),
                   regs.a[vx_i]);
        };
    case 0x0800:
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_u(read8(regs.i_ea), read8(regs.i_ea + 1), regs.d[vx_i] & 0xff);
            if(regs.c) {
                CHK_ERROR();
            }
        };
    case 0:
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_u(read8(regs.i_ea), read8(regs.i_ea + 1), regs.d[vx_i] & 0xff);
        };
    default:
        __builtin_unreachable();
    }
}

std::function<void()> compile_cmp2_chk2_w(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int vx_i = op2 >> 12 & 7;
    auto addr_f = ea_addr(type, reg, 0, false);
    switch(op2 & 0x8800) {
    case 0x8800:
        // CHK2(An)
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_s(DO_EXT_L(read16(regs.i_ea)), DO_EXT_L(read16(regs.i_ea + 2)),
                   regs.a[vx_i]);
            if(regs.c) {
                CHK_ERROR();
            }
        };
    case 0x8000:
        // CMP2(An)
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_s(DO_EXT_L(read16(regs.i_ea)), DO_EXT_L(read16(regs.i_ea + 2)),
                   regs.a[vx_i]);
        };
    case 0x0800:
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_u(read16(regs.i_ea), read16(regs.i_ea + 2),
                   regs.d[vx_i] & 0xffff);
            if(regs.c) {
                CHK_ERROR();
            }
        };
    case 0:
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_u(read16(regs.i_ea), read16(regs.i_ea + 2),
                   regs.d[vx_i] & 0xffff);
        };
    default:
        __builtin_unreachable();
    }
}

std::function<void()> compile_subi_b(uint16_t op, int type, int reg, int dm) {
    uint8_t v2 = FETCH();
    return EA_Update8(type, reg, [v2](uint8_t v1) { return DO_SUB_B(v1, v2); });
}

std::function<void()> compile_subi_w(uint16_t op, int type, int reg, int dm) {
    uint16_t v2 = FETCH();
    return EA_Update16(type, reg,
                       [v2](uint16_t v1) { return DO_SUB_W(v1, v2); });
}

std::function<void()> compile_subi_l(uint16_t op, int type, int reg, int dm) {
    uint32_t v2 = FETCH32();
    return EA_Update32(type, reg,
                       [v2](uint32_t v1) { return DO_SUB_L(v1, v2); });
}

std::function<void()> compile_cmp2_chk2_l(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int vx_i = op2 >> 12 & 7;
    auto addr_f = ea_addr(type, reg, 0, false);
    switch(op2 & 0x8800) {
    case 0x8800:
        // CHK2(An)
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_s(read32(regs.i_ea), read32(regs.i_ea + 4), regs.a[vx_i]);
            if(regs.c) {
                CHK_ERROR();
            }
        };
    case 0x8000:
        // CMP2(An)
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_s(read32(regs.i_ea), read32(regs.i_ea + 4), regs.a[vx_i]);
        };
    case 0x0800:
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_u(read32(regs.i_ea), read32(regs.i_ea + 4), regs.d[vx_i]);
            if(regs.c) {
                CHK_ERROR();
            }
        };
    case 0:
        return [addr_f = std::move(addr_f), vx_i]() {
            regs.i_ea = addr_f();
            chk2_u(read32(regs.i_ea), read32(regs.i_ea + 4), regs.d[vx_i]);
        };
    default:
        __builtin_unreachable();
    }
}

std::function<void()> compile_addi_b(uint16_t op, int type, int reg, int dm) {
    uint8_t v2 = FETCH();
    return EA_Update8(type, reg, [v2](uint8_t v1) { return DO_ADD_B(v1, v2); });
}

std::function<void()> compile_addi_w(uint16_t op, int type, int reg, int dm) {
    uint16_t v2 = FETCH();
    return EA_Update16(type, reg,
                       [v2](uint16_t v1) { return DO_ADD_W(v1, v2); });
}

std::function<void()> compile_addi_l(uint16_t op, int type, int reg, int dm) {
    uint32_t v2 = FETCH32();
    return EA_Update32(type, reg,
                       [v2](uint32_t v1) { return DO_ADD_L(v1, v2); });
}

std::function<void()> compile_cas_b(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int dc = op2 & 7;
    int du = op2 >> 6 & 7;
    return [ea_f = ea_addr(type, reg, 1, true), dc, du]() {
        regs.traced = true;
        regs.i_ea = ea_f();
        uint8_t d = read8(regs.i_ea);
        DO_CMP_B(d, regs.d[dc]);
        if(regs.z) {
            write8(regs.i_ea, regs.d[du]);
        } else {
            WRITE_D8(dc, d);
        }
    };
}

std::function<void()> compile_cmpi_b(uint16_t op, int type, int reg, int dm) {
    uint8_t v2 = FETCH();
    return EA_Read8(type, reg, [v2](uint8_t v1) { DO_CMP_B(v1, v2); });
}

std::function<void()> compile_cmpi_w(uint16_t op, int type, int reg, int dm) {
    uint16_t v2 = FETCH();
    return EA_Read16(type, reg, [v2](uint16_t v1) { DO_CMP_W(v1, v2); });
}

std::function<void()> compile_cmpi_l(uint16_t op, int type, int reg, int dm) {
    uint32_t v2 = FETCH32();
    return EA_Read32(type, reg, [v2](uint32_t v1) { DO_CMP_L(v1, v2); });
}

std::function<void()> compile_cas_w(uint16_t op, int type, int reg, int dm) {
    if(type == 7 && reg == 4) {
        uint16_t op2 = FETCH();
        uint16_t op3 = FETCH();
        int rn1 = op2 >> 12 & 15;
        int rn2 = op3 >> 12 & 15;
        int du1 = op2 >> 6 & 7;
        int dc1 = op2 & 7;
        int du2 = op3 >> 6 & 7;
        int dc2 = op3 & 7;
        return [rn1, rn2, du1, du2, dc1, dc2]() {
            regs.traced = true;
            uint32_t a1 = regs.r[rn1];
            uint32_t a2 = regs.r[rn2];
            uint16_t v1 = read16(a1);
            uint16_t v2 = read16(a2);
            DO_CMP_W(v1, regs.d[dc1]);
            if(regs.z) {
                DO_CMP_W(v2, regs.d[dc2]);
                if(regs.z) {
                    write16(a1, regs.d[du1]);
                    write16(a2, regs.d[du2]);
                    return;
                }
            }
            WRITE_D16(dc1, v1);
            WRITE_D16(dc2, v2);
        };
    } else {
        uint16_t op2 = FETCH();
        int dc = op2 & 7;
        int du = op2 >> 6 & 7;
        return [ea_f = ea_addr(type, reg, 2, true), dc, du]() {
            regs.traced = true;
            regs.i_ea = ea_f();
            uint16_t d = read16(regs.i_ea);
            DO_CMP_W(d, regs.d[dc]);
            if(regs.z) {
                write16(regs.i_ea, regs.d[du]);
            } else {
                WRITE_D16(dc, d);
            }
        };
    }
}

std::function<void()> compile_cas_l(uint16_t op, int type, int reg, int dm) {
    if(type == 7 && reg == 4) {
        uint16_t op2 = FETCH();
        uint16_t op3 = FETCH();
        int rn1 = op2 >> 12 & 15;
        int rn2 = op3 >> 12 & 15;
        int du1 = op2 >> 6 & 7;
        int dc1 = op2 & 7;
        int du2 = op3 >> 6 & 7;
        int dc2 = op3 & 7;
        return [rn1, rn2, du1, du2, dc1, dc2]() {
            regs.traced = true;
            uint32_t a1 = regs.r[rn1];
            uint32_t a2 = regs.r[rn2];
            uint32_t v1 = read32(a1);
            uint32_t v2 = read32(a2);
            DO_CMP_L(v1, regs.d[dc1]);
            if(regs.z) {
                DO_CMP_L(v2, regs.d[dc2]);
                if(regs.z) {
                    write32(a1, regs.d[du1]);
                    write32(a2, regs.d[du2]);
                    return;
                }
            }
            regs.d[dc1] = v1;
            regs.d[dc2] = v2;
        };
    } else {
        uint16_t op2 = FETCH();
        int dc = op2 & 7;
        int du = op2 >> 6 & 7;
        return [ea_f = ea_addr(type, reg, 4, true), dc, du]() {
            regs.traced = true;
            regs.i_ea = ea_f();
            uint32_t d = read32(regs.i_ea);
            DO_CMP_L(d, regs.d[dc]);
            if(regs.z) {
                write32(regs.i_ea, regs.d[du]);
            } else {
                regs.d[dc] = d;
            }
        };
    }
}

std::function<void()> compile_negx_b(uint16_t op, int type, int reg, int dm) {

    return EA_Update8(type, reg, [](uint8_t v1) {
        bool tp = regs.z;
        auto v = DO_SUB_B(0, v1 + regs.x);
        if(v) {
            regs.z = false;
        } else {
            regs.z = tp;
        }
        return v;
    });
}

std::function<void()> compile_negx_w(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](uint16_t v1) {
        bool tp = regs.z;
        auto v = DO_SUB_W(0, v1 + regs.x);
        if(v) {
            regs.z = false;
        } else {
            regs.z = tp;
        }
        return v;
    });
}

std::function<void()> compile_negx_l(uint16_t op, int type, int reg, int dm) {
    return EA_Update32(type, reg, [](uint32_t v1) {
        bool tp = regs.z;
        auto v = DO_SUB_L(0, v1 + regs.x);
        if(v) {
            regs.z = false;
        } else {
            regs.z = tp;
        }
        return v;
    });
}

std::function<void()> compile_neg_b(uint16_t op, int type, int reg, int dm) {

    return EA_Update8(type, reg, [](uint8_t v1) { return DO_SUB_B(0, v1); });
}

std::function<void()> compile_neg_w(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](uint16_t v1) { return DO_SUB_W(0, v1); });
}

std::function<void()> compile_neg_l(uint16_t op, int type, int reg, int dm) {
    return EA_Update32(type, reg, [](uint32_t v1) { return DO_SUB_L(0, v1); });
}
inline int64_t qmul_s(int32_t v1, int32_t v2) {
    int64_t ret = static_cast<int64_t>(v1) * static_cast<int64_t>(v2);
    regs.n = ret < 0;
    regs.z = ret == 0;
    return ret;
}

inline uint64_t qmul_u(uint32_t v1, uint32_t v2) {
    uint64_t ret = static_cast<uint64_t>(v1) * static_cast<uint64_t>(v2);
    regs.n = static_cast<int64_t>(ret) < 0;
    regs.z = ret == 0;
    return ret;
}

std::function<void()> compile_mul_l(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int dl = op2 >> 12 & 7;
    int dh = op2 & 7;
    bool qw = op2 & 1 << 10;
    if(op2 & 1 << 11) {
        // muls
        if(qw) {
            return EA_Read32(type, reg, [dl, dh](uint32_t v1) {
                regs.c = false;
                int64_t ret = qmul_s(v1, regs.d[dl]);
                regs.d[dl] = ret & 0xffffffff;
                regs.d[dh] = ret >> 32;
            });
        } else {
            return EA_Read32(type, reg, [dl](uint32_t v1) {
                regs.c = false;
                int64_t ret = qmul_s(v1, regs.d[dl]);
                regs.d[dl] = ret & 0xffffffff;
                regs.v = OVERFLOW_S<int32_t>(ret);
            });
        }
    } else {
        // mulu
        if(qw) {
            return EA_Read32(type, reg, [dl, dh](uint32_t v1) {
                regs.c = false;
                int64_t ret = qmul_u(v1, regs.d[dl]);
                regs.d[dl] = ret & 0xffffffff;
                regs.d[dh] = ret >> 32;
            });
        } else {
            return EA_Read32(type, reg, [dl](uint32_t v1) {
                regs.c = false;
                int64_t ret = qmul_u(v1, regs.d[dl]);
                regs.d[dl] = ret & 0xffffffff;
                regs.v = OVERFLOW_U<uint32_t>(ret);
            });
        }
    }
}

inline std::pair<int32_t, int32_t> qdiv_s(int64_t a, int32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    int64_t ret1 = a / b;
    int64_t ret2 = a % b;
    regs.n = ret1 < 0;
    regs.z = ret1 == 0;
    regs.v = OVERFLOW_S<int32_t>(ret1);
    if(regs.v) {
        return {a >> 32, static_cast<int32_t>(a)};
    } else {
        return {ret2, ret1};
    }
}

inline std::pair<uint32_t, uint32_t> qdiv_u(uint64_t a, uint32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    uint64_t ret1 = a / b;
    uint64_t ret2 = a % b;
    regs.n = static_cast<int64_t>(ret1) < 0;
    regs.z = ret1 == 0;
    regs.v = OVERFLOW_U<uint32_t>(ret1);
    if(regs.v) {
        return {a >> 32, static_cast<uint32_t>(a)};
    } else {
        return {ret2, ret1};
    }
}
inline uint64_t to_qword(uint32_t h, uint32_t l) {
    return static_cast<uint64_t>(h) << 32 | l;
}
std::function<void()> compile_div_l(uint16_t op, int type, int reg, int dm) {
    uint16_t op2 = FETCH();
    int dq = op2 >> 12 & 7;
    int dr = op2 & 7;
    bool qw = op2 & 1 << 10;
    if(op2 & 1 << 11) {
        // divs
        if(qw) {
            if(dq != dr) {
                return EA_Read32(type, reg, [dq, dr](uint32_t v1) {
                    int64_t vx = to_qword(regs.d[dr], regs.d[dq]);
                    auto [rem, quo] = qdiv_s(vx, v1);
                    regs.d[dq] = quo;
                    regs.d[dr] = rem;
                });
            } else {
                return EA_Read32(type, reg, [dq](uint32_t v1) {
                    int64_t vx = to_qword(regs.d[dq], regs.d[dq]);
                    auto [rem, quo] = qdiv_s(vx, v1);
                    regs.d[dq] = quo;
                });
            }
        } else {
            if(dq != dr) {
                return EA_Read32(type, reg, [dq, dr](uint32_t v1) {
                    auto [rem, quo] =
                        qdiv_s(static_cast<int32_t>(regs.d[dq]), v1);
                    regs.d[dq] = quo;
                    regs.d[dr] = rem;
                });
            } else {
                return EA_Read32(type, reg, [dq](uint32_t v1) {
                    auto [_, quo] =
                        qdiv_s(static_cast<int32_t>(regs.d[dq]), v1);
                    regs.d[dq] = quo;
                });
            }
        }
    } else {
        // DIVU
        if(qw) {
            if(dq != dr) {
                return EA_Read32(type, reg, [dq, dr](uint32_t v1) {
                    uint64_t vx = to_qword(regs.d[dr], regs.d[dq]);
                    auto [rem, quo] = qdiv_u(vx, v1);
                    regs.d[dq] = quo;
                    regs.d[dr] = rem;
                });
            } else {
                return EA_Read32(type, reg, [dq](uint32_t v1) {
                    uint64_t vx = to_qword(regs.d[dq], regs.d[dq]);
                    auto [rem, quo] = qdiv_u(vx, v1);
                    regs.d[dq] = quo;
                });
            }
        } else {
            if(dq != dr) {
                return EA_Read32(type, reg, [dq, dr](uint32_t v1) {
                    auto [rem, quo] =
                        qdiv_u(static_cast<int32_t>(regs.d[dq]), v1);
                    regs.d[dq] = quo;
                    regs.d[dr] = rem;
                });
            } else {
                return EA_Read32(type, reg, [dq](uint32_t v1) {
                    auto [_, quo] =
                        qdiv_u(static_cast<int32_t>(regs.d[dq]), v1);
                    regs.d[dq] = quo;
                });
            }
        }
    }
}

std::function<void()> compile_chk_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t limit) {
        int32_t v = regs.d[dm];
        if(v < 0) {
            regs.n = true;
            CHK_ERROR();
        }
        if(v > static_cast<int32_t>(limit)) {
            regs.n = false;
            CHK_ERROR();
        }
    });
}
std::function<void()> compile_chk_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t limit) {
        int16_t v = regs.d[dm];
        if(v < 0) {
            regs.n = true;
            CHK_ERROR();
        }
        if(v > static_cast<int16_t>(limit)) {
            regs.n = false;
            CHK_ERROR();
        }
    });
}

std::function<void()> compile_addq_b(uint16_t op, int type, int reg, int dm) {
    int v2 = dm ? dm : 8;
    return EA_Update8(type, reg, [v2](uint8_t v1) { return DO_ADD_B(v1, v2); });
}
std::function<void()> compile_addq_w(uint16_t op, int type, int reg, int dm) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        return [reg, v2]() { regs.a[reg] += v2; };
    } else {
        return EA_Update16(type, reg,
                           [v2](uint16_t v1) { return DO_ADD_W(v1, v2); });
    }
}
std::function<void()> compile_addq_l(uint16_t op, int type, int reg, int dm) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        return [reg, v2]() { regs.a[reg] += v2; };
    } else {
        return EA_Update32(type, reg,
                           [v2](uint32_t v1) { return DO_ADD_L(v1, v2); });
    }
}

std::function<void()> compile_subq_b(uint16_t op, int type, int reg, int dm) {
    int v2 = dm ? dm : 8;
    return EA_Update8(type, reg, [v2](uint8_t v1) { return DO_SUB_B(v1, v2); });
}
std::function<void()> compile_subq_w(uint16_t op, int type, int reg, int dm) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        return [reg, v2]() { regs.a[reg] -= v2; };
    } else {
        return EA_Update16(type, reg,
                           [v2](uint16_t v1) { return DO_SUB_W(v1, v2); });
    }
}
std::function<void()> compile_subq_l(uint16_t op, int type, int reg, int dm) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        return [reg, v2]() { regs.a[reg] -= v2; };
    } else {
        return EA_Update32(type, reg,
                           [v2](uint32_t v1) { return DO_SUB_L(v1, v2); });
    }
}

std::function<void()> compile_divu_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t dy) {
        uint32_t dx = regs.d[dm];
        if(dy == 0) {
            DIV0_ERROR();
        }
        regs.c = false;
        uint32_t q = dx / dy;
        uint32_t r = dx % dy;
        regs.v = (q >> 16) != 0;
        regs.n = q >> 15;
        regs.z = q == 0;
        regs.d[dm] = (q & 0xffff) | (r << 16);
    });
}

std::function<void()> compile_divs_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t dy) {
        int32_t dx = regs.d[dm];
        if(dy == 0) {
            DIV0_ERROR();
        }
        regs.c = false;
        int32_t q = dx / static_cast<int16_t>(dy);
        int32_t r = dx % static_cast<int16_t>(dy);
        regs.v = OVERFLOW_S<int16_t>(q);
        regs.n = q < 0;
        regs.z = q == 0;
        regs.d[dm] = (q & 0xffff) | (r << 16);
    });
}

std::function<void()> compile_sub_b(uint16_t op, int type, int reg, int dm) {
    return EA_Read8(type, reg, [dm](uint8_t v2) {
        WRITE_D8(dm, DO_SUB_B(regs.d[dm], v2));
    });
}
std::function<void()> compile_sub_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v2) {
        WRITE_D16(dm, DO_SUB_W(regs.d[dm], v2));
    });
}
std::function<void()> compile_sub_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v2) {
        regs.d[dm] = DO_SUB_L(regs.d[dm], v2);
    });
}

std::function<void()> compile_suba_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg,
                     [dm](uint16_t v2) { WRITE_A16(dm, regs.a[dm] - v2); });
}

std::function<void()> compile_sub_to_ea_b(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        // SUBX.B
        return [dm, reg]() {
            uint8_t src = regs.d[reg];
            uint8_t dst = regs.d[dm];
            bool oldz = regs.z;
            uint8_t v = DO_SUB_B(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            WRITE_D8(dm, v);
        };
    } else if(type == 1) {
        // SUBX.B
        return [dm, reg]() {
            uint8_t src = read8(--regs.a[reg]);
            uint8_t dst = read8(--regs.a[dm]);
            bool oldz = regs.z;
            uint8_t v = DO_SUB_B(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            write8(regs.a[dm], v);
        };
    } else {
        return EA_Update8(
            type, reg, [dm](uint8_t v1) { return DO_SUB_B(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_sub_to_ea_w(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        // SUBX.W
        return [dm, reg]() {
            uint16_t src = regs.d[reg];
            uint16_t dst = regs.d[dm];
            bool oldz = regs.z;
            uint16_t v = DO_SUB_W(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            WRITE_D16(dm, v);
        };
    } else if(type == 1) {
        // SUBX.W
        return [dm, reg]() {
            uint16_t src = read16(regs.a[reg] -= 2);
            uint16_t dst = read16(regs.a[dm] -= 2);
            bool oldz = regs.z;
            uint16_t v = DO_SUB_W(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            write16(regs.a[dm], v);
        };
    } else {
        return EA_Update16(
            type, reg, [dm](uint16_t v1) { return DO_SUB_W(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_sub_to_ea_l(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        // SUBX.W
        return [dm, reg]() {
            uint32_t src = regs.d[reg];
            uint32_t dst = regs.d[dm];
            bool oldz = regs.z;
            uint32_t v = DO_SUB_L(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            regs.d[dm] = v;
        };
    } else if(type == 1) {
        // SUBX.W
        return [dm, reg]() {
            uint32_t src = read32(regs.a[reg] -= 4);
            uint32_t dst = read32(regs.a[dm] -= 4);
            bool oldz = regs.z;
            uint32_t v = DO_SUB_L(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            write32(regs.a[dm], v);
        };
    } else {
        return EA_Update32(
            type, reg, [dm](uint32_t v1) { return DO_SUB_L(v1, regs.d[dm]); });
    }
}

std::function<void()> compile_suba_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v2) { regs.a[dm] -= v2; });
}

std::function<void()> compile_cmp_b(uint16_t op, int type, int reg, int dm) {
    return EA_Read8(type, reg, [dm](uint8_t v) { DO_CMP_B(regs.d[dm], v); });
}
std::function<void()> compile_cmp_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v) { DO_CMP_W(regs.d[dm], v); });
}
std::function<void()> compile_cmp_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v) { DO_CMP_L(regs.d[dm], v); });
}

std::function<void()> compile_cmpa_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v) {
        DO_CMP_L(regs.a[dm], static_cast<int16_t>(v));
    });
}
std::function<void()> compile_cmpa_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v) { DO_CMP_L(regs.a[dm], v); });
}

std::function<void()> compile_mulu_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v2) {
        uint32_t v1 = regs.d[dm] & 0xffff;
        uint32_t re = v1 * v2;
        TEST_ALL32(re);
        regs.d[dm] = re;
    });
}

std::function<void()> compile_muls_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v2) {
        int32_t v1 = DO_EXT_L(regs.d[dm] & 0xffff);
        int32_t re = v1 * static_cast<int16_t>(v2);
        TEST_ALL32(re);
        regs.d[dm] = re;
    });
}

std::function<void()> compile_add_b(uint16_t op, int type, int reg, int dm) {
    return EA_Read8(type, reg, [dm](uint8_t v2) {
        WRITE_D8(dm, DO_ADD_B(regs.d[dm], v2));
    });
}
std::function<void()> compile_add_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v2) {
        WRITE_D16(dm, DO_ADD_W(regs.d[dm], v2));
    });
}
std::function<void()> compile_add_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v2) {
        regs.d[dm] = DO_ADD_L(regs.d[dm], v2);
    });
}

std::function<void()> compile_adda_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg,
                     [dm](uint16_t v2) { WRITE_A16(dm, regs.a[dm] + v2); });
}


std::function<void()> compile_add_to_ea_b(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        // ADDX.B
        return [dm, reg]() {
            uint8_t src = regs.d[reg];
            uint8_t dst = regs.d[dm];
            bool oldz = regs.z;
            uint8_t v = DO_ADD_B(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            WRITE_D8(dm, v);
        };
    } else if(type == 1) {
        // ADDX.B
        return [dm, reg]() {
            uint8_t src = read8(--regs.a[reg]);
            uint8_t dst = read8(--regs.a[dm]);
            bool oldz = regs.z;
            uint8_t v = DO_ADD_B(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            write8(regs.a[dm], v);
        };
    } else {
        return EA_Update8(
            type, reg, [dm](uint8_t v1) { return DO_ADD_B(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_add_to_ea_w(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        // ADDX.W
        return [dm, reg]() {
            uint16_t src = regs.d[reg];
            uint16_t dst = regs.d[dm];
            bool oldz = regs.z;
            uint16_t v = DO_ADD_W(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            WRITE_D16(dm, v);
        };
    } else if(type == 1) {
        // ADDX.W
        return [dm, reg]() {
            uint16_t src = read16(regs.a[reg] -= 2);
            uint16_t dst = read16(regs.a[dm] -= 2);
            bool oldz = regs.z;
            uint16_t v = DO_ADD_W(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            write16(regs.a[dm], v);
        };
    } else {
        return EA_Update16(
            type, reg, [dm](uint16_t v1) { return DO_ADD_W(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_add_to_ea_l(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        // ADDX.W
        return [dm, reg]() {
            uint32_t src = regs.d[reg];
            uint32_t dst = regs.d[dm];
            bool oldz = regs.z;
            uint32_t v = DO_ADD_L(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            regs.d[dm] = v;
        };
    } else if(type == 1) {
        // ADDX.W
        return [dm, reg]() {
            uint32_t src = read32(regs.a[reg] -= 4);
            uint32_t dst = read32(regs.a[dm] -= 4);
            bool oldz = regs.z;
            uint32_t v = DO_ADD_L(dst, src + regs.x);
            if(v) {
                regs.z = false;
            } else {
                regs.z = oldz;
            }
            write32(regs.a[dm], v);
        };
    } else {
        return EA_Update32(
            type, reg, [dm](uint32_t v1) { return DO_ADD_L(v1, regs.d[dm]); });
    }
}

std::function<void()> compile_adda_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v2) { regs.a[dm] += v2; });
}
