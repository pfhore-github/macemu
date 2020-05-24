#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <atomic>
#include "asc.hpp"
#include "via.hpp"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "machine.hpp"
BOOST_FIXTURE_TEST_SUITE(asc_test, fixture);

BOOST_AUTO_TEST_CASE( sine ) {
	auto asc = newPlaneASC();
	asc->write(ASC_REG::MODE, 0);
	asc->write(ASC_REG::CLOCK, 0);
	asc->write(ASC_REG::CONTROL, 0);
	asc->write(ASC_REG::VOLUME, 0xe0);
	// reset freq table
	for(int j = 0; j < 32; ++j ) {
		asc->write(0x810 + j, 0);
	}

	asc->write(ASC_REG::FIFO_MODE, 0);
	asc->write(ASC_REG::MODE, 2);
	
	for(int j =0; j < 0x200; ++j ) {
		int8_t v = sin(j*2*M_PI/512)*64;
		asc->write(j, v^0x80);
		asc->write(j+0x200, v^0x80);
		asc->write(j+0x400, v^0x80);
		asc->write(j+0x600, v^0x80);
	}
	asc->write(0x815, 0x4);
	asc->write(0x816, 0x50);
	SDL_Delay(3000);
	
	asc->write(ASC_REG::MODE, 0);

}

BOOST_AUTO_TEST_CASE( square ) {
	auto asc = newPlaneASC();
	asc->write(ASC_REG::MODE, 0);
	asc->write(ASC_REG::CLOCK, 0);
	asc->write(ASC_REG::CONTROL, 0);
	asc->write(ASC_REG::VOLUME, 0xe0);
	// reset freq table
	for(int j = 0; j < 32; ++j ) {
		asc->write(0x810 + j, 0);
	}

	asc->write(ASC_REG::FIFO_MODE, 0);
	asc->write(ASC_REG::MODE, 2);
	
	for(int j =0; j < 0x200; ++j ) {
		int8_t v = j >= 0x100 ? -64 : 64;
		asc->write(j, v^0x80);
		asc->write(j+0x200, v^0x80);
		asc->write(j+0x400, v^0x80);
		asc->write(j+0x600, v^0x80);
	}
	asc->write(0x815, 0x4);
	asc->write(0x816, 0x50);
	SDL_Delay(3000);
	
	asc->write(ASC_REG::MODE, 0);

}

BOOST_AUTO_TEST_CASE( saw ) {
	auto asc = newPlaneASC();
	asc->write(ASC_REG::MODE, 0);
	asc->write(ASC_REG::CLOCK, 0);
	asc->write(ASC_REG::CONTROL, 0);
	asc->write(ASC_REG::VOLUME, 0xe0);
	// reset freq table
	for(int j = 0; j < 32; ++j ) {
		asc->write(0x810 + j, 0);
	}

	asc->write(ASC_REG::FIFO_MODE, 0);
	asc->write(ASC_REG::MODE, 2);
	
	for(int j =0; j < 0x200; ++j ) {
		int8_t v =  (j-256)/4;
		asc->write(j, v^0x80);
		asc->write(j+0x200, v^0x80);
		asc->write(j+0x400, v^0x80);
		asc->write(j+0x600, v^0x80);
	}
	asc->write(0x815, 0x4);
	asc->write(0x816, 0x50);
	SDL_Delay(3000);
	
	asc->write(ASC_REG::MODE, 0);

}



BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(easc_test, fixture);
void init_easc(const std::shared_ptr<ASC>& asc) {
	asc->write(ASC_REG::A_IRQ_CTL, 1);
	asc->write(ASC_REG::B_IRQ_CTL, 1);
	// FIFO RESET
	asc->write(ASC_REG::FIFO_MODE, asc->read(ASC_REG::FIFO_MODE) | 0x80);
	asc->write(ASC_REG::FIFO_MODE, asc->read(ASC_REG::FIFO_MODE) & 0x7f);
	// VOLUE RESET
	asc->write(ASC_REG::VOLUME, 0xe0);
	// MODE RESET to FIFO
	asc->write(ASC_REG::MODE, 1);
}

BOOST_AUTO_TEST_CASE( sine ) {
	auto asc = newEASC();
	init_easc(asc);
	int u = 0;
	asc->read(ASC_REG::FIFO_IRQ_STATUS);
	static int8_t sn_table[0x800];
	for(int k = 0; k < 0x800; ++k ) {
		sn_table[k] = int8_t(sin(k/1024.0*2*M_PI)*64.0) ^ 0x80;
	}
	for(int k = 0;k < 0x40; ++k ) {
		do {
			asc->write(0, sn_table[((++u)%169)*0x800/169]);
			asc->write(0x400, 0x80);
		} while( asc->read(ASC_REG::FIFO_IRQ_STATUS) == 0 );
	}
	while( 	asc->read(ASC_REG::FIFO_IRQ_STATUS) != FIFO_FULL ) {
		__builtin_ia32_pause();
	}
	asc->write(ASC_REG::MODE, 0);

}


BOOST_AUTO_TEST_CASE( square ) {
	auto asc = newEASC();
	init_easc(asc);
	int u = 0;
	static int8_t sq_table[0x800];
	for(int k = 0; k < 0x800; ++k ) {
		sq_table[k] = (k & 0x400 ? -64 : 64) ^ 0x80;
	}
	for(int k = 0;k < 0x40; ++k ) {
		do {
			asc->write(0, sq_table[((++u)%169)*0x800/169]);
			asc->write(0x400, 0x80);
		} while( asc->read(ASC_REG::FIFO_IRQ_STATUS) == 0 );
	}
	while( 	asc->read(ASC_REG::FIFO_IRQ_STATUS) != FIFO_FULL ) {
		__builtin_ia32_pause();
	}
	
	asc->write(ASC_REG::MODE, 0);

}



BOOST_AUTO_TEST_SUITE_END();
