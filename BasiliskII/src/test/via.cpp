#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include "chips/via.h"
void m68k_do_execute();

BOOST_FIXTURE_TEST_SUITE(test_via, InitFix)
class VIA_TEST : public VIA {
  public:
    VIA_TEST() : VIA(1) {}
    bool readA(int n) override { return false;}
    void writeA(int n, bool v) override { }
    bool readB(int n) override { return false;}
    void writeB(int n, bool v) override {}
};
BOOST_AUTO_TEST_CASE(works) {
    VIA_TEST v;
    v.read_irq_enable();
    uint8_t d1l = 0xff;
    uint8_t oldv = v.read_irq_enable();
    v.write_irq_enable(d1l);
    do {
        d1l = (-d1l&0xff) * 2;
        if(d1l == 0) {
            break;
        }
        d1l = (-d1l&0xff);
    } while(v.read_irq_enable() == d1l);
    v.write_irq_enable(0x7f);
    v.write_irq_enable(oldv);
}
BOOST_AUTO_TEST_SUITE_END()