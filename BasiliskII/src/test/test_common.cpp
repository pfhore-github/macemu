#define BOOST_TEST_MODULE mmu_cpu
#define BOOST_TEST_DYN_LINK
#include "SDL.h"
#include "sysdeps.h"

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

#include "cpu_emulation.h"
#include "main.h"
#include "newcpu.h"
#include "test_common.h"
#include <dirent.h>
#include <fmt/core.h>
#include <random>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

std::vector<std::byte> RAM;
uint8_t *ROMBaseHost;
std::unique_ptr<std::mt19937> rnd;
void init_m68k();
extern bool rom_overlay;
bool reset = false;
void reset_all() { reset = true; }
static std::string paths[] = {"../test/arithmetic", "../test/bcd",
                              "../test/bits",       "../test/branch",
                              "../test/fpu",        "../test/move"};
struct MyGlobalFixture {
    MyGlobalFixture() {
        if(SDL_Init(SDL_INIT_EVERYTHING)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "fatal error",
                                     SDL_GetError(), nullptr);
            exit(1);
        }
        init_m68k();
        RAM.resize(0x100000);
        srand(time(nullptr));
        rom_overlay = false;
        for(const auto &path : paths) {
            DIR *p = opendir(path.c_str());
            struct dirent *d;
            while((d = readdir(p)) != nullptr) {
                std::string name = d->d_name;
                if(name.ends_with(".yaml")) {
                    std::string full = path + "/" + name;
                    load_test(full.c_str());
                }
            }
            closedir(p);
        }
    }

    ~MyGlobalFixture() {}
};
void init_mmu();
std::unordered_map<std::string, YAML::Node> tests;
void load_test(const char *file) {

    for(auto i : YAML::LoadFile(file)) {
        std::string name = i["name"].as<std::string>();
        tests[name] = static_cast<YAML::Node>(i);
    }
}
uint32_t do_op_movec_from(int o);
void do_op_movec_to(int op, uint32_t v);
bool cpu_test(const YAML::Node &test, bool jit) {
    InitFix f;
    std::string name = test["name"].as<std::string>();
    BOOST_TEST_CONTEXT(name) {
        // initialize
        uint32_t start_address = (rand() & 0xff) * 2;
        if(test["expected"]["exception"]) {
            regs.M = false;
            regs.vbr = 0x3000;
            regs.a[7] = regs.isp = regs.msp = 0x2000;
            raw_write32(0x3000 + test["expected"]["exception"].as<int>() * 4,
                        0x5000);
        }

        for(const auto &i : test["init"]) {
            const std::string &key = i.first.as<std::string>();
            const auto &value = i.second;
            regs.msp = regs.isp = 0x1000;
            if(key.starts_with("D") && isdigit(key[1])) {
                regs.d[(key[1] - '0')] = value.as<long>();
            } else if(key.starts_with("A") && isdigit(key[1])) {
                regs.a[(key[1] - '0')] = value.as<long>();
            } else if(key.starts_with("FP") && isdigit(key[2])) {
                auto v_s = value.as<std::string>();
                fpu.fp[(key[2] - '0')] = v_s.c_str();
            } else if(key == "MEM") {
                for(const auto &j : value) {
                    const std::string &addr = j.first.as<std::string>();
                    const auto &mem_v = j.second;
                    char *suffix;
                    uint32_t addr_v = strtoul(addr.c_str(), &suffix, 0);
                    uint32_t value_v = mem_v.as<long>();
                    if(strncmp(suffix, "/B", 2) == 0) {
                        raw_write8(addr_v, value_v);
                    } else if(strncmp(suffix, "/W", 2) == 0) {
                        raw_write16(addr_v, value_v);
                    } else if(strncmp(suffix, "/L", 2) == 0) {
                        raw_write32(addr_v, value_v);
                    }
                }
            } else if(key == "X") {
                regs.x = value.as<bool>();
            } else if(key == "V") {
                regs.v = value.as<bool>();
            } else if(key == "C") {
                regs.c = value.as<bool>();
            } else if(key == "N") {
                regs.n = value.as<bool>();
            } else if(key == "Z") {
                regs.z = value.as<bool>();
            } else if(key == "S") {
                regs.S = value.as<bool>();
            } else if(key == "IM") {
                regs.IM = value.as<int>();
            } else if(key == "PC") {
                start_address = value.as<int>();
            } else if(key == "USP") {
                regs.usp = value.as<int>();
            } else if(key == "MSP") {
                regs.msp = value.as<int>();
            } else if(key == "ISP") {
                regs.isp = value.as<int>();
            } else if(key == "SFC") {
                regs.sfc = value.as<int>();
            } else if(key == "DFC") {
                regs.dfc = value.as<int>();
            } else if(key == "VBR") {
                regs.vbr = value.as<uint32_t>();
            } else if(key == "CACR") {
                do_op_movec_to(2, value.as<uint32_t>());
            } else if(key == "TCR") {
                do_op_movec_to(3, value.as<uint32_t>());
            } else if(key == "ITTR0") {
                do_op_movec_to(4, value.as<uint32_t>());
            } else if(key == "ITTR1") {
                do_op_movec_to(5, value.as<uint32_t>());
            } else if(key == "DTTR0") {
                do_op_movec_to(6, value.as<uint32_t>());
            } else if(key == "DTTR1") {
                do_op_movec_to(7, value.as<uint32_t>());
            } else if(key == "MMUSR") {
                do_op_movec_to(0x805, value.as<uint32_t>());
            } else if(key == "URP") {
                do_op_movec_to(0x806, value.as<uint32_t>());
            } else if(key == "SRP") {
                do_op_movec_to(0x807, value.as<uint32_t>());
            }
        }
        uint32_t pc = start_address;
        for(const auto &value : test["opcodes"]) {

            long v = value.as<long>();
            if(v < std::numeric_limits<int16_t>::min() ||
               v > std::numeric_limits<uint16_t>::max()) {
                raw_write32(pc, v);
                pc += 4;
            } else {
                raw_write16(pc, v);
                pc += 2;
            }
        }
        regs.pc = start_address;
        regs.exception = false;
        if(!jit) {
            while( regs.pc < pc) {
                m68k_do_execute();
            }
        } else {
            jit_compile(start_address, pc);
            jit_jump(start_address);
        }

        // TEST
        for(const auto &i : test["expected"]) {
            auto key = i.first.as<std::string>();
            auto value = i.second;
            if(key.starts_with("D") && isdigit(key[1])) {
                uint32_t expected = value.as<long>();
                BOOST_TEST(regs.d[(key[1] - '0')] == expected);
            } else if(key.starts_with("A") && isdigit(key[1])) {
                uint32_t expected = value.as<long>();
                BOOST_TEST(regs.a[(key[1] - '0')] == expected);
            } else if(key.starts_with("FP") && isdigit(key[2])) {
                // TODO
            } else if(key == "MEM") {
                for(const auto &j : value) {
                    const std::string &addr = j.first.as<std::string>();
                    const auto &mem_v = j.second;
                    char *suffix;
                    uint32_t addr_v = strtoul(addr.c_str(), &suffix, 0);
                    uint32_t value_v = mem_v.as<long>();
                    if(strncmp(suffix, "/B", 2) == 0) {
                        BOOST_TEST(raw_read8(addr_v) == value_v);
                    } else if(strncmp(suffix, "/W", 2) == 0) {
                        BOOST_TEST(raw_read16(addr_v) == value_v);
                    } else if(strncmp(suffix, "/L", 2) == 0) {
                        BOOST_TEST(raw_read32(addr_v) == value_v);
                    }
                }
            } else if(key == "X") {
                BOOST_TEST(regs.x == value.as<bool>());
            } else if(key == "V") {
                BOOST_TEST(regs.v == value.as<bool>());
            } else if(key == "C") {
                BOOST_TEST(regs.c == value.as<bool>());
            } else if(key == "N") {
                BOOST_TEST(regs.n == value.as<bool>());
            } else if(key == "Z") {
                BOOST_TEST(regs.z == value.as<bool>());
            } else if(key == "S") {
                BOOST_TEST(regs.S == value.as<bool>());
            } else if(key == "PC") {
                BOOST_TEST(regs.pc == value.as<int>());
            } else if(key == "USP") {
                BOOST_TEST(regs.usp == value.as<int>());
            } else if(key == "MSP") {
                BOOST_TEST(regs.msp == value.as<int>());
            } else if(key == "ISP") {
                BOOST_TEST(regs.isp == value.as<int>());
            } else if(key == "IM") {
                BOOST_TEST(regs.IM == value.as<int>());
            } else if(key == "SFC") {
                BOOST_TEST(regs.sfc == value.as<int>());
            } else if(key == "DFC") {
                BOOST_TEST(regs.dfc == value.as<int>());
            } else if(key == "VBR") {
                BOOST_TEST(regs.vbr == value.as<int>());
            } else if(key == "CACR") {
                BOOST_TEST(do_op_movec_from(2) == value.as<uint32_t>(),
                           boost::test_tools::bitwise());
            } else if(key == "TCR") {
                BOOST_TEST(do_op_movec_from(3) == value.as<uint32_t>(),
                           boost::test_tools::bitwise());
            } else if(key == "ITTR0") {
                BOOST_TEST(do_op_movec_from(4) == value.as<uint32_t>(),
                           boost::test_tools::bitwise());
            } else if(key == "ITTR1") {
                BOOST_TEST(do_op_movec_from(5) == value.as<uint32_t>(),
                           boost::test_tools::bitwise());
            } else if(key == "DTTR0") {
                BOOST_TEST(do_op_movec_from(6) == value.as<uint32_t>(),
                           boost::test_tools::bitwise());
            } else if(key == "DTTR1") {
                BOOST_TEST(do_op_movec_from(7) == value.as<uint32_t>(),
                           boost::test_tools::bitwise());
            } else if(key == "MMUSR") {
                BOOST_TEST(do_op_movec_from(0x805) == value.as<uint32_t>(),
                           boost::test_tools::bitwise());
            } else if(key == "URP") {
                BOOST_TEST(do_op_movec_from(0x806) == value.as<uint32_t>());
            } else if(key == "SRP") {
                BOOST_TEST(do_op_movec_from(0x807) == value.as<uint32_t>());
            } else if(key == "exception") {
                int e = value.as<int>();
                if(e) {
                    BOOST_TEST(regs.S, "exception doesn't occur");
                    BOOST_TEST(regs.pc == 0x5000, "invalid exception");
                    BOOST_TEST((raw_read16(regs.a[7] + 6) & 0xfff) == e * 4);
                } else {
                    BOOST_TEST(regs.pc != 0x5000, "unexpected exception");
                }
            }
        }
    }
    return true;
}
InitFix::InitFix() {
    RAM.clear();
    RAM.resize(0x6000);
    memset(regs.r, 0, sizeof(uint32_t) * 16);
    regs.exception = false;
    regs.pc = 0;
    regs.v = regs.c = regs.n = regs.x = regs.z = false;
    regs.S = false;
    regs.M = false;
    regs.IM = 0;
    regs.irq = 0;
    memset(&fpu.FPCR, 0, sizeof(fpu.FPCR));
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    mpfr_clear_flags();
    init_mmu();
}
BOOST_TEST_GLOBAL_FIXTURE(MyGlobalFixture);

