#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <stdio.h>
#include "machine.hpp"
#include "iop.hpp"

namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(iop);
BOOST_AUTO_TEST_CASE( exec ) {
	IOP iop;
	uint8_t ctrl = iop.read(IOP_REG::CTRL) & (IOP_RUN|IOP_AUTOINC);
	// get addr
	uint16_t addr = iop.read(IOP_REG::ADDR_HI) << 8 |
		iop.read(IOP_REG::ADDR_LO);
	// enable auto increment
	iop.write(IOP_REG::CTRL, ctrl | IOP_AUTOINC );
	// read memory(so increment addr)
	iop.read(IOP_REG::RAM);
	// read addr again
	int16_t addr_diff = (iop.read(IOP_REG::ADDR_HI) << 8 |
						 iop.read(IOP_REG::ADDR_LO)) - addr;
	// restore addr
	iop.write(IOP_REG::ADDR_HI, addr >> 8);
	iop.write(IOP_REG::ADDR_LO, addr );
	// reset IRQ
	iop.write(IOP_REG::CTRL, ctrl &~ (IOP_INT0 | IOP_INT1));
	BOOST_TEST( addr_diff == 1 );
}



BOOST_AUTO_TEST_SUITE_END();
