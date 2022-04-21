#define BOOST_TEST_DYN_LINK
#include "intop.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(or_, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_OR_B(0x12, 0x34) == (0x12 | 0x34));
}

BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_OR_B(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_OR_B(0, value << 7);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_OR_B(0, !value);
    BOOST_TEST(regs.z == value);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_OR_W(0x1234, 0x5678) == (0x1234 | 0x5678));
}
BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_OR_W(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_OR_W(0, value << 15);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_OR_W(0, !value);
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_OR_L(0x12345678, 0xaaaaaaaa) == (0x12345678 | 0xaaaaaaaa));
}

BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_OR_L(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_OR_L(0, value << 31);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_OR_L(0, !value);
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(and_, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_AND_B(0x12, 0x34) == (0x12 & 0x34));
}

BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_AND_B(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_AND_B(0x80, value << 7);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_AND_B(1, !value);
    BOOST_TEST(regs.z == value);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_AND_W(0x1234, 0x5678) == (0x1234 & 0x5678));
}
BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_AND_W(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_AND_W(0x8000, value << 15);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_AND_W(1, !value);
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_AND_L(0x12345678, 0xaaaaaaaa) == (0x12345678 & 0xaaaaaaaa));
}

BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_AND_L(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_AND_L(0x80000000, value << 31);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_AND_L(1, !value);
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(xor_, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_EOR_B(0x12, 0x34) == (0x12 ^ 0x34));
}

BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_EOR_B(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_EOR_B(0, value << 7);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_EOR_B(0, !value);
    BOOST_TEST(regs.z == value);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_EOR_W(0x1234, 0x5678) ^ (0x1234 & 0x5678));
}
BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_EOR_W(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_EOR_W(0, value << 15);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_EOR_W(0, !value);
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_EOR_L(0x12345678, 0xaaaaaaaa) ^ (0x12345678 & 0xaaaaaaaa));
}

