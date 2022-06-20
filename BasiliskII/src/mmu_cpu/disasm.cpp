#include "main.h"
#include "memory.h"
#include "sysdeps.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <optional>
#include <stdio.h>
#include <string>
std::string disasm_ea(uint32_t pc, int type, int reg, int sz = 0,
                      int *sz_p = nullptr) {
    switch(type) {
    case 0:
        if(sz_p) {
            *sz_p = 0;
        }
        return fmt::format("%D{}", reg);
    case 1:
        if(sz_p) {
            *sz_p = 0;
        }
        return fmt::format("%A{}", reg);
    case 2:
        if(sz_p) {
            *sz_p = 0;
        }
        return fmt::format("(%A{})", reg);
    case 3:
        if(sz_p) {
            *sz_p = 0;
        }
        return fmt::format("(%A{})+", reg);
    case 4:
        if(sz_p) {
            *sz_p = 0;
        }
        return fmt::format("-(%A{})", reg);
    case 5: {
        if(sz_p) {
            *sz_p = 2;
        }
        int16_t d = read16(pc);
        return fmt::format("({}, %A{})", d, reg);
    }
    case 6: {
        uint16_t ext = read16(pc);
        int ri = ext >> 12 & 15;
        char w = ext >> 11 & 1 ? 'L' : 'W';
        int c = 1 << (ext >> 9 & 3);

        if(!(ext & 1 << 8)) {
            if(sz_p) {
                *sz_p = 2;
            }
            int8_t d = ext & 0xff;
            return fmt::format("({}, %A{}, %R{}.{}*{})", d, reg, ri, w, c);
        } else {
            int32_t bd;
            if(sz_p) {
                *sz_p = 2;
            }
            switch(ext >> 4 & 3) {
            case 0:
                return "<UNDEFINED>";
            case 1:
                bd = 0;
                break;
            case 2:
                if(sz_p) {
                    *sz_p += 2;
                }
                bd = static_cast<int16_t>(read16(pc));
                pc += 2;
                break;
            case 3:
                if(sz_p) {
                    *sz_p = 4;
                }
                bd = read32(pc);
                pc += 4;
                break;
            }
            std::optional<int32_t> od;
            switch(ext & 3) {
            case 0:
                break;
            case 1:
                bd = 0;
                break;
            case 2:
                if(sz_p) {
                    *sz_p += 2;
                }
                od = static_cast<int16_t>(read16(pc));
                break;
            case 3:
                if(sz_p) {
                    *sz_p += 4;
                }
                od = read32(pc);
                break;
            }
            std::string an = (ext >> 7 & 1) ? "" : fmt::format(", %A{}", reg);
            std::string rn =
                (ext >> 6 & 1) ? "" : fmt::format(", %R{}.{}*{}", ri, w, c);
            if(!(ext >> 2 & 1)) {
                if(!od) {
                    return fmt::format("({}{}{})", bd, an, rn);
                } else {
                    return fmt::format("([{}{}{}], {})", bd, an, rn, *od);
                }
            } else {
                if(!od || (an == "" && rn == "")) {
                    return "<UNDEFINED>";
                } else {
                    return fmt::format("([{}{}]{}, {})", bd, an, rn, *od);
                }
            }
        }
        break;
    }
    case 7:
        switch(reg) {
        case 0:
            if(sz_p) {
                *sz_p = 2;
            }
            return fmt::format("(0x{:x})", read16(pc));
        case 1:
            if(sz_p) {
                *sz_p = 4;
            }
            return fmt::format("(0x{:x})", read32(pc));
        case 2: {
            if(sz_p) {
                *sz_p = 2;
            }
            int16_t d = read16(pc);
            return fmt::format("({}, %PC)", d);
        }
        case 3: {
            if(sz_p) {
                *sz_p = 2;
            }

            uint16_t ext = read16(pc);
            int ri = ext >> 12 & 15;
            char w = ext >> 11 & 1 ? 'L' : 'W';
            int c = 1 << (ext >> 9 & 3);

            if(!(ext & 1 << 8)) {
                int8_t d = ext & 0xff;
                return fmt::format("({}, %PC, %R{}.{}*{})", d, ri, w, c);
            } else {
                int32_t bd;
                switch(ext >> 4 & 3) {
                case 0:
                    return "<UNDEFINED>";
                case 1:
                    bd = 0;
                    break;
                case 2:
                    if(sz_p) {
                        *sz_p += 2;
                    }

                    bd = static_cast<int16_t>(read16(pc));
                    pc += 2;
                    break;
                case 3:
                    if(sz_p) {
                        *sz_p += 4;
                    }

                    bd = read32(pc);
                    pc += 4;
                    break;
                }
                std::optional<int32_t> od;
                switch(ext & 3) {
                case 0:
                    break;
                case 1:
                    bd = 0;
                    break;
                case 2:
                    if(sz_p) {
                        *sz_p += 2;
                    }

                    od = static_cast<int16_t>(read16(pc));
                    break;
                case 3:
                    if(sz_p) {
                        *sz_p += 4;
                    }

                    od = read32(pc);
                    break;
                }
                std::string an = (ext >> 7 & 1) ? "" : ", %PC";
                std::string rn =
                    (ext >> 6 & 1) ? "" : fmt::format(", %R{}.{}*{}", ri, w, c);
                if(!(ext >> 2 & 1)) {
                    if(!od) {
                        return fmt::format("({}{}{})", bd, an, rn);
                    } else {
                        return fmt::format("([{}{}{}], {})", bd, an, rn, *od);
                    }
                } else {
                    if(!od || (an == "" && rn == "")) {
                        return "<UNDEFINED>";
                    } else {
                        return fmt::format("([{}{}]{}, {})", bd, an, rn, *od);
                    }
                }
            }
            break;
        }
        case 4:
            switch(sz) {
            case 1:
                if(sz_p) {
                    *sz_p = 2;
                }
                return fmt::format("#0x{:x}", read8(pc));
            case 2:
                if(sz_p) {
                    *sz_p = 2;
                }
                return fmt::format("#0x{:x}", read16(pc));
            case 4:
                if(sz_p) {
                    *sz_p = 4;
                }
                return fmt::format("#0x{:X}", read32(pc));
            case -4:
                if(sz_p) {
                    *sz_p = 4;
                }
                return fmt::format("#{}", bit_cast<float>(read32(pc)));
            case -8: {
                if(sz_p) {
                    *sz_p = 8;
                }
                uint64_t v =
                    static_cast<uint64_t>(read32(pc)) << 32 | read32(pc + 4);
                return fmt::format("#{}", bit_cast<double>(v));
            }
            case 12: {
                // long doble
                if(sz_p) {
                    *sz_p = 12;
                }

                return fmt::format("#{}", read8(pc));
            }
            case -12: {
                // bcd double
                if(sz_p) {
                    *sz_p = 12;
                }

                return "";
            }
            default:
                break;
            }
        }
    }
    return "<UNDEFINED>";
}
static const std::unordered_map<uint16_t, const char *> cr_map = {
    {0x0000, "SFC"},
    {0x0001, "DFC"},
    {0x0800, "USP"},
    {0x0801, "VBR"},
    {0x0002, "CACR"},
    //    { 0x0802, "CAAR"},
    {0x0803, "MSP"},
    {0x0804, "ISP"},
    {0x0003, "TC"},
    {0x0004, "ITT0"},
    {0x0005, "ITT1"},
    {0x0006, "DTT0"},
    {0x0007, "DTT1"},
    {0x0805, "MMUSR"},
    {0x0806, "URP"},
    {0x0807, "SRP"},
};

static const char *cc[] = {"T",  "F",  "HI", "LS", "CC", "CS", "NE", "EQ",
                           "VC", "CS", "PL", "MI", "GE", "LT", "GT", "LE"};

static const char *shifts[] = {"AS", "LS", "ROX", "RO"};
static const char *bit_tst[] = {"TST", "CHG", "CLR", "SET"};

static const char fp_szc[] = {'L', 'S', 'X', 'P', 'W', 'D', 'B'};
static int fp_sz[] = {4, -4, 12, 12, 2, -8, 1};

static const char *fcc[] = {
    "F",    "EQ",  "OGT", "OGE", "OLT", "OLE", "OGL", "OR",
    "UN",   "UEQ", "UGT", "UGE", "ULT", "ULE", "NE",  "T",
    "SF",   "SEQ", "GT",  "GE",  "LT",  "LE",  "GL",  "GLE",
    "NGLE", "NGL", "NLE", "NLT", "NGE", "NGT", "SNE", "ST",

};

static std::unordered_map<uint8_t, const char *> fpu_op = {
    {0x00, "FMOVE"},   {0x01, "FINT"},    {0x02, "FSINH"},   {0x03, "FINTRZ"},
    {0x04, "FSQRT"},   {0x06, "FLOGNP1"}, {0x08, "FETOXM1"}, {0x09, "FTANH"},
    {0x0A, "FATAN"},   {0x0C, "FASIN"},   {0x0D, "FATANH"},  {0x0E, "FSIN"},
    {0x0F, "FTAN"},    {0x10, "FETOX"},   {0x11, "FTWOTOX"}, {0x12, "FTENTOX"},
    {0x14, "FLOGN"},   {0x16, "FLOG2"},   {0x18, "FABS"},    {0x19, "FCOSH"},
    {0x1A, "FNEG"},    {0x1C, "FACOS"},   {0x1D, "FCOS"},    {0x1E, "FGETEXP"},
    {0x1F, "FGETMAN"}, {0x20, "FDIV"},    {0x21, "FMOD"},    {0x22, "FADD"},
    {0x23, "FMUL"},    {0x24, "FSGLDIV"}, {0x25, "FREM"},    {0x27, "FSGLMUL"},
    {0x28, "FSUB"},    {0x38, "FCMP"},    {0x40, "FSMOVE"},  {0x41, "FSSQRT"},
    {0x44, "FDMOVE"},  {0x45, "FDSQRT"},  {0x58, "FSABS"},   {0x5A, "FSNEG"},
    {0x5C, "FDABS"},   {0x5E, "FDNEG"},   {0x60, "FSDIV"},   {0x62, "FSADD"},
    {0x63, "FSMUL"},   {0x64, "FDDIV"},   {0x66, "FDADD"},   {0x67, "FDMUL"},
    {0x68, "FSSUB"},   {0x6C, "FDSUB"}};
std::string to_regname(int i) {
    if(i < 8) {
        return fmt::format("%D{}", i);
    } else {
        return fmt::format("%A{}", i);
    }
}
std::string to_reglists(uint16_t list, bool rv) {
    static int rr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    static int rr2[] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    std::string regs;
    int nowr = -1;
    for(int i = 0; i < 16; ++i) {
        if(list >> (rv ? rr2[i] : rr[i]) & 1) {
            if(nowr == -1) {
                nowr = i;
                if(regs == "") {
                    regs = to_regname(i);
                } else {
                    regs += fmt::format("/{}", to_regname(i));
                }
            }
        } else if(nowr != -1) {
            regs += fmt::format("-{}", to_regname(i - 1));
            nowr = -1;
        }
    }
    if(nowr != -1) {
        regs += "-%A7";
    }
    return regs;
}

std::string to_fpreglists(uint16_t list, bool rv) {
    static int rr[] = {7, 6, 5, 4, 3, 2, 1, 0};
    static int rr2[] = {0, 1, 2, 3, 4, 5, 6, 7};
    std::string regs;
    int nowr = -1;
    for(int i = 0; i < 8; ++i) {
        if(list >> (rv ? rr2[i] : rr[i]) & 1) {
            if(nowr == -1) {
                nowr = i;
                if(regs == "") {
                    regs = fmt::format("%FP{}", i);
                } else {
                    regs += fmt::format("/%FP{}", i);
                }
            }
        } else if(nowr != -1) {
            regs += fmt::format("-%FP{}", i - 1);
            nowr = -1;
        }
    }
    if(nowr != -1) {
        regs += "-%FP7";
    }
    return regs;
}
std::string disasm(uint32_t pc) {
    uint16_t op = read16(pc);
    uint16_t nextop = read16(pc + 2);
    uint16_t imm16 = read16(pc + 2);
    uint32_t imm32 = read32(pc + 2);
    int8_t simm8 = imm16;
    int16_t simm16 = imm16;
    int32_t simm32 = imm32;
    pc += 2;
    unsigned int tp = op >> 3 & 7;
    unsigned int rg = op & 7;
    unsigned int dn = op >> 9 & 7;
    unsigned int opl = (op >> 6) & 7;
    switch(op >> 12) {
    case 000:
        switch(opl) {
        case 0:
            switch(dn) {
            case 0:
                if(tp == 7 && rg == 4) {
                    return fmt::format("ORI #{:X}, %CCR", imm16);
                } else {
                    return fmt::format("ORI.B #{:X}, {}", imm16,
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 1:
                if(tp == 7 && rg == 4) {
                    return fmt::format("ANDI #{:X}, %CCR", imm16);
                } else {
                    return fmt::format("ANDI.B #{:X}, {}", imm16,
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 2:
                return fmt::format("SUBI.B #{:X}, {}", simm8,
                                   disasm_ea(pc + 2, tp, rg));
            case 3:
                return fmt::format("ADDI.B #{:X}, {}", simm8,
                                   disasm_ea(pc + 2, tp, rg));
            case 4:
                return fmt::format("BTST.L #{}, {}", imm16,
                                   disasm_ea(pc + 2, tp, rg));
            case 5:
                if(tp == 7 && rg == 4) {
                    return fmt::format("EORI #{:X}, %CCR", imm16);
                } else {
                    return fmt::format("EORI.B #{:X}, {}", imm16,
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 6:
                return fmt::format("CMPI.B #{:X}, {}", simm8,
                                   disasm_ea(pc + 2, tp, rg, 1));
            case 7: {
                int rn = nextop >> 12;
                if(!(nextop >> 11 & 1)) {
                    return fmt::format("MOVES.B {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                } else {
                    return fmt::format("MOVES.B %R{1}, {0}, ",
                                       disasm_ea(pc + 4, tp, rg), rn);
                }
            }
            }
            break;

        case 1:
            switch(dn) {
            case 0:
                if(tp == 7 && rg == 4) {
                    return fmt::format("ORI #{:X}, %SR", imm16);
                } else {
                    return fmt::format("ORI.W #{:X}, {}", imm16,
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 1:
                if(tp == 7 && rg == 4) {
                    return fmt::format("ANDI #{:X}, %SR", imm16);
                } else {
                    return fmt::format("ANDI.W #{:X}, {}", imm16,
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 2:
                return fmt::format("SUBI.W #{:X}, {}", simm16,
                                   disasm_ea(pc + 2, tp, rg));
            case 3:
                return fmt::format("ADDI.W #{:X}, {}", simm16,
                                   disasm_ea(pc + 2, tp, rg));
            case 4:
                return fmt::format("BCHG.L #{}, {}", imm16,
                                   disasm_ea(pc + 2, tp, rg));
            case 5:
                if(tp == 7 && rg == 4) {
                    return fmt::format("EORI #{:X}, %CCR", imm16);
                } else {
                    return fmt::format("EORI.W #{:X}, {}", imm16,
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 6:
                return fmt::format("CMPI.W #{:X}, {}", simm16,
                                   disasm_ea(pc + 2, tp, rg, 2));
            case 7: {
                int rn = nextop >> 12;
                if(!(nextop >> 11 & 1)) {
                    return fmt::format("MOVES.W {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                } else {
                    return fmt::format("MOVES.W %R{1}, {0}, ",
                                       disasm_ea(pc + 4, tp, rg), rn);
                }
            }
            }
            break;
        case 2:
            switch(dn) {
            case 0:
                return fmt::format("ORI.L #{:X}, {}", imm32,
                                   disasm_ea(pc + 2, tp, rg));
            case 1:
                return fmt::format("ANDI.L #{:X}, {}", imm32,
                                   disasm_ea(pc + 2, tp, rg));
            case 2:
                return fmt::format("SUBI.L #{:X}, {}", simm32,
                                   disasm_ea(pc + 2, tp, rg));
            case 3:
                return fmt::format("ADDI.L #{:X}, {}", simm32,
                                   disasm_ea(pc + 2, tp, rg));
            case 4:
                return fmt::format("BCLR.L #{}, {}", imm16,
                                   disasm_ea(pc + 2, tp, rg));
            case 5:
                return fmt::format("EORI.L #{:X}, {}", imm32,
                                   disasm_ea(pc + 2, tp, rg));
            case 6:
                return fmt::format("CMPI.L #{:X}, {}", simm32,
                                   disasm_ea(pc + 2, tp, rg, 2));
            case 7: {
                int rn = nextop >> 12;
                if(!(nextop >> 11 & 1)) {
                    return fmt::format("MOVES.L {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                } else {
                    return fmt::format("MOVES.L %R{1}, {0}, ",
                                       disasm_ea(pc + 4, tp, rg), rn);
                }
            }
            }
            break;
        case 3:
            switch(dn) {
            case 0: {
                int rn = nextop >> 12;
                if(!(nextop >> 11 & 1)) {
                    return fmt::format("CMP2.B {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                } else {
                    return fmt::format("CHK2.B {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                }
            }
            case 1: {
                int rn = nextop >> 12;
                if(!(nextop >> 11 & 1)) {
                    return fmt::format("CMP2.W {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                } else {
                    return fmt::format("CHK2.W {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                }
            }
            case 2: {
                int rn = nextop >> 12;
                if(!(nextop >> 11 & 1)) {
                    return fmt::format("CMP2.L {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                } else {
                    return fmt::format("CHK2.L {0}, %R{1}",
                                       disasm_ea(pc + 4, tp, rg), rn);
                }
            }
            case 3:
                break;
            case 4:
                return fmt::format("BSET.L #{}, {}", imm16,
                                   disasm_ea(pc + 2, tp, rg));
            case 5:
                return fmt::format("CAS.B %D{}, %D{}, {}", nextop & 7,
                                   nextop >> 6 & 7, disasm_ea(pc + 2, tp, rg));
            case 6:
                if(tp == 7 && rg == 4) {
                    uint16_t op3 = read16(pc + 4);
                    return fmt::format(
                        "CAS2.W %D{0}:%D{3}, %D{1}:%D{4}, (%R{2}):(%R{5})",
                        nextop & 7, nextop >> 6 & 7, nextop >> 12 & 15, op3 & 7,
                        op3 >> 6 & 7, op3 >> 12 & 15);
                } else {
                    return fmt::format("CAS.W %D{}, %D{}, {}", nextop & 7,
                                       nextop >> 6 & 7,
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 7:
                if(tp == 7 && rg == 4) {
                    uint16_t op3 = read16(pc + 4);
                    return fmt::format(
                        "CAS2.L %D{0}:%D{3}, %D{1}:%D{4}, (%R{2}):(%R{5})",
                        nextop & 7, nextop >> 6 & 7, nextop >> 12 & 15, op3 & 7,
                        op3 >> 6 & 7, op3 >> 12 & 15);
                } else {
                    return fmt::format("CAS.L %D{}, %D{}, {}", nextop & 7,
                                       nextop >> 6 & 7,
                                       disasm_ea(pc + 2, tp, rg));
                }
            }
            break;
        case 4:
            if(tp == 1) {
                return fmt::format("MOVEP.W (#{2}, %A{1}), %D{0}", dn, rg,
                                   simm16);
            } else {
                return fmt::format("BTST.L %D{}, %D{}", dn, rg);
            }
            break;
        case 5:
            if(tp == 1) {
                return fmt::format("MOVEP.L (#{2}, %A{1}), %D{0}", dn, rg,
                                   simm16);
            } else {
                return fmt::format("BCHG.L %D{}, %D{}", dn, rg);
            }
        case 6:
            if(tp == 1) {
                return fmt::format("MOVEP.W %D{0}, (#{2}, %A{1})", dn, rg,
                                   simm16);
            } else {
                return fmt::format("BCLR.L %D{}, %D{}", dn, rg);
            }
        case 7:
            if(tp == 1) {
                return fmt::format("MOVEP.L %D{0}, (#{2}, %A{1})", dn, rg,
                                   simm16);
            } else {
                return fmt::format("BSET.L %D{}, %D{}", dn, rg);
            }
        }
        break;
    case 001:
        if(opl == 1) {
            break;
        } else {
            int szz;
            std::string sc = disasm_ea(pc, tp, rg, 1, &szz);
            std::string dt = disasm_ea(pc + szz, op >> 6 & 7, dn);
            return fmt::format("MOVE.B {}, {}", sc, dt);
            break;
        }
    case 002:
        if(opl == 1) {
            return fmt::format("MOVEA.L {}, %A{}", disasm_ea(pc, tp, rg, 4),
                               dn);
        } else {
            int szz;
            std::string sc = disasm_ea(pc, tp, rg, 4, &szz);
            std::string dt = disasm_ea(pc + szz, op >> 6 & 7, dn);
            return fmt::format("MOVE.L {}, {}", sc, dt);
        }
    case 003:
        if(opl == 1) {
            return fmt::format("MOVEA.W {}, %A{}", disasm_ea(pc, tp, rg, 2),
                               dn);
        } else {
            int szz;
            std::string sc = disasm_ea(pc, tp, rg, 2, &szz);
            std::string dt = disasm_ea(pc + szz, op >> 6 & 7, dn);
            return fmt::format("MOVE.W {}, {}", sc, dt);
        }

    case 004:
        switch(opl) {
        case 0:
            switch(dn) {
            case 0:
                return fmt::format("NEGX.B {}", disasm_ea(pc, tp, rg));
            case 1:
                return fmt::format("CLR.B {}", disasm_ea(pc, tp, rg));
            case 2:
                return fmt::format("NEG.B {}", disasm_ea(pc, tp, rg));
            case 3:
                return fmt::format("NOT.B {}", disasm_ea(pc, tp, rg));
            case 4:
                if(tp == 1) {
                    return fmt::format("LINK %A{}, #{}", disasm_ea(pc, tp, rg),
                                       simm32);
                } else {
                    return fmt::format("NBCD {}", disasm_ea(pc, tp, rg));
                }
            case 5:
                return fmt::format("TST.B {}", disasm_ea(pc, tp, rg, 1));
            case 6:
                switch(nextop >> 10 & 3) {
                case 0:
                    return fmt::format("MULU.L {}, %D{}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7);
                case 1:
                    return fmt::format("MULU.L {}, %D{}-%D{}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7, nextop & 7);
                case 2:
                    return fmt::format("MULS.L {}, %D{}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7);
                case 3:
                    return fmt::format("MULS.L {}, %D{}-%D{}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7, nextop & 7);
                }
            case 7:
                break;
            }
            break;
        case 1:
            switch(dn) {
            case 0:
                return fmt::format("NEGX.W {}", disasm_ea(pc, tp, rg));
            case 1:
                return fmt::format("CLR.W {}", disasm_ea(pc, tp, rg));
            case 2:
                return fmt::format("NEG.W {}", disasm_ea(pc, tp, rg));
            case 3:
                return fmt::format("NOT.W {}", disasm_ea(pc, tp, rg));
            case 4:
                if(tp == 0) {
                    return fmt::format("SWAP %D{}", rg);
                } else if(tp == 1) {
                    return fmt::format("BKPT #{}", rg);
                } else {
                    return fmt::format("PEA {}", disasm_ea(pc, tp, rg));
                }
            case 5:
                return fmt::format("TST.W {}", disasm_ea(pc, tp, rg, 2));
            case 6:
                switch(nextop >> 10 & 3) {
                case 0:
                    return fmt::format("DIVU.L {}, %D{}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7);
                case 1:
                    return fmt::format("DIVU.L {0}, %D{2}-%D{1}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7, nextop & 7);
                case 2:
                    return fmt::format("DIVS.L {}, %D{}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7);
                case 3:
                    return fmt::format("DIVS.L {0}, %D{2}:%D{1}",
                                       disasm_ea(pc, tp, rg, 4),
                                       nextop >> 12 & 7, nextop & 7);
                }
                break;
            case 7:
                switch(tp) {
                case 0:
                case 1:
                    return fmt::format("TRAP #{}", tp << 3 | rg);
                case 2:
                    return fmt::format("LINK %A{}, #{}", rg, simm16);
                case 3:
                    return fmt::format("UNLK %A{}", rg);
                case 4:
                    return fmt::format("MOVE %A{}, %USP", rg);
                case 5:
                    return fmt::format("MOVE %USP, %A{}", rg);
                case 6:
                    switch(rg) {
                    case 0:
                        return "RESET";
                    case 1:
                        return "NOP";
                    case 2:
                        return fmt::format("STOP #{}", imm16);
                    case 3:
                        return "RTE";
                    case 4:
                        return fmt::format("RTD #{}", simm16);
                    case 5:
                        return "RTS";
                    case 6:
                        return "TRAPV";
                    case 7:
                        return "RTR";
                    }
                    break;
                case 7: {
                    int rn = nextop >> 12 & 7;
                    if(rg == 2) {
                        return fmt::format("MOVEC %{0}, %R{1}",
                                           cr_map.at(nextop & 0xfff), rn);
                    } else if(rg == 3) {
                        return fmt::format("MOVEC %R{1}, %{0}",
                                           cr_map.at(nextop & 0xfff), rn);
                    }
                    break;
                }
                }
            }
            break;
        case 2:
            switch(dn) {
            case 4:
                if(tp == 0) {
                    return fmt::format("EXT.W %D{}", rg);
                } else {
                    uint16_t rl = read16(pc);
                    auto regs = to_reglists(rl, tp == 4);
                    auto ea = disasm_ea(pc + 2, tp, rg);
                    return fmt::format("MOVEM.W {}, {}", regs, ea);
                }
            case 5:
                return fmt::format("TST.L {}", disasm_ea(pc, tp, rg, 2));
            case 6:
                if(tp == 0) {
                    return fmt::format("EXT.L %D{}", rg);
                } else {
                    uint16_t rl = read16(pc);
                    auto regs = to_reglists(rl, tp == 4);
                    auto ea = disasm_ea(pc + 2, tp, rg);
                    return fmt::format("MOVEM.L {}, {}", regs, ea);
                }
            case 7:
                return fmt::format("JSR {}", disasm_ea(pc, tp, rg));
            }
            break;
        case 3:
            switch(dn) {
            case 0:
                return fmt::format("MOVE %SR, {}", disasm_ea(pc, tp, rg, 2));
            case 1:
                return fmt::format("MOVE %CCR, {}", disasm_ea(pc, tp, rg, 1));
            case 2:
                return fmt::format("MOVE {}, %CCR", disasm_ea(pc, tp, rg, 1));
            case 3:
                return fmt::format("MOVE {}, %SR", disasm_ea(pc, tp, rg, 2));
            case 4: {
                uint16_t rl = read16(pc);
                auto regs = to_reglists(rl, false);
                auto ea = disasm_ea(pc + 2, tp, rg);
                return fmt::format("MOVEM.W {}, {}", ea, regs);
            }
            case 5:
                return fmt::format("TAS {}", disasm_ea(pc, tp, rg));
            case 6: {
                uint16_t rl = read16(pc);
                auto regs = to_reglists(rl, false);
                auto ea = disasm_ea(pc + 2, tp, rg);
                return fmt::format("MOVEM.L {}, {}", ea, regs);
            }
            case 7:
                return fmt::format("JMP {}", disasm_ea(pc, tp, rg));
            }
            break;
        case 4:
            return fmt::format("CHK.L {}, %D{}", disasm_ea(pc, tp, rg, 4), dn);
        case 5:
            break;
        case 6:
            return fmt::format("CHK.W {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        case 7:
            if(tp == 0 && dn == 4) {
                return fmt::format("EXTB.L %D{}", rg);
            } else {
                return fmt::format("LEA {}, %A{}", disasm_ea(pc, tp, rg), dn);
            }
        }
        break;
    case 005:
        switch(opl) {
        case 0:
            return fmt::format("ADDQ.B #{}, {}", dn ? dn : 8,
                               disasm_ea(pc, tp, rg));
        case 1:
            return fmt::format("ADDQ.W #{}, {}", dn ? dn : 8,
                               disasm_ea(pc, tp, rg));
        case 2:
            return fmt::format("ADDQ.L #{}, {}", dn ? dn : 8,
                               disasm_ea(pc, tp, rg));
        case 4:
            return fmt::format("SUBQ.B #{}, {}", dn ? dn : 8,
                               disasm_ea(pc, tp, rg));
        case 5:
            return fmt::format("SUBQ.W #{}, {}", dn ? dn : 8,
                               disasm_ea(pc, tp, rg));
        case 6:
            return fmt::format("SUBQ.L #{}, {}", dn ? dn : 8,
                               disasm_ea(pc, tp, rg));
        case 3:
        case 7:
            if(tp == 1) {
                return fmt::format("DB{}, %D{}, #{}", cc[op >> 8 & 0x0f], rg,
                                   simm16);
            } else if(tp == 7) {
                if(rg == 2) {
                    return fmt::format("TRAP{} #{:x}", cc[op >> 8 & 0xf],
                                       imm16);
                } else if(rg == 3) {
                    return fmt::format("TRAP{} #{:x}", cc[op >> 8 & 0xf],
                                       imm32);
                } else if(rg == 4) {
                    return fmt::format("TRAP{}", cc[op >> 8 & 0xf]);
                } else {
                    break;
                }
            } else {
                return fmt::format("S{} {}", cc[op >> 8 & 0xf],
                                   disasm_ea(pc, tp, rg));
            }
        }
        break;
    case 006: {
        int cc_v = op >> 8 & 0xf;
        int d = int8_t(op & 0xff);
        if(d == 0) {
            d = int16_t(read16(pc));
        } else if(d == -1) {
            d = read32(pc);
        }
        if(cc_v == 0) {
            return fmt::format("BRA #{:X}", d + pc);
        } else if(cc_v == 1) {
            return fmt::format("BSR #{:X}", d + pc);
        } else {
            return fmt::format("B{} #{:X}", cc[cc_v], d + pc);
        }
    } break;
    case 7:
        if(!(op >> 8 & 1)) {
            int d = int8_t(op & 0xff);
            return fmt::format("MOVEQ #{}, %D{}", d, dn);
        } else {
            return fmt::format("SPECIAL:{:x}", op);
        }
    case 010:
        switch(opl) {
        case 0:
            return fmt::format("OR.B {0}, %D{1}", disasm_ea(pc, tp, rg, 1), dn);
        case 1:
            return fmt::format("OR.W {0}, %D{1}", disasm_ea(pc, tp, rg, 2), dn);
        case 2:
            return fmt::format("OR.L {0}, %D{1}", disasm_ea(pc, tp, rg, 4), dn);
        case 3:
            return fmt::format("DIVU.W {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        case 4:
            if(tp == 0) {
                return fmt::format("SBCD %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("SBCD -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("OR.B %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 5:
            if(tp == 0) {
                return fmt::format("PACK %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("PACK -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("OR.W %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 6:
            if(tp == 0) {
                return fmt::format("UNPK %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("UNPK -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("OR.L %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 7:
            return fmt::format("DIVS.W {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        }
        break;
    case 011:
        switch(opl) {
        case 0:
            return fmt::format("SUB.B {0}, %D{1}", disasm_ea(pc, tp, rg, 1),
                               dn);
        case 1:
            return fmt::format("SUB.W {0}, %D{1}", disasm_ea(pc, tp, rg, 2),
                               dn);
        case 2:
            return fmt::format("SUB.L {0}, %D{1}", disasm_ea(pc, tp, rg, 4),
                               dn);
        case 3:
            return fmt::format("SUBA.W {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        case 4:
            if(tp == 0) {
                return fmt::format("SUBX.B %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("SUBX.B -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("SUB.B %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 5:
            if(tp == 0) {
                return fmt::format("SUBX.W %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("SUBX.W -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("SUB.W %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 6:
            if(tp == 0) {
                return fmt::format("SUBX.L %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("SUBX.L -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("SUB.L %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 7:
            return fmt::format("SUBA.L {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        }
        break;
    case 012:
        return fmt::format("ALINE {:x}", op);
    case 013:
        switch(opl) {

        case 0:
            return fmt::format("CMP.B {0}, %D{1}", disasm_ea(pc, tp, rg, 1),
                               dn);
        case 1:
            return fmt::format("CMP.W {0}, %D{1}", disasm_ea(pc, tp, rg, 2),
                               dn);
        case 2:
            return fmt::format("CMP.L {0}, %D{1}", disasm_ea(pc, tp, rg, 4),
                               dn);
        case 3:
            return fmt::format("CMPA.W {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        case 4:
            if(tp == 1) {
                return fmt::format("CMPM.B (%A{})+, (%A{})+", rg, dn);
            } else {
                return fmt::format("EOR.B %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 5:
            if(tp == 1) {
                return fmt::format("CMPM.W (%A{})+, (%A{})+", rg, dn);
            } else {
                return fmt::format("EOR.W %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 6:
            if(tp == 1) {
                return fmt::format("CMPM.L (%A{})+, (%A{})+", rg, dn);
            } else {
                return fmt::format("EOR.L %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 7:
            return fmt::format("CMPA.L {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        }
        break;
    case 014:
        switch(opl) {
        case 0:
            return fmt::format("AND.B {0}, %D{1}", disasm_ea(pc, tp, rg, 1),
                               dn);
        case 1:
            return fmt::format("AND.W {0}, %D{1}", disasm_ea(pc, tp, rg, 2),
                               dn);
        case 2:
            return fmt::format("AND.L {0}, %D{1}", disasm_ea(pc, tp, rg, 4),
                               dn);
        case 3:
            return fmt::format("MULU.W {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        case 4:
            if(tp == 0) {
                return fmt::format("ABCD.B %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("ABCD.B -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("AND.B %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 5:
            if(tp == 0) {
                return fmt::format("EXG %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("EXG %A{}, %A{}", rg, dn);
            } else {
                return fmt::format("AND.W %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 6:
            if(tp == 0) {
                break;
            } else if(tp == 1) {
                return fmt::format("EXG %D{}, %A{}", rg, dn);
            } else {
                return fmt::format("AND.L %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 7:
            return fmt::format("MULS.W {}, %D{}", disasm_ea(pc, tp, rg, 2), dn);
        }
        break;
    case 015:
        switch(opl) {
        case 0:
            return fmt::format("ADD.B {0}, %D{1}", disasm_ea(pc, tp, rg, 1),
                               dn);
        case 1:
            return fmt::format("ADD.W {0}, %D{1}", disasm_ea(pc, tp, rg, 2),
                               dn);
        case 2:
            return fmt::format("ADD.L {0}, %D{1}", disasm_ea(pc, tp, rg, 4),
                               dn);
        case 3:
            return fmt::format("ADDA.W {}, %A{}", disasm_ea(pc, tp, rg, 2), dn);
        case 4:
            if(tp == 0) {
                return fmt::format("ADDX.B %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("ADDX.B -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("ADD.B %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 5:
            if(tp == 0) {
                return fmt::format("ADDX.W %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("ADDX.W -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("ADD.W %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 6:
            if(tp == 0) {
                return fmt::format("ADDX.L %D{}, %D{}", rg, dn);
            } else if(tp == 1) {
                return fmt::format("ADDX.L -(%A{}), -(%A{})", rg, dn);
            } else {
                return fmt::format("ADD.L %D{1}, {0}", disasm_ea(pc, tp, rg),
                                   dn);
            }
        case 7:
            return fmt::format("ADDA.L {}, %A{}", disasm_ea(pc, tp, rg, 2), dn);
        }
        break;
    case 016:
        switch(opl) {
        case 0:
            if(!(tp & 4)) {
                return fmt::format("{}R.B #{}, %D{}", shifts[tp & 3],
                                   dn ? dn : 8, rg);
            } else {
                return fmt::format("{}R.B %D{}, %D{}", shifts[tp & 3], dn, rg);
            }
        case 1:
            if(!(tp & 4)) {
                return fmt::format("{}R.W #{}, %D{}", shifts[tp & 3],
                                   dn ? dn : 8, rg);
            } else {
                return fmt::format("{}R.W %D{}, %D{}", shifts[tp & 3], dn, rg);
            }
        case 2:
            if(!(tp & 4)) {
                return fmt::format("{}R.L #{}, %D{}", shifts[tp & 3],
                                   dn ? dn : 8, rg);
            } else {
                return fmt::format("{}R.L %D{}, %D{}", shifts[tp & 3], dn, rg);
            }
        case 3:
            if(!(dn & 4)) {
                return fmt::format("{}R.W {}", shifts[dn & 3],
                                   disasm_ea(pc, tp, rg));
            } else {
                std::string offset = nextop >> 11 & 1
                                         ? fmt::format("%D{}", nextop >> 6 & 7)
                                         : fmt::format("{}", nextop >> 6 & 31);
                std::string width =
                    nextop >> 5 & 1
                        ? fmt::format("%D{}", nextop & 7)
                        : fmt::format("{}", (nextop & 31) ? (nextop & 31) : 32);
                return fmt::format("BF{} {}{{{}:{}}}", bit_tst[dn & 3],
                                   disasm_ea(pc, tp, rg), offset, width);
            }

        case 4:
            if(!(tp & 4)) {
                return fmt::format("{}L.B #{}, %D{}", shifts[tp & 3],
                                   dn ? dn : 8, rg);
            } else {
                return fmt::format("{}L.B %D{}, %D{}", shifts[tp & 3], dn, rg);
            }
        case 5:
            if(!(tp & 4)) {
                return fmt::format("{}L.W #{}, %D{}", shifts[tp & 3],
                                   dn ? dn : 8, rg);
            } else {
                return fmt::format("{}L.W %D{}, %D{}", shifts[tp & 3], dn, rg);
            }
        case 6:
            if(!(tp & 4)) {
                return fmt::format("{}L.L #{}, %D{}", shifts[tp & 3],
                                   dn ? dn : 8, rg);
            } else {
                return fmt::format("{}L.L %D{}, %D{}", shifts[tp & 3], dn, rg);
            }
        case 7:
            if(!(dn & 4)) {
                return fmt::format("{}L.L {}", shifts[dn & 3],
                                   disasm_ea(pc, tp, rg));
            } else {
                std::string offset = nextop >> 11 & 1
                                         ? fmt::format("%D{}", nextop >> 6 & 7)
                                         : fmt::format("{}", nextop >> 6 & 31);
                std::string width =
                    nextop >> 5 & 1
                        ? fmt::format("%D{}", nextop & 7)
                        : fmt::format("{}", (nextop & 31) ? (nextop & 31) : 32);
                switch(dn) {
                case 4:
                    return fmt::format("BFEXTU {0} {{{1}:{2}}}, %D{3}",
                                       disasm_ea(pc, tp, rg), offset, width,
                                       dn);
                case 5:
                    return fmt::format("BFEXTS {0} {{{1}:{2}}}, %D{3}",
                                       disasm_ea(pc, tp, rg), offset, width,
                                       dn);
                case 6:
                    return fmt::format("BFFFO {0} {{{1}:{2}}}, %D{3}",
                                       disasm_ea(pc, tp, rg), offset, width,
                                       dn);
                case 7:
                    return fmt::format("BFINS %D{3}, {0} {{{1}:{2}}}",
                                       disasm_ea(pc, tp, rg), offset, width,
                                       dn);
                }
            }
        }
        break;
    case 017:
        if(dn == 0) {
            break;
        } else if(dn == 1) {
            // FPU
            switch(opl) {
            case 0:
                if(!(nextop >> 15 & 1)) {
                    if(!(nextop >> 13 & 1)) {
                        std::string src;
                        bool rm = nextop >> 14 & 1;
                        int fpm_size = nextop >> 10 & 7;
                        int fpn = nextop >> 7 & 7;
                        char sz = 'X';
                        if(rm) {
                            if(fpm_size < 7) {
                                src =
                                    disasm_ea(pc + 2, tp, rg, fp_sz[fpm_size]);
                                sz = fp_szc[fpm_size];
                            } else {
                                return fmt::format("FMOVECR.X #{:x}, %FP{}",
                                                   nextop & 0x7f, fpn);
                            }
                        } else {
                            src = fmt::format("%FP{}", fpm_size);
                            sz = 'X';
                        }
                        if((nextop & 0x78) == 0x30) {
                            return fmt::format("FSINCOS.{} {}, %FP{}, %FP{}",
                                               sz, src, nextop & 7, fpn);
                        } else if((nextop & 0x7f) == 0x3A) {
                            return fmt::format("FTST.{} {}", sz, src);
                        } else {
                            return fmt::format("{}.{} {}, %FP{}",
                                               fpu_op[nextop & 0x7f], sz, src,
                                               fpn);
                        }
                    } else {
                        int fpm_size = nextop >> 10 & 7;
                        int fpn = nextop >> 7 & 7;
                        std::string ea = disasm_ea(pc + 2, tp, rg);
                        if(fpm_size == 3) {
                            int8_t k = (op & 0x7f) << 1;
                            return fmt::format("FMOVE.P %FP{}, ({}) {{{}}}",
                                               nextop & 0x7f, fpn, int(k >> 1));
                        } else if(fpm_size == 7) {
                            return fmt::format("FMOVE.P %FP{}, ({}) {{%D{}}}",
                                               nextop & 0x7f, fpn, op >> 4 & 7);

                        } else {
                            return fmt::format("FMOVE.{} %FP{}, ({})",
                                               fp_szc[fpm_size], fpn, ea);
                        }
                    }
                } else {
                    if(!(nextop >> 14) & 1) {

                        std::string ea = disasm_ea(pc + 2, tp, rg);
                        std::vector<std::string> crs;
                        if(nextop >> 12 & 1) {
                            crs.push_back("%FPCR");
                        }
                        if(nextop >> 11 & 1) {
                            crs.push_back("%FPSR");
                        }
                        if(nextop >> 10 & 1) {
                            crs.push_back("%FPIAR");
                        }
                        if(!(nextop >> 13 & 1)) {
                            if(crs.empty()) {
                                break;
                            } else if(crs.size() == 1) {
                                return fmt::format("FMOVE {0}, {1}", ea,
                                                   crs[0]);
                            } else {
                                return fmt::format("FMOVEM {0}, {1}", ea,
                                                   fmt::join(crs, ","));
                            }
                        } else {
                            if(crs.empty()) {
                                break;
                            } else if(crs.size() == 1) {
                                return fmt::format("FMOVE {1}, {0}", ea,
                                                   crs[0]);
                            } else {
                                return fmt::format("FMOVEM {1}, {0}", ea,
                                                   fmt::join(crs, ","));
                            }
                        }
                    } else {
                        auto ea = disasm_ea(pc + 2, tp, rg);
                        switch(nextop >> 11 & 7) {
                        case 2:
                            return fmt::format(
                                "FMOVEM.X {0}, {1}", ea,
                                to_fpreglists(nextop & 0xff, false));
                        case 3:
                            return fmt::format("FMOVEM.X {0}, %D{1}", ea,
                                               nextop >> 4 & 7);
                        case 4:
                            return fmt::format(
                                "FMOVEM.X {0}, -(%A{1})",
                                to_fpreglists(nextop & 0xff, true), rg);
                        case 5:
                            return fmt::format("FMOVEM.X %D{0}, -(%A{1})",
                                               nextop >> 4 & 7, rg);
                        case 6:
                            return fmt::format(
                                "FMOVEM.X {1}, {0}", ea,
                                to_fpreglists(nextop & 0xff, false));
                        case 7:
                            return fmt::format("FMOVEM.X %D{1}, {0}", ea,
                                               nextop >> 4 & 7);
                        }
                    }
                }
                break;
            case 1:
                if(tp == 7 && rg == 2) {
                    int16_t disp = read16(pc + 2);
                    return fmt::format("FTRAP{} #{}", fcc[nextop & 0x1f], disp);
                } else if(tp == 7 && rg == 3) {
                    int32_t disp = read32(pc + 2);
                    return fmt::format("FTRAP{} #{}", fcc[nextop & 0x1f], disp);
                } else if(tp == 7 && rg == 4) {
                    return fmt::format("FTRAP{}", fcc[nextop & 0x1f]);
                } else if(tp == 1) {
                    int16_t disp = read16(pc + 2);
                    return fmt::format("FDB{} %D{}, #{}", fcc[nextop & 0x1f],
                                       rg, disp);
                } else {
                    return fmt::format("FS{} {}", fcc[nextop & 0x1f],
                                       disasm_ea(pc + 2, tp, rg));
                }
            case 2: {
                uint16_t ccv = op & 0x1f;
                if(ccv == 0) {
                    return "FNOP";
                } else {
                    return fmt::format("FB{}.W {}", fcc[ccv],
                                       static_cast<int16_t>(nextop));
                }
            }
            case 3: {
                uint16_t ccv = op & 0x1f;
                int32_t disp = read32(pc);
                return fmt::format("FB{}.W {}", fcc[ccv], disp);
            }
            case 4:
                return fmt::format("FSAVE {}", disasm_ea(pc, tp, rg));
            case 5:
                return fmt::format("FRESTORE {}", disasm_ea(pc, tp, rg));
            }
        } else if(dn == 2) {
            // MMU
            static const char *cache_type[] = {"", "DC", "IC", "BC"};
            if(opl > 1 && opl < 4) {
                switch(tp) {
                case 1:
                    return fmt::format("CINVL {}, (%A{})", cache_type[opl], rg);
                case 2:
                    return fmt::format("CINVP {}, (%A{})", cache_type[opl], rg);
                case 3:
                    return fmt::format("CINVA {}", cache_type[opl]);
                case 5:
                    return fmt::format("CPUSHL {}, (%A{})", cache_type[opl],
                                       rg);
                case 6:
                    return fmt::format("CPUSHP DC, (%A{})", cache_type[opl],
                                       rg);
                case 7:
                    return fmt::format("CPUSHA {}", cache_type[opl]);
                }
            } else if(opl == 4) {
                switch(tp) {
                case 0:
                    return fmt::format("PFLUSHN (%A{})", rg);
                case 1:
                    return fmt::format("PFLUSH (%A{})", rg);
                case 2:
                    return "PFLUSHAN";
                case 3:
                    return "PFLUSHA";
                }
            } else if(opl == 5) {
                switch(tp) {
                case 1:
                    return fmt::format("PTESTR (%A{})", rg);
                case 5:
                    return fmt::format("PTESTW (%A{})", rg);
                }
            }

        } else if(dn == 3) {
            int32_t imm = read32(pc + 2);
            // MOVE16
            switch(tp) {
            case 0:
                return fmt::format("MOVE16 (%A{0})+, (0x{1:X})", rg, imm);
            case 1:
                return fmt::format("MOVE16 (0x{1:X}), (%A{0})+", rg, imm);
            case 2:
                return fmt::format("MOVE16 (%A{0}), (0x{1:X})", rg, imm);
            case 3:
                return fmt::format("MOVE16 (0x{1:X}), (%A{0})", rg, imm);
            case 4:
                return fmt::format("MOVE16 (%A{})+, (%A{})+", rg,
                                   nextop >> 12 & 7);
            }
        }
        break;
    }

    return "ILLEGAL";
}
extern std::unordered_map<uint32_t, void (*)()> rom_functions;

void dump_regs() {
    if(rom_functions.count(regs.pc & 0xfffff)) {
        printf("%08x: ROM\n", regs.pc);
        fflush(stdout);
        return;
    }
    auto op = disasm(regs.pc);
    printf("%08x: %s\t ", regs.pc, op.c_str());
    for(int i = 0; i < 8; ++i) {
        printf("D%d=%X ", i, regs.d[i]);
    }
    for(int i = 0; i < 8; ++i) {
        printf("A%d=%X ", i, regs.a[i]);
    }
    if(regs.S) {
        printf("S");
    }
    if(regs.M) {
        printf("M");
    }
    if(regs.x) {
        printf("X");
    }
    if(regs.n) {
        printf("N");
    }
    if(regs.z) {
        printf("Z");
    }
    if(regs.v) {
        printf("V");
    }
    if(regs.c) {
        printf("C");
    }
    printf("\n");
    fflush(stdout);
}
