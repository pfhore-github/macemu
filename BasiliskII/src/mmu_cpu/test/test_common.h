#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;
void asm_m68k(const char *a, int offset = 0);
void m68k_do_execute();
extern std::vector<uint8_t> RAM;

uint8_t raw_read8(uint32_t addr);
uint16_t raw_read16(uint32_t addr);
uint32_t raw_read32(uint32_t addr);
void raw_write8(uint32_t addr, uint8_t v);
void raw_write16(uint32_t addr, uint16_t v);
void raw_write32(uint32_t addr, uint32_t v);

const int SIGN[2] = {-1, 1};
const bool BIT[2] = {false, true};
inline auto REG() { return bdata::xrange(0, 8); }
const int REG2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
inline auto EA_D() { return bdata::xrange(8) + bdata::xrange(020, 072); }
inline auto EA_M() { return bdata::xrange(020, 072); }
inline auto EA_D_R() { return bdata::xrange(8) + bdata::xrange(020, 074); }
inline auto EA_M_R() { return bdata::xrange(020, 074); }
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536];
struct InitFix {
    InitFix();
};

void exception_check(int e);