BOOST_AUTO_TEST_CASE(vcx) {
    regs.v = regs.c = regs.x = true;
    DO_EOR_L(0, 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    DO_EOR_L(0, value << 31);
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    DO_EOR_L(0, !value);
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(add, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ADD_B(12, 34) == 46);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(v1) {
    DO_ADD_B(0x7f, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_ADD_B(1, 0x7f);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_ADD_B(0x80, 0xff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_ADD_B(0xff, 0x80);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c1) {
    DO_ADD_B(0xff, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_ADD_B(1, 0xff);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ADD_B(0, 0x80);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ADD_B(0, 0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ADD_W(1234, 5678) == (1234 + 5678));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(v1) {
    DO_ADD_W(0x7fff, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_ADD_W(1, 0x7fff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_ADD_W(0x8000, 0xffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_ADD_W(0xffff, 0x8000);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c1) {
    DO_ADD_W(0xffff, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_ADD_W(1, 0xffff);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ADD_W(0, 0x8000);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ADD_W(0, 0);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long) BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ADD_L(123456789, 987654321) == (123456789 + 987654321));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(v1) {
    DO_ADD_L(0x7fffffff, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_ADD_L(1, 0x7fffffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_ADD_L(0x80000000, 0xffffffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_ADD_L(0xffffffff, 0x80000000);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c1) {
    DO_ADD_L(0xffffffff, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_ADD_L(1, 0xffffffff);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ADD_L(0, 0x80000000);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ADD_L(0, 0);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(sub, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_SUB_B(40, 17) == 23);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(v1) {
    DO_SUB_B(0x7f, 0xff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_SUB_B(0, 0x80);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_SUB_B(0x80, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_SUB_B(0xfe, 0x7f);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c1) {
    DO_SUB_B(0xfe, 0xff);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_SUB_B(0, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_SUB_B(0, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_SUB_B(0, 0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_SUB_W(5678, 1234) == (5678 - 1234));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(v1) {
    DO_SUB_W(0x7fff, 0xffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_SUB_W(0, 0x8000);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_SUB_W(0x8000, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_SUB_W(0xfffe, 0x7fff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c1) {
    DO_SUB_W(0xfffe, 0xffff);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_SUB_W(0, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_SUB_W(0, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_SUB_W(0, 0);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_SUB_L(987654321, 123456789) == (987654321 - 123456789));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(v1) {
    DO_SUB_L(0x7fffffff, 0xffffffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_SUB_L(1, 0x80000000);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_SUB_L(0x80000000, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_SUB_L(0xfffffffe, 0x7fffffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c1) {
    DO_SUB_L(0xfffffffe, 0xffffffff);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_SUB_L(0, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_SUB_L(0, 0x80000000);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_SUB_L(0, 0);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(cmp, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(v1) {
    DO_CMP_B(0x7f, 0xff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_CMP_B(0, 0x80);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_CMP_B(0x80, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_CMP_B(0xfe, 0x7f);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c1) {
    DO_CMP_B(0xfe, 0xff);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_CMP_B(0, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_CMP_B(0, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_CMP_B(0, 0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(v1) {
    DO_CMP_W(0x7fff, 0xffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_CMP_W(0, 0x8000);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_CMP_W(0x8000, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_CMP_W(0xfffe, 0x7fff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_CMP_W(0xfffe, 0xffff);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_CMP_W(0, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_CMP_W(0, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_CMP_W(0, 0);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(v1) {
    DO_CMP_L(0x7fffffff, 0xffffffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v2) {
    DO_CMP_L(1, 0x80000000);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v3) {
    DO_CMP_L(0x80000000, 1);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(v4) {
    DO_CMP_L(0xfffffffe, 0x7fffffff);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_CMP_L(0xfffffffe, 0xffffffff);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c2) {
    DO_CMP_L(0, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_CMP_L(0, 0x80000000);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_CMP_L(0, 0);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(asr, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(positive) {
    BOOST_TEST(DO_ASR_B(32, 2) == 8);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(DO_ASR_B(-32, 2) == -8);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ASR_B(3, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_ASR_B(3, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ASR_B(1, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(positive) {
    BOOST_TEST(DO_ASR_W(1024, 2) == 256);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(DO_ASR_W(-1024, 2) == -256);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ASR_W(1025, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_ASR_W(1024, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ASR_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(positive) {
    BOOST_TEST(DO_ASR_L(0x10000, 2) == 0x4000);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(DO_ASR_L(-65536, 2) == -16384);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ASR_L(65537, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_ASR_L(65536, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ASR_L(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(asl, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ASL_B(4, 2) == 16);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
// 0 <- 0xx
BOOST_DATA_TEST_CASE(v1, boost::unit_test::data::xrange(4), value) {
    DO_ASL_B(0x10 | value << 5, 3);
    BOOST_TEST(regs.v == (value != 0));
}
// 1 <- 1xx
BOOST_DATA_TEST_CASE(v2, boost::unit_test::data::xrange(4), value) {
    DO_ASL_B(0x90 | value << 5, 3);
    BOOST_TEST(regs.v == (value != 3));
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ASL_B(0xc0, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_ASL_B(0x80, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ASL_B(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ASL_W(0x80, 2) == 0x200);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
// 0 <- 0xx
BOOST_DATA_TEST_CASE(v1, boost::unit_test::data::xrange(4), value) {
    DO_ASL_W(0x1000 | value << 13, 3);
    BOOST_TEST(regs.v == (value != 0));
}
// 1 <- 1xx
BOOST_DATA_TEST_CASE(v2, boost::unit_test::data::xrange(4), value) {
    DO_ASL_W(0x9000 | value << 13, 3);
    BOOST_TEST(regs.v == (value != 3));
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ASL_W(0xc000, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_ASL_W(0x8000, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ASL_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ASL_L(0x8000, 2) == 0x20000);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
// 0 <- 0xx
BOOST_DATA_TEST_CASE(v1, boost::unit_test::data::xrange(4), value) {
    DO_ASL_L(0x10000000 | value << 29, 3);
    BOOST_TEST(regs.v == (value != 0));
}
// 1 <- 1xx
BOOST_DATA_TEST_CASE(v2, boost::unit_test::data::xrange(4), value) {
    DO_ASL_L(0x90000000 | value << 29, 3);
    BOOST_TEST(regs.v == (value != 3));
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ASL_L(0xc0000000, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_ASL_L(0x80000000, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ASL_L(0, 1);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(lsr, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(positive) {
    BOOST_TEST(DO_LSR_B(32, 2) == 8);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_CASE(negative) { BOOST_TEST(DO_LSR_B(0x80, 2) == 0x20); }

BOOST_AUTO_TEST_CASE(cx) {
    DO_LSR_B(3, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_LSR_B(3, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_LSR_B(1, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(positive) {
    BOOST_TEST(DO_LSR_W(1024, 2) == 256);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_CASE(negative) { BOOST_TEST(DO_LSR_W(0x8000, 2) == 0x2000); }

BOOST_AUTO_TEST_CASE(cx) {
    DO_LSR_W(1025, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_LSR_W(1024, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_LSR_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(positive) {
    BOOST_TEST(DO_LSR_L(0x10000, 2) == 0x4000);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(DO_LSR_L(0x80000000, 2) == 0x20000000);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_LSR_L(65537, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_LSR_L(65536, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_LSR_L(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(lsl, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_LSL_B(4, 2) == 16);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_LSL_B(0xc0, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_LSL_B(0x80, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_LSL_B(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_LSL_W(0x80, 2) == 0x200);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_LSL_W(0xc000, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_LSL_W(0x8000, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_LSL_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_LSL_L(0x8000, 2) == 0x20000);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_LSL_L(0xc0000000, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.c = regs.x = true;
    DO_LSL_L(0x80000000, 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_LSL_L(0, 1);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(roxr, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(value, boost::unit_test::data::xrange(2), value) {
    regs.x = value;
    BOOST_TEST(DO_ROXR_B(0x39, 3) == (0x47 | value << 5));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ROXR_B(1, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.x = true;
    DO_ROXR_B(1, 0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.x = true;
    DO_ROXR_B(0, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROXR_B(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(value, boost::unit_test::data::xrange(2), value) {
    regs.x = value;
    BOOST_TEST(DO_ROXR_W(0x3009, 3) == (0x4601 | value << 13));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ROXR_W(1, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.x = true;
    DO_ROXR_W(1, 0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.x = true;
    DO_ROXR_W(0, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROXR_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(value, boost::unit_test::data::xrange(2), value) {
    regs.x = value;
    BOOST_TEST(DO_ROXR_L(0x30000009, 3) == (0x46000001 | value << 29));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ROXR_L(1, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.x = true;
    DO_ROXR_L(1, 0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.x = true;
    DO_ROXR_L(0, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROXR_L(0, 1);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(roxl, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(value, boost::unit_test::data::xrange(2), value) {
    regs.x = value;
    BOOST_TEST(DO_ROXL_B(0xc3, 3) == (0x1B | value << 2));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ROXL_B(0x80, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.x = true;
    DO_ROXL_B(0x80, 0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROXL_B(0x40, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROXL_B(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(value, boost::unit_test::data::xrange(2), value) {
    regs.x = value;
    BOOST_TEST(DO_ROXL_W(0xc003, 3) == (0x1b | value << 2));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ROXL_W(0x8000, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.x = true;
    DO_ROXL_W(1, 0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROXL_W(0x4000, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROXL_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(value, boost::unit_test::data::xrange(2), value) {
    regs.x = value;
    BOOST_TEST(DO_ROXL_L(0xc0000003, 3) == (0x1b | value << 2));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    DO_ROXL_L(0x80000000, 1);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx0) {
    regs.x = true;
    DO_ROXL_L(1, 0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROXL_L(0x40000000, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROXL_L(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ror, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ROR_B(0x12, 4) == 0x21);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_ROR_B(1, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c0) {
    DO_ROR_B(1, 0);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROR_B(1, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROR_B(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ROR_W(0x1234, 4) == 0x4123);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_ROR_W(1, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c0) {
    DO_ROXR_W(1, 0);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROR_W(1, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROR_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ROR_L(0x12345670, 4) == 0x01234567);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_ROR_L(1, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c0) {
    DO_ROXR_L(1, 0);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROR_L(1, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROR_L(0, 1);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(rol, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ROL_B(0x21, 4) == 0x12);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_ROL_B(0x80, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c0) {
    DO_ROL_B(0x80, 0);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROL_B(0x40, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROL_B(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ROL_W(0x4321, 4) == 0x3214);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_ROL_W(0x8000, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(cx0) {
    DO_ROL_W(0x8000, 0);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROL_W(0x4000, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROL_W(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(DO_ROL_L(0x07654321, 4) == 0x76543210);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(c) {
    DO_ROL_L(0x80000000, 1);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c0) {
    DO_ROL_L(0x80000000, 0);
    BOOST_TEST(! regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    DO_ROL_L(0x40000000, 1);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    DO_ROL_L(0, 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()