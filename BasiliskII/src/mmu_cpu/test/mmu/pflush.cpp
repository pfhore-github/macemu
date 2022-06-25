#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(PFLUSH, InitFix)
BOOST_AUTO_TEST_SUITE(PFLUSHN)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0172400);
    exception_check(8);
}
BOOST_AUTO_TEST_CASE(user_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 1;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    regs.a[1] = 0x1000;
    raw_write16(0, 0172401);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 0);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 1);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 1);
}

BOOST_AUTO_TEST_CASE(system_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 5;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    regs.a[1] = 0x1000;
    raw_write16(0, 0172401);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 1);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 1);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 1);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PFLUSH)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0172410);
    exception_check(8);
}
BOOST_AUTO_TEST_CASE(user_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 1;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    regs.a[1] = 0x1000;
    raw_write16(0, 0172411);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 0);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 1);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 1);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 1);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 1);
}

BOOST_AUTO_TEST_CASE(system_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 5;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    regs.a[1] = 0x1000;
    raw_write16(0, 0172411);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 1);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 1);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 1);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 1);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 1);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PFLUSHAN)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0172420);
    exception_check(8);
}
BOOST_AUTO_TEST_CASE(user_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 1;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    raw_write16(0, 0172421);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 0);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 0);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 0);
}

BOOST_AUTO_TEST_CASE(system_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 5;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    raw_write16(0, 0172421);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 1);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 0);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 0);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PFLUSHA)
BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0172430);
    exception_check(8);
}
BOOST_AUTO_TEST_CASE(user_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 1;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    raw_write16(0, 0172431);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 0);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 0);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 0);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 0);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 0);
}

BOOST_AUTO_TEST_CASE(system_page) {
    regs.S = true;
    regs.dfc = 1;
    mmu.pflush();
    regs.dfc = 5;
    mmu.pflush();
    regs.dfc = 5;
    mmu_d.atc_cache_g_s[1] = atc_entry_t{};
    mmu_d.atc_cache_g_u[1] = atc_entry_t{};
    mmu_d.atc_cache_l_s[1] = atc_entry_t{};
    mmu_d.atc_cache_l_u[1] = atc_entry_t{};
    mmu_i.atc_cache_g_s[1] = atc_entry_t{};
    mmu_i.atc_cache_g_u[1] = atc_entry_t{};
    mmu_i.atc_cache_l_s[1] = atc_entry_t{};
    mmu_i.atc_cache_l_u[1] = atc_entry_t{};

    mmu_d.atc_cache_g_s[2] = atc_entry_t{};
    mmu_d.atc_cache_g_u[2] = atc_entry_t{};
    mmu_d.atc_cache_l_s[2] = atc_entry_t{};
    mmu_d.atc_cache_l_u[2] = atc_entry_t{};
    mmu_i.atc_cache_g_s[2] = atc_entry_t{};
    mmu_i.atc_cache_g_u[2] = atc_entry_t{};
    mmu_i.atc_cache_l_s[2] = atc_entry_t{};
    mmu_i.atc_cache_l_u[2] = atc_entry_t{};

    regs.a[1] = 0x1000;
    raw_write16(0, 0172431);
    exception_check(0);

    BOOST_TEST(mmu_d.atc_cache_g_s.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_d.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_d.atc_cache_l_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_g_s.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_g_u.count(1) == 1);
    BOOST_TEST(mmu_i.atc_cache_l_s.count(1) == 0);
    BOOST_TEST(mmu_i.atc_cache_l_u.count(1) == 1);

    BOOST_TEST(mmu_d.atc_cache_g_s.size() == 0);
    BOOST_TEST(mmu_d.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_d.atc_cache_l_s.size() == 0);
    BOOST_TEST(mmu_d.atc_cache_l_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_g_s.size() == 0);
    BOOST_TEST(mmu_i.atc_cache_g_u.size() == 2);
    BOOST_TEST(mmu_i.atc_cache_l_s.size() == 0);
    BOOST_TEST(mmu_i.atc_cache_l_u.size() == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()