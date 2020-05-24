#ifndef MMU_HPP
#define MMU_HPP
#include "mem.hpp"
class MMU {
public:
	virtual void movec_to(int v, uint32_t value) = 0;
	virtual uint32_t movec_from(int v) = 0;
	virtual void exec(uint16_t op, int mode, int reg) = 0;
	virtual uint32_t ptest(uint32_t in) = 0;
	virtual uint32_t to_real(uint32_t addr, int attr) = 0;
	virtual void reset() = 0;
};
#include <unordered_map>
class MC68040_MMU : public MMU {
	std::unordered_map<uint32_t, uint32_t> g_caches;
	std::unordered_map<uint32_t, uint32_t> ng_caches;
	friend class fixture;
	friend class MMU_Tester;
	uint32_t URP;
	uint32_t SRP;
	uint32_t DTTR[2], ITTR[2];
	uint32_t MMU_SR;
	uint16_t TCR;
	uint32_t match_ttr(uint32_t ttr, uint32_t va, bool s);
	bool enable() const { return TCR &1 <<15; }
	bool large_page() const { return TCR &1 <<14; }
public:
	void flush_n(uint32_t v, bool n);
	uint32_t ptest(uint32_t v) override;
	uint32_t to_real(uint32_t addr, int attr) override;
	void movec_to(int v, uint32_t value) override; 
	uint32_t movec_from(int v) override;
	void exec(uint16_t op, int mode, int reg) override;
	void reset() override {
		DTTR[0] &= ~ (1<<15);
		DTTR[1] &= ~ (1<<15);
		ITTR[0] &= ~ (1<<15);
		ITTR[1] &= ~ (1<<15);
	}
};
/* TODO */
class MC68030_MMU : public MMU {
public:
	uint32_t ptest(uint32_t ) override { return 0; }
	uint32_t to_real(uint32_t , int ) override { return 0; }
	void movec_to(int , uint32_t ) override {}
	uint32_t movec_from(int ) override { return 0; }
	void exec(uint16_t , int , int ) override {}
	void reset() override {}
	void pmove(int , uint32_t ) {}
};

#endif
