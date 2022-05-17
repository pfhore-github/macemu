#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;
void asm_m68k(const char *a, int offset = 0);
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
int rand_reg();
int rand_ar();
std::pair<int, int> rand_reg2();
std::tuple<int, int, int> rand_reg3();
std::tuple<int, int, int, int> rand_reg4();
uint8_t get_v8();
uint16_t get_v16();
uint32_t get_v32();
uint64_t get_vn(int mn, int mx);

double get_rx(double mn, double mx);
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];
struct InitFix {
    InitFix();
    ~InitFix() { regs.exception = false; }
};

void exception_check(int e);
struct xval {
    long frac;
    int exp;
};
void set_fpu_reg(int reg, const xval &v);
void set_fpu_reg(int reg, double v);
void set_fpu_reg(int reg, float v);

template <class T, class T2 = T>
void fpu_test(char op, T v1, T v2, T2 expected) {
    auto [src, dst] = rand_reg2();
    set_fpu_reg(src, v1);
    set_fpu_reg(dst, v2);
    raw_write16(0, 0171000);
    raw_write16(2, src << 10 | dst << 7 | op);
    m68k_do_execute();
    switch(fpclassify(expected)) {
    case FP_NAN:
        BOOST_TEST(mpfr_nan_p(regs.fpu.fp[dst]));
        break;
    case FP_INFINITE:
        BOOST_TEST(mpfr_inf_p(regs.fpu.fp[dst]));
        BOOST_TEST(mpfr_signbit(regs.fpu.fp[dst]) == signbit(expected));
        break;
    case FP_ZERO:
        BOOST_TEST(mpfr_zero_p(regs.fpu.fp[dst]));
        BOOST_TEST(mpfr_signbit(regs.fpu.fp[dst]) == signbit(expected));
        break;
    default:
        if(sizeof(T2) == sizeof(float)) {
            BOOST_CHECK_CLOSE(
                mpfr_get_flt(regs.fpu.fp[dst], MPFR_RNDN), expected, 1e-04);
        } else if(sizeof(T2) == sizeof(double)) {
            BOOST_CHECK_CLOSE(mpfr_get_d(regs.fpu.fp[dst], MPFR_RNDN),
                                       expected, 1e-10);
        }
        break;
    }
}

#ifndef NAN
constexpr double NAN = std::numeric_limits<double>::quiet_NaN();
#endif
#ifndef INFINITY
constexpr double INFINITY = std::numeric_limits<double>::infinity();
#endif
