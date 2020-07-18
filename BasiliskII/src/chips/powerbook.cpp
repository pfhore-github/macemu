#include "io.hpp"
#include <vector>
#include "powerbook.hpp"
#include <stdio.h>
#include <string.h>
std::optional<uint8_t> PB_EX_REG::pop_out() {
	if( c_out.empty() ) {
		return {};
	} else {
		uint8_t ret = c_out.front();
		c_out.pop_front();
		return ret;
	}
}
std::vector<uint8_t> PB_EX_REG::do_cmd(uint8_t c) {
	switch(c) {
	case 0x3A :
		printf("CMD: $3A %02x%02x\n", buffers[0], buffers[1] );
		return { 0x3a };
	case 0x71 :
		printf("CMD: $71\n" );
		return { };		
	case 0xE1 :
		memcpy(firmware, buffers.data(), 0x8004 );
		firmware_loaded = true;
		return {};
	case 0xE2:				// ROM VERSION?
		return { uint8_t( firmware_loaded ? 0x62 : 0 ) };
	default:
		printf("CMD: %02x\n", c);
		return {};
	}
}
unsigned int PB_EX_REG::arg_size(uint8_t cmd) {
	switch(cmd) {
	case 0x3A : return 2;				// unknown command
	case 0x71 : return 0;				// unknown command
	case 0xE1 : return 0x8004;			// send firmware
	case 0xE2 : return 0;				// ROM VERSION?
	default : return 0;
	}
}
void PB_EX_REG::cmd(uint8_t c) {
	std::vector<uint8_t> ret;
	if( in_cmd == -1 ) {
		in_cmd = c;
	} else {
		buffers.push_back( c );
	}
	if( buffers.size() == arg_size( in_cmd) ) {
		ret = do_cmd( in_cmd );
		in_cmd = -1;
		buffers.clear();
		c_out.insert( c_out.end(), ret.begin(), ret.end() );
	}
}

