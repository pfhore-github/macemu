#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>


BOOST_FIXTURE_TEST_SUITE(FMOVECR, InitFix)
static const int dbl_off[] = {0, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x30, 0x31};
static const double dbl_const[] = {
    M_PI, M_LN2 / M_LN10, M_E, M_LOG2E, M_LOG10E, 0.0, M_LN2, M_LN10};

BOOST_DATA_TEST_CASE(math_const, dbl_off ^ dbl_const, offset, value) {
    auto dstr = rand_reg();
    raw_write16(0, 0171000);
    raw_write16(2, 0x5C00 | dstr << 7 | offset);
    m68k_do_execute();
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[dstr]), value, 1e-08);
}

static const int exps[] = {0,  1,   2,   4,   8,    16,   32,
                           64, 128, 256, 512, 1024, 2048, 4096};
BOOST_DATA_TEST_CASE(exp10_const, bdata::xrange(14) ^ exps, i, value) {
    auto dstr = rand_reg();
    raw_write16(0, 0171000);
    raw_write16(2, 0x5C00 | dstr << 7 | (0x32 + i));
    m68k_do_execute();
    MPFR_DECL_INIT(tmp, 64);
    mpfr_log10(tmp, fpu.fp[dstr].ptr(), MPFR_RNDN);
    BOOST_TEST(mpfr_get_ui(tmp, MPFR_RNDN) == value);
}
BOOST_AUTO_TEST_SUITE_END()
