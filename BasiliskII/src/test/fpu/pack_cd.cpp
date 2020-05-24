#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
struct M68040_FPU : FPU {
	
	union FP_REG {
		float f;
		double d;
		long double ld;
	} FP[8];
	struct {
		enum { NEAREST = 0, ZERO, M_INF, P_INF } RND;
		enum { EXTENDED = 0, SINGLE, DOUBLE } PREC;
		FPU_EX ex_enable;
	} FPCR;
	struct {
		bool nan;
		bool I;
		bool Z;
		bool N;
		int16_t quotient;
		FPU_EX ex;
		bool INEX;
		bool DZ;
		bool UNFL;
		bool OFVL;
		bool IOP;
	} FPSR;
	uint32_t FPIAR;
	void exec(CPU* cpu, uint16_t op, int mode, int reg) override;
};
M68040_FPU fpu;


long double load_fd(CPU* cpu, M68040_FPU* fpu, bool rm, int t, int mode, int reg);

BOOST_FIXTURE_TEST_SUITE(fp_load, fixture);
BOOST_AUTO_TEST_CASE(fd)
{
	cpu.fpu = &fpu;
	FPU_R.FP[3].ld = 0.3;
	long double ll = load_fd(&cpu, &fpu, false, 3, 0, 0);
	BOOST_TEST( (double)ll == 0.3 );
}
BOOST_AUTO_TEST_CASE(i)
{
	cpu.fpu = &fpu;
	cpu.R[2] = 4;
	long double ll = load_fd(&cpu, &fpu, true, 0, 0, 2);
	BOOST_TEST( (double)ll == 4.0 );
}
BOOST_AUTO_TEST_CASE(s)
{
	cpu.fpu = &fpu;
	union iv_t {
		uint32_t i;
		float f;
	};
	iv_t i ;
	i.f = 0.4;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, i.i);
	long double ll = load_fd(&cpu, &fpu, true, 1, 2, 2);
	BOOST_TEST( (double)ll == 0.4, boost::test_tools::tolerance(0.1e-4) );
}

BOOST_AUTO_TEST_CASE(ld)
{
	cpu.fpu = &fpu;
	cpu.R[8+2] = 0x1000;
	test_write16(0x1000, 0x3fff);
	test_write16(0x1002, 0);
	test_write32(0x1004, 0xA6666666);
	test_write32(0x1008, 0x66666666);
	long double ll = load_fd(&cpu, &fpu, true, 2, 3, 2);
	BOOST_TEST( cpu.R[8+2] == 0x100C);
	BOOST_TEST( (double)ll == 1.3, boost::test_tools::tolerance(0.1e-4) );
}

BOOST_AUTO_TEST_CASE(p)
{
	cpu.fpu = &fpu;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x00070001);
	test_write32(0x1004, 0x30000000);
	test_write32(0x1008, 0);
	long double ll = load_fd(&cpu, &fpu, true, 3, 3, 2);
	BOOST_TEST( cpu.R[8+2] == 0x100C);
	BOOST_TEST( (double)ll == 1.3e+7, boost::test_tools::tolerance(0.1e-4) );
}

BOOST_AUTO_TEST_CASE(w)
{
	cpu.fpu = &fpu;
	cpu.R[2] = 333;
	long double ll = load_fd(&cpu, &fpu, true, 4, 0, 2);
	BOOST_TEST( (double)ll == 333 );
}


BOOST_AUTO_TEST_CASE(d)
{
	cpu.fpu = &fpu;
	cpu.R[8+2] = 0x1000;
	test_write32(0x1000, 0x00070001);
	test_write32(0x1004, 0x30000000);
	long double ll = load_fd(&cpu, &fpu, true, 5, 3, 2);
	BOOST_TEST( cpu.R[8+2] == 0x1008);
	BOOST_TEST( (double)ll == 333 );
}

BOOST_AUTO_TEST_CASE(b)
{
	cpu.fpu = &fpu;
	cpu.R[2] = 33;
	long double ll = load_fd(&cpu, &fpu, true, 6, 0, 2);
	BOOST_TEST( (double)ll == 33 );
}

BOOST_AUTO_TEST_SUITE_END();
