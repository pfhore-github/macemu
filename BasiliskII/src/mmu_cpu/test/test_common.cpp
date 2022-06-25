#define BOOST_TEST_MODULE mmu_cpu
#define BOOST_TEST_DYN_LINK
#include "SDL.h"
#include "sysdeps.h"
#include <boost/test/unit_test.hpp>
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
#include <fmt/core.h>
#include <random>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
std::vector<std::byte> RAM;
uint8_t *ROMBaseHost;
std::unique_ptr<std::mt19937> rnd;
void init_m68k();
bool reset = false;
void reset_all() { reset = true; }
struct MyGlobalFixture {
    MyGlobalFixture() {
        init_m68k();
        RAM.resize(0x100000);
        srand(time(nullptr));
        std::random_device seed_gen;
        rnd = std::make_unique<std::mt19937>(seed_gen());
    }

    ~MyGlobalFixture() {}
};
void init_mmu();

InitFix::InitFix() {
    RAM.clear();
    RAM.resize(0x100000);
    memset(regs.r, 0, sizeof(uint32_t) * 16);
    regs.pc = 0;
    regs.v = regs.c = regs.n = regs.x = regs.z = false;
    regs.S = false;
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
void set_fpu_reg(int reg, double v) {
    fpu.fp[reg] = v;
}

void set_fpu_reg(int reg, float v) { fpu.fp[reg] = v; }

std::unordered_map<uint32_t, void (*)()> rom_functions;
void dump_regs() {}
void EmulOp(uint16_t opcode, M68kRegisters *r) {}

void exception_check(int e) {
    regs.M = false;
    regs.usp = regs.isp = regs.a[7] = 0x1000;
    regs.vbr = 0x3000;
    raw_write32(0x3000 + e * 4, 0x5000);
    m68k_do_execute();
    if(e) {
        BOOST_TEST(regs.S);
        BOOST_TEST(regs.pc == 0x5000);
        BOOST_TEST((raw_read16(regs.a[7] + 6) & 0xfff) == e * 4);
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

int rand_reg() {
    std::uniform_int_distribution<> dist(0, 7);
    return dist(*rnd);
}

int rand_ar() {
    std::uniform_int_distribution<> dist(0, 6);
    return dist(*rnd);
}
std::pair<int, int> rand_reg2() {
    std::uniform_int_distribution<> dist(0, 7);
    int a, b;
    do {
        a = dist(*rnd);
        b = dist(*rnd);
    } while(a == b);
    return {a, b};
}
std::tuple<int, int, int> rand_reg3() {
    std::uniform_int_distribution<> dist(0, 7);
    int a, b, c;
    do {
        a = dist(*rnd);
        b = dist(*rnd);
        c = dist(*rnd);
    } while(a == b || a == c || b == c);
    return {a, b, c};
}

std::tuple<int, int, int, int> rand_reg4() {
    std::uniform_int_distribution<> dist(0, 7);
    int a, b, c, d;
    do {
        a = dist(*rnd);
        b = dist(*rnd);
        c = dist(*rnd);
        d = dist(*rnd);
    } while(a == b || a == c || a == d || b == c || b == d || c == d);
    return {a, b, c, d};
}

uint8_t get_v8() {
    std::uniform_int_distribution<uint8_t> dist;
    return dist(*rnd);
}
uint16_t get_v16() {
    std::uniform_int_distribution<uint16_t> dist;
    return dist(*rnd);
}
uint32_t get_v32() {
    std::uniform_int_distribution<uint32_t> dist;
    return dist(*rnd);
}

uint64_t get_vn(int mn, int mx) {
    std::uniform_int_distribution<uint64_t> dist(mn, mx);
    return dist(*rnd);
}

double get_rx(double mn, double mx) {
    std::uniform_real_distribution<double> dist(mn, mx);
    return dist(*rnd);
}