bool done = false;
size_t ROMSize = 0x3fff;
void Exit680x0() { done = true; }
uint8_t raw_read8(uint32_t addr) { return (uint8_t)RAM[addr]; }
uint16_t raw_read16(uint32_t addr) {
    auto p = reinterpret_cast<const uint16_t *>(&RAM[addr]);
    return SDL_SwapBE16(*p);
}

uint32_t raw_read32(uint32_t addr) {
    auto p = reinterpret_cast<const uint32_t *>(&RAM[addr]);
    return SDL_SwapBE32(*p);
}
void raw_write8(uint32_t addr, uint8_t v) { RAM[addr] = (std::byte)v; }
void raw_write16(uint32_t addr, uint16_t v) {
    auto p = reinterpret_cast<uint16_t *>(&RAM[addr]);
    *p = SDL_SwapBE16(v);
}
void raw_write32(uint32_t addr, uint32_t v) {
    auto p = reinterpret_cast<uint32_t *>(&RAM[addr]);
    *p = SDL_SwapBE32(v);
}

static std::unordered_map<std::string, std::string> asmcodes;

void set_fpu_reg(int reg, const xval &v) {
    fpu.fp[reg].set_exp(v.sg, v.frac, v.exp + 64);
}
void set_fpu_reg(int reg, double v) { fpu.fp[reg] = v; }

void set_fpu_reg(int reg, float v) { fpu.fp[reg] = v; }

std::unordered_map<uint32_t, void (*)()> rom_functions;
void dump_regs() {}
void EmulOp(uint16_t opcode, M68kRegisters *r) {}
bool jit_jump(uint32_t to);
void jit_exception_check(int e) {
    regs.M = false;
    regs.usp = regs.isp = regs.a[7] = 0x1000;
    regs.vbr = 0x3000;
    raw_write32(0x3000 + e * 4, 0x5000);
    jit_jump(0);
    if(e) {
        BOOST_TEST(regs.S);
        BOOST_TEST(regs.pc == 0x5000);
        BOOST_TEST((raw_read16(regs.a[7] + 6) & 0xfff) == e * 4);
    } else {
        BOOST_TEST(regs.pc != 0x5000);
    }
}

void exception_check(int e, int tp) {
    regs.M = false;
    regs.usp = regs.isp = regs.a[7] = 0x1000;
    regs.vbr = 0x3000;
    raw_write32(0x3000 + e * 4, 0x5000);
    m68k_do_execute();
    if(e) {
        BOOST_TEST(regs.S);
        BOOST_TEST(regs.pc == 0x5000);
        BOOST_TEST((raw_read16(regs.a[7] + 6) & 0xfff) == e * 4);
        if(tp != -1) {
            BOOST_TEST((raw_read16(regs.a[7] + 6) >> 12) == tp);
        }
    } else {
        BOOST_TEST(regs.pc != 0x5000);
    }
}

uint8_t readIO8(uint32_t addr) { return 0; }
uint16_t readIO16(uint32_t addr) { return 0; }
uint32_t readIO32(uint32_t addr) { return 0; }
void writeIO8(uint32_t addr, uint8_t v) {}
void writeIO16(uint32_t addr, uint16_t v) {}
void writeIO32(uint32_t addr, uint32_t v) {}
