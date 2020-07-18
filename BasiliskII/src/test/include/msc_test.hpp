#pragma once
#include "io.hpp"
#include "msc.hpp"
#include "test_common.hpp"
#include "powerbook.hpp"
struct MockMSC : public PB_EX_REG {
	std::vector<uint8_t> executed_cmd;
	std::vector<uint8_t> do_cmd(uint8_t c) override {
		executed_cmd.push_back( c );
		executed_cmd.insert( executed_cmd.end(), buffers.begin(), buffers.end() );
		buffers.clear();
		return { 0 };
	}
};
