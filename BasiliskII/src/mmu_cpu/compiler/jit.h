#ifndef COMPILER_JIT_
#define COMPILER_JIT_
#include <asmjit/x86.h>
#include <stdint.h>
#include <unordered_map>
using jit_t = void (*)(uint16_t, int type, int reg, asmjit::x86::Assembler &a);
using op_t = void (*)(uint16_t, int, int, int);
#ifdef _WIN64
constexpr auto ARG1 = asmjit::x86::rcx;
constexpr auto ARG2 = asmjit::x86::rdx;
constexpr auto ARG3 = asmjit::x86::r8;
constexpr auto ARG4 = asmjit::x86::r9;
constexpr auto eARG1 = asmjit::x86::ecx;
constexpr auto eARG2 = asmjit::x86::edx;
constexpr auto eARG3 = asmjit::x86::r8l;
constexpr auto eARG4 = asmjit::x86::r9l;
#else
constexpr auto ARG1 = asmjit::x86::rdi;
constexpr auto ARG2 = asmjit::x86::rsi;
constexpr auto ARG3 = asmjit::x86::rdx;
constexpr auto ARG4 = asmjit::x86::rcx;
constexpr auto eARG1 = asmjit::x86::edi;
constexpr auto eARG2 = asmjit::x86::esi;
constexpr auto eARG3 = asmjit::x86::edx;
constexpr auto eARG4 = asmjit::x86::ecx;
#endif

void jit_ea_addr(int type, int reg, int sz, bool w, asmjit::x86::Assembler &a);
void jit_ea_read8(int type, int reg, bool override, asmjit::x86::Assembler &a);
void jit_ea_read16(int type, int reg, bool override, asmjit::x86::Assembler &a);
void jit_ea_read32(int type, int reg, bool override, asmjit::x86::Assembler &a);
void jit_ea_write8(int type, int reg, asmjit::x86::Assembler &a) ;
void jit_ea_write16(int type, int reg, asmjit::x86::Assembler &a) ;
void jit_ea_write32(int type, int reg, asmjit::x86::Assembler &a) ;

void jit_ea_update8(int type, int reg, asmjit::x86::Assembler &a) ;
void jit_ea_update16(int type, int reg, asmjit::x86::Assembler &a) ;
void jit_ea_update32(int type, int reg, asmjit::x86::Assembler &a) ;

uint32_t jit_read8(uint32_t addr) ;
uint32_t jit_read16(uint32_t addr) ;
uint32_t jit_read32(uint32_t addr) ;

void jit_write8(uint32_t addr, uint32_t v);
void jit_write16(uint32_t addr, uint32_t v);
void jit_write32(uint32_t addr, uint32_t v);

void exit_jit () __attribute__((noreturn));
#define REG_BYTE(n) (x86::byte_ptr(x86::r15, offsetof(m68k_reg, n)))
#define REG_WORD(n) (x86::word_ptr(x86::r15, offsetof(m68k_reg, n)))
#define REG_LWORD(n) (x86::dword_ptr(x86::r15, offsetof(m68k_reg, n)))

#define JIT_REG_D_B(n) (x86::byte_ptr(x86::r15, (n)*4))
#define JIT_REG_D_W(n) (x86::word_ptr(x86::r15, (n)*4))
#define JIT_REG_D_L(n) (x86::dword_ptr(x86::r15, (n)*4))
#define JIT_REG_A_W(n) (x86::word_ptr(x86::r15, 8*4+(n)*4))
#define JIT_REG_A_L(n) (x86::dword_ptr(x86::r15, 8*4+(n)*4))

struct JIT_CODE {
    asmjit::CodeHolder holder;
    std::unordered_map<uint32_t, asmjit::Label> jit_labels;
    std::unordered_map<uint32_t, uint64_t> init_entry;
    void (*func)(void);
};
#endif