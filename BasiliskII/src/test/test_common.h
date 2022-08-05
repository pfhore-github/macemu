#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include "fpu/fpu.h"
#include <vector>
#include <optional>
#include "yaml-cpp/yaml.h"
namespace bdata = boost::unit_test::data;

void load_test(const char *file) ;
bool cpu_test(const YAML::Node& test);

void m68k_do_execute();
extern std::vector<std::byte> RAM;

uint8_t raw_read8(uint32_t addr);
uint16_t raw_read16(uint32_t addr);
uint32_t raw_read32(uint32_t addr);
void raw_write8(uint32_t addr, uint8_t v);
void raw_write16(uint32_t addr, uint16_t v);
void raw_write32(uint32_t addr, uint32_t v);

const int SIGN[2] = {-1, 1};
const bool BIT[2] = {false, true};
inline auto REG() { return bdata::xrange(0, 8); }
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];
struct InitFix {
    InitFix();
    ~InitFix() { regs.exception = false; }
};

void exception_check(int e, int tp = -1);
struct xval {
    bool sg;
    uint64_t frac;
    int exp;
};
void set_fpu_reg(int reg, const xval &v);
void set_fpu_reg(int reg, double v);
void set_fpu_reg(int reg, float v);
extern M68881 fpu;
#ifndef NAN
constexpr double NAN = std::numeric_limits<double>::quiet_NaN();
#endif
#ifndef INFINITY
constexpr double INFINITY = std::numeric_limits<double>::infinity();
#endif
template<class T>
void fpu_check(int d, T expected) {
     switch(fpclassify(expected)) {
    case FP_NAN:
        BOOST_TEST(fpu.fp[d].is_nan());
        break;
    case FP_INFINITE:
        BOOST_TEST(fpu.fp[d].is_inf());
        BOOST_TEST(fpu.fp[d].signbit() == signbit(expected));
        break;
    case FP_ZERO:
        BOOST_TEST(fpu.fp[d].is_zero());
        BOOST_TEST(fpu.fp[d].signbit()== signbit(expected));
        break;
    default:
        if(sizeof(T) == sizeof(float)) {
            BOOST_CHECK_CLOSE(
                static_cast<float>(fpu.fp[d]), expected, 1e-04);
        } else if(sizeof(T) == sizeof(double)) {
            BOOST_CHECK_CLOSE( static_cast<double>(fpu.fp[d]),
                                       expected, 1e-10);
        }
        break;
    }
}
template <class T, class T2 = T>
void fpu_test2(char op, T v1, T2 expected) {
    set_fpu_reg(1, v1);
    set_fpu_reg(2, NAN);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | op);
    m68k_do_execute();
    fpu_check(2, expected);
}

template <class T, class T2 = T>
void fpu_test(char op, T v1, T2 expected) {
    set_fpu_reg(1, v1);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 1 << 7 | op);
    m68k_do_execute();
    fpu_check(1, expected);
}
template <class T, class T2 = T>
void fpu_test(char op, T v1, T v2, T2 expected) {
    set_fpu_reg(1, v1);
    set_fpu_reg(2, v2);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | op);
    m68k_do_execute();
    fpu_check(2, expected);
}
bool op_compile(uint32_t from, uint32_t to);
bool jit_jump(uint32_t to);
void jit_exception_check(int e);

