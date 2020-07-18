#pragma once
#include "io.hpp"
#include "via.hpp"
#include "rbv.hpp"
#include "test_common.hpp"
#include <fmt/core.h>
struct VIA_TEST : public VIA {
	std::vector<STREAM_TEST> a, b;
	STREAM_TEST ca2, cb2;
	int irq_cnt = 0;
	uint8_t get_ora() { return orA; }
	uint8_t get_orb() { return orB; }
	void set_ora(uint8_t v) { orA = v; }
	void set_orb(uint8_t v) { orB = v; }
	void set_ira(uint8_t v) { irA = v; }
	void set_irb(uint8_t v) { irB = v; }
	VIA_TEST() :VIA(0), ca2("CA2"), cb2("CB2") {
		for(int i = 0; i < 8; ++i ) {
			a.emplace_back( fmt::format("Port A:{}", i) );
			b.emplace_back( fmt::format("Port B:{}", i) );
		}
	}
	~VIA_TEST() {}
	void reset_written() {
		for( auto& ap : a ) { ap.reset(); }
		for( auto& bp : b ) { bp.reset(); }
		ca2.reset();
		cb2.reset();
	}
protected:
	bool readA(int n) override { return a.at(n).in(); }
	void writeA(int n, bool v) override  { a.at(n).out(v); }
	bool readB(int n) override  { return b.at(n).in(); }
	void writeB(int n, bool v) override  { b.at(n).out(v); }
//	bool cb2_in_pull() override  { return cb2.out(); }
	void cb2_out(uint8_t v) override  { cb2.out(v); }
	void irq_pin() override  { ++irq_cnt; }
};

struct RBV_TEST : public RBV {
	std::vector<STREAM_TEST> a, b;
	RBV_TEST() :RBV() {
		for(int i = 0; i < 8; ++i ) {
			a.emplace_back( fmt::format("Port A:{}", i) );
			b.emplace_back( fmt::format("Port B:{}", i) );
		}
	}
	~RBV_TEST() {}
	void reset() {
		RBV::reset();
		for( auto& ap : a ) { ap.reset(); }
		for( auto& bp : b ) { bp.reset(); }
	}
protected:
	bool readA(int n) override { return a.at(n).in(); }
	void writeA(int n, bool v) override  { a.at(n).out(v); }
	bool readB(int n) override  { return b.at(n).in(); }
	void writeB(int n, bool v) override  { b.at(n).out(v); }
};
