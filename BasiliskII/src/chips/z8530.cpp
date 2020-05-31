#include "scc.hpp"
#include "devices/serial.hpp"
#include "scc_impl.hpp"
#include "registers.hpp"
std::shared_ptr<SCC> newZ8530(bool is_iop) {
	auto z8530 = std::make_shared<Z8530>();
	if( is_iop ) {
		return std::make_shared<SCC_IOP>( z8530 );
	} else {
		return std::make_shared<SCC>( z8530 );
	}
}

std::shared_ptr<SCC> newZ85C80(bool is_iop) {
	auto z85c80 = std::make_shared<Z85C80>();
	if( is_iop ) {
		return std::make_shared<SCC_IOP>( z85c80 );
	} else {
		return std::make_shared<SCC>( z85c80 );
	}
}

