#include "68881.hpp"
#include <fenv.h>
#include <math.h>
#include "../inst_cmn.hpp"
#include "../exceptions.hpp"
#define SELF ((M68040_FPU*)fpu)
enum PREC_T { EXTENDED, SINGLE, DOUBLE };

constexpr uint32_t FPCR_N = 1 << 27;
constexpr uint32_t FPCR_Z = 1 << 26;
constexpr uint32_t FPCR_I = 1 << 25;
constexpr uint32_t FPCR_NAN = 1 << 24;
inline PREC_T get_fpcr_prec(uint16_t v) { return PREC_T(v >> 6 & 3); }
#define OP(name_) extern "C" void op_##name_(uint16_t op, long double a, int dst)
OP(fabs) {
	SELF->set(dst, fabsl(a));
}

OP(fsabs) {
	SELF->set(dst, fabsf(a));
}
OP(fdabs) {
	SELF->set(dst, fabs(a));
}

OP(facos) {
	SELF->set(dst, acosl(a));
}

OP(fadd) {
	SELF->set(dst, SELF->FP[dst] + a);
	SELF->e3 = true;
}

OP(fsadd) {
	SELF->set(dst, float(SELF->FP[dst] + a));
}

OP(fdadd) {
	SELF->set(dst, double(SELF->FP[dst] + a));
}

OP(fasin) {
	SELF->set(dst, asinl(a));
}

OP(fatan) {
	SELF->set(dst, atanl(a));
}

OP(fatanh) {
	SELF->set(dst, atanhl(a));
}

OP(fcmp) {
	SELF->FPSR_cond &= ~ 2;
	SELF->FPSR_cond |= (isnan(SELF->FP[dst]) || isnan(SELF->FP[dst]));
	SELF->FPSR_cond |= ( SELF->FP[dst] == a ) << 2;
	SELF->FPSR_cond |= ( SELF->FP[dst] < a ) << 3;
}

OP(fcos) {
	SELF->set(dst, cosl(a));
}

OP(fcosh) {
	SELF->set(dst, coshl(a));
}

OP(ftst) {
	SELF->test_fp(a);
}

void fscc(int mode, int reg, int op) {
	if( mode == 1 ) {
		// FDBCC
		int16_t disp = fetch_w();
		if( SELF->cond(op) ) {
			if( int16_t(--cpu.R[reg]) != -1 ) {
				if( disp & 1 ) {
					throw Exception::AddressError(cpu.PC + disp);
				}
				cpu.PC += disp;
			}
		}
	} else if( mode == 7 && reg == 2 ) {
		// FTRAPcc.W
		int16_t imm = fetch_w();
		(void)imm;
		if( SELF->cond(op) ) {
			throw Exception::TrapCC();
		}
		return;
	} else if( mode == 7 && reg == 3 ) {
		// FTRAPcc.L
		int32_t imm = fetch_l();
		(void)imm;
		if( SELF->cond(op) ) {
			throw Exception::TrapCC();
		}
		return;
	} else if( mode == 7 && reg == 4 ) {
		// FTRAPcc.
		if( SELF->cond(op) ) {
			throw Exception::TrapCC();
		}
		return;
	} else {
		// FScc
		int re = SELF->cond( op ) ? 0xff : 0;
		cpu.parse_EA(mode, 1, reg, true);
		cpu.ea_write( re );
	}
}
void fbcc(int op, int32_t disp) {
	if( SELF->cond( op & 0x3f ) ) {
		if( disp & 1 ) {
			throw Exception::AddressError(cpu.PC + disp);
		}
		cpu.PC += disp;
		test_trace_branch();
	}
}

void fdbcc(uint16_t op) {
	uint16_t cond = fetch_w();
	int16_t disp = fetch_w();
	int dn = op & 7;
	if( SELF->cond( cond & 0x3f ) )
		return;
	if( int16_t(--cpu.R[dn]) != -1 ) {
		if( disp & 1 ) {
			throw Exception::AddressError(cpu.PC + disp);
		}
		cpu.PC += disp;
	}
	test_trace_branch();
}

OP(fdiv) {
	SELF->set(dst, SELF->FP[dst] / a);
	SELF->e3 = true;
}

OP(fsgldiv) {
	SELF->set(dst, float(SELF->FP[dst] / a));
}

OP(fsdiv) {
	SELF->set(dst, float(SELF->FP[dst] / a));
}

OP(fddiv) {
	SELF->set(dst, double(SELF->FP[dst] / a));
}

OP(fetox) {
	SELF->set(dst, expl(a));
}

OP(fetoxm1) {
	SELF->set(dst, expm1l(a));
}

OP(fgetexp) {
	if( isinf(a) ) {
		SELF->FPSR_ex |= EX_OPERR;
		SELF->set(dst, NAN);
	} else if( a == 0.0 ) {
		SELF->set(dst, 0.0);
	} else {
		int ci;
		frexpl(a, &ci);
		long double c= ci - 1;
		SELF->set(dst, c);
	}
}

OP(fgetman) {
	if( isinf(a) ) {
		SELF->FPSR_ex |= EX_OPERR;
		SELF->set(dst, NAN);
	} else {
		int ci;
		long double c = frexpl(a, &ci)*2;
		SELF->set(dst, c);
	}
}

OP(fint) {
	SELF->set(dst, nearbyintl(a));
}

OP(fintrz) {
	SELF->set(dst, truncl(a));
}

OP(flog10) {
	SELF->set(dst, log10l(a));
}	

OP(flog2) {
	SELF->set(dst, log2l(a));
}	

OP(flogn) {
	SELF->set(dst, logl(a));
}	

OP(flognp1) {
	SELF->set(dst, log1pl(a));
}

OP(fmod) {
	if( isinf(SELF->FP[dst]) || a == 0.0 ) {
		SELF->FPSR_ex |= EX_OPERR;
		SELF->set(dst, NAN);
	} else {
		SELF->FPSR_quotient = (int16_t)nearbyintl(SELF->FP[dst] / a);
		SELF->set(dst, fmodl(SELF->FP[dst], a));
	}
}	


OP(fmove_to_reg) {
	SELF->set(dst, a);	
}

OP(fsmove_to_reg) {
	SELF->set(dst, (float)a);	
}

OP(fdmove_to_reg) {
	SELF->set(dst, (double)a);	
}

extern "C" void fmove_cr(int dst, uint8_t op) {
	switch(op) {
	case 0    : SELF->set(dst, M_PI); break;
	case 0x0b : SELF->set(dst, M_LN2/M_LN10); break;
	case 0x0c : SELF->set(dst, M_E); break;
	case 0x0d : SELF->set(dst, M_LOG2E); break;
	case 0x0e : SELF->set(dst, M_LOG10E); break;
	case 0x0f : SELF->set(dst, 0.0); break;
	case 0x30 : SELF->set(dst, M_LN2); break;
	case 0x31 : SELF->set(dst, M_LN10); break;
	case 0x32 : SELF->set(dst, 1.0); break;
	case 0x33 : SELF->set(dst, 10.0); break;
	case 0x34 : SELF->set(dst, 1e2); break;
	case 0x35 : SELF->set(dst, 1e4); break;
	case 0x36 : SELF->set(dst, 1e8); break;
	case 0x37 : SELF->set(dst, 1e16); break;
	case 0x38 : SELF->set(dst, 1e32); break;
	case 0x39 : SELF->set(dst, 1e64); break;
	case 0x3A : SELF->set(dst, 1e128); break;
	case 0x3B : SELF->set(dst, 1e256); break;
	case 0x3C : SELF->set(dst, 1e512L); break;
	case 0x3D : SELF->set(dst, 1e1024L); break;
	case 0x3E : SELF->set(dst, 1e2048L); break;
	case 0x3F : SELF->set(dst, 1e4096L); break;
	default: SELF->set(dst, 0.0); break;
	}
}

OP(fmul) {
	SELF->set(dst, SELF->FP[dst] * a);
	SELF->e3 = true;
}

OP(fsglmul) {
	SELF->set(dst, float(SELF->FP[dst] * a));
}

OP(fsmul) {
	SELF->set(dst, float(SELF->FP[dst] * a));
}

OP(fdmul) {
	SELF->set(dst, double(SELF->FP[dst] * a));
}

OP(fneg) {
	SELF->set(dst, -a);
}

OP(fsneg) {
	SELF->set(dst, float(-a));
}

OP(fdneg) {
	SELF->set(dst, double(-a));
}

OP(frem) {
	SELF->FPSR_quotient = (int16_t)roundl(SELF->FP[dst] / a);
	SELF->set(dst, remainderl(SELF->FP[dst], a));
}	

OP(fscale) {
	SELF->set(dst, ldexpl(SELF->FP[dst], (int)a));
}

OP(fsin) {
	SELF->set(dst, sinl(a));
}

OP(fsinh) {
	SELF->set(dst, sinhl(a));
}

OP(fsqrt) {
	SELF->set(dst, sqrtl(a));
	SELF->e3 = true;
}

OP(fssqrt) {
	SELF->set(dst, sqrtf(a));
}

OP(fdsqrt) {
	SELF->set(dst, sqrt(a));
}

OP(ftan) {
	SELF->set(dst, tanl(a));
}

OP(ftanh) {
	SELF->set(dst, tanhl(a));
}

OP(ftwotox) {
	SELF->set(dst, exp2l(a));
}

OP(ftentox) {
	SELF->set(dst, powl(10.0, a));
}

OP(fsub) {
	SELF->set(dst, SELF->FP[dst] - a);
	SELF->e3 = true;
}

OP(fssub) {
	SELF->set(dst, float(SELF->FP[dst] - a));
}
OP(fdsub) {
	SELF->set(dst, double(SELF->FP[dst] - a));
}

OP(fsincos) {
	int fpc = op & 7; 
	long double s, c;
#ifdef _GNU_SOURCE
	sincosl(a, &s, &c);
#else
	s = sinl(a);
	c = cosl(a);
#endif
	SELF->set(dst, s);
	SELF->set(fpc, c);
}

extern "C" void op_fmovem_to_cc(int mode, int reg,
								bool fpcr, bool fpsr, bool fpir) {
	if( mode == 0 ) {
		if( fpcr + fpsr +fpir == 1 ) {
			if( fpcr ) {
				SELF->set_fpcr(cpu.R[reg]);
			} else if( fpsr ) {
				SELF->set_fpsr(cpu.R[reg]);
			}
		} else {
			throw Exception::IllegalInst();
		}
		return;
	}
	if( mode == 1 ) {
		if( fpir && ! fpcr && ! fpsr ) {
			SELF->set_iar(cpu.R[8+reg]);
			return;
		} else {
			throw Exception::IllegalInst();
		}
	}
	if( mode == 7 && reg == 4 ) {
		uint32_t d = fetch_l();
		if( fpcr ) {
			SELF->set_fpcr(d);
		}
		if( fpsr ) {
			SELF->set_fpsr(d);
		}
		if( fpir ) {
			SELF->set_iar(d);
		}
		return ;
	}
	int sz = fpcr + fpsr + fpir;
	cpu.parse_EA(mode, sz*4, reg, false);
	uint32_t addr = cpu.ea_v;
	if( fpcr ) {
		SELF->set_fpcr(read_l(addr));
		addr += 4;
	}
	if( fpsr ) {
		SELF->set_fpsr(read_l(addr));
		addr += 4;
	}
	if( fpir) {
		SELF->set_iar(read_l(addr));
	}
}

extern "C" void op_fmovem_from_cc(int mode, int reg,
								  bool fpcr, bool fpsr, bool fpir) {
	if( mode == 0 ) {
		if( fpcr + fpsr +fpir == 1 ) {			
			if( fpcr ) {
				cpu.R[reg] = SELF->get_fpcr();
				return;
			}
			if( fpsr ) {
				cpu.R[reg] = SELF->get_fpsr();
				return;
			}
			if( fpir ) {
				cpu.R[reg] = SELF->get_iar();
				return;
			}
		} else {
			throw Exception::IllegalInst();
		}
		return;
	}
	if( mode == 1 ) {
		if( !fpcr && !fpsr && fpir ) {
			cpu.R[8+reg] = SELF->get_iar();
			return;
		}
	}
	int sz = fpcr + fpsr + fpir;
	cpu.parse_EA(mode, sz*4, reg, false);
	uint32_t addr = cpu.ea_v;
	if( fpcr ) {
		write_l(addr, SELF->get_fpcr());
		addr += 4;
	}
	if( fpsr ) {
		write_l(addr, SELF->get_fpsr());
		addr += 4;
	}
	if( fpir ) {
		write_l(addr, SELF->get_iar());
	}
}
void m68k_execute();

extern "C" void op_fmovem_to_regs(int mode, int reg, bool is_dn,
								  uint8_t regs) {
	if( is_dn ) {
		regs = cpu.R[(regs>>4)&7];
	}
	uint32_t addr;
	if( ! cpu.restart ) {
		if( mode == 2 || mode > 4 ) {
			cpu.parse_EA(mode, 12, reg, false);
			addr = cpu.ea_v;
		} else if( mode == 3 ) {
			addr = cpu.R[8+reg];
		} else {
			throw Exception::IllegalInst();
		}
	} else {
		addr = cpu.ea_v;
	}
	try {
		for(int i = 0; i < 8; ++i ) {
			if( regs & (1 << (7-i))) {
				SELF->FP[i] = read_ld(addr);
				addr += 12;
			}
		}
		cpu.restart = false;
		if( mode == 3 ) {
			cpu.R[8+reg] = addr;
		}
		test_trace_branch();
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}			

}


extern "C" void op_fmovem_from_regs(int mode, int reg, bool is_dn,
									uint8_t regs) {
	if( is_dn ) {
		regs = cpu.R[(regs>>4)&7];
	}
	if( mode == 2 || mode > 4 ) {
		if( ! cpu.restart ) {
			cpu.parse_EA( mode, 12, reg, false);
		}
		uint32_t addr = cpu.ea_v;
		try {
			for(int i = 0; i < 8; ++i ) {
				if( regs & (1 << (7-i))) {
					write_ld( addr, SELF->FP[i]);
					addr += 12;
				}
			}
			cpu.restart = false;
		} catch( Exception::AccessFault& e) {
			e.ssw |= 1 << 13; // CM
			throw e;
		}			
	} else if( mode == 4 ) {
		if( ! cpu.restart ) {
			cpu.ea_v = cpu.R[8+reg];
		}
		uint32_t addr = cpu.ea_v;
		try {
			for(int i = 7; i >= 0; --i ) {
				if( regs & (1 << i)) {
					write_ld( addr -= 12, SELF->FP[i]);
				}
			}
			cpu.R[8+reg] = addr;
			cpu.restart = false;
		} catch( Exception::AccessFault& e) {
			e.ssw |= 1 << 13; // CM
			throw e;
		}			
	} else {
		throw Exception::IllegalInst();
	}
	test_trace_branch();
}

uint16_t M68040_FPU::get_fpcr() {
	uint16_t v = FPCR;
	switch( fegetround() ) {
	case FE_TONEAREST : v |= 0 << 4; break;
	case FE_UPWARD : v |= 3 << 4; break;
	case FE_DOWNWARD : v |= 2 << 4; break;
	case FE_TOWARDZERO : v |= 1 << 4; break;
	}
	return v;
}

void M68040_FPU::set_fpcr(uint16_t v) {
	switch(v>>4 & 3) {
	case 0 : fesetround(FE_TONEAREST); break;
	case 1 : fesetround(FE_TOWARDZERO);break;
	case 2 : fesetround(FE_DOWNWARD);break;
	case 3 : fesetround(FE_UPWARD);break;
	}
	FPCR = v &~ 0x3f;
}

uint32_t M68040_FPU::get_fpsr() {
	return FPSR_cond << 24 | (uint8_t)FPSR_quotient << 16
		| FPSR_ex << 8 | FPSR_aex;
}
void M68040_FPU::set_fpsr(uint32_t v) {
	FPSR_cond = v >> 24;
	FPSR_quotient = v >> 16;
	FPSR_ex = v >> 8;
	FPSR_aex = v;
}
void M68040_FPU::exec(uint16_t op, int mode, int reg) {
	switch((op>>6)&7) {
	case 0 :
		break;
	case 1 :
		// Fscc/FDBcc
		fscc(mode, reg, fetch_w() & 0x3f);
		return;
	case 2 : {
		// FBcc.W
		int16_t off = fetch_w();
		fbcc(op & 0x3f, off);
		return;
	}
	case 3 : {
		// FBcc.L
		int32_t off = fetch_l();
		fbcc(op & 0x3f, off);
		return;
	}
	case 4 : 
		// FSAVE
		if( ! cpu.S ) {
			throw Exception::PrivilegeViolation();
		} else {
			cpu.parse_EA(mode, 4, reg, true);
			uint32_t addr = cpu.ea_v;
			if( idle ) {
				write_l(addr, 0x41000000);
			} else {
				// TODO; WBTE isn't saved.
				write_l(addr, 0x41600000);
				write_l(addr+8, cu_savepc);
				write_ld(addr+0x18, wbt);
				write_l(addr+0x28, fpiarcu);
				write_w(addr+0x34, cmdreg3b);
				write_w(addr+0x3c, stag << 28 );
				write_w(addr+0x40, cmdreg1b);
				write_w(addr+0x44, dtag << 28 );
				write_w(addr+0x48, e1 << 10 | e3 << 9 | t << 4);
				write_ld(addr+0x4c, fpt);
				write_ld(addr+0x58, et);
			}
			test_trace_branch();
			return;
		}
	case 5 : {
		// FRESTORE
		if( ! cpu.S ) {
			throw Exception::PrivilegeViolation();
		}
		cpu.parse_EA(mode, 4, reg, true);
		(void)cpu.ea_read();
		test_trace_branch();
		return;
	}
	default:
		throw Exception::IllegalInst();
	}
	uint16_t nw = fetch_w();
	if( nw & (1<<15)) {
		bool is_w = nw & (1 << 13);
		if( nw & (1<<14)) {
			bool is_dn = nw & (1<<11);
			if( is_w ) {
				op_fmovem_from_regs(mode, reg, is_dn, nw & 0xff);
			} else {
				op_fmovem_to_regs(mode, reg, is_dn, nw & 0xff);
			}
		} else {
			if( is_w ) {
				// to memory
				op_fmovem_from_cc(mode, reg,
								  nw & (1 << 12), nw & ( 1 << 11), nw & (1 << 10));
			} else {
				// from memory
				op_fmovem_to_cc(mode, reg,
								nw & (1 << 12), nw & ( 1 << 11), nw & (1 << 10));
			}
		}
	} else if( nw & (1 << 13)) {
		// fmove to
		signed char k = nw & 0x7f;
		if( ((nw>>10)&7) == 7 ) {
			k = cpu.R[k>>4];
		} else {
			k = (signed char)(k << 1) >> 1;
		}
		t = true;
		store_fd((nw>>10)&7, (op>>3)&7, op&7, FP[(nw>>7)&7],k);
	} else if( (nw & (1 << 14)) && ( (nw & (7<<10)) == (7<<10)) ) {
		fmove_cr( (nw >> 7) & 7, nw & 0x7f);
	} else {
		stag = 0;
		long double ld = load_fd(nw & (1 << 14), (nw >> 10)&7, mode, reg);
		// set internal data
		idle = false;
		cmdreg1b = nw;
		cmdreg3b = (nw & 0x3c3) |
			(( nw << 3 ) & 0x20) |
			(( nw >> 1 ) & 0x1c);
		cu_savepc = cpu.PC;
		fpiarcu = cpu.NPC;
		opclass = (cmdreg1b >> 13) & 3;
		et = ld;
		e1 = e3 = false;
		t = false;
		if( stag != 5 ) {
			switch( fpclassify(et) ) {
			case FP_NORMAL: stag = 0; break;
			case FP_ZERO: stag = 1; break;
			case FP_INFINITE: stag = 2; break;
			case FP_NAN: stag = 3; break;
			case FP_SUBNORMAL: stag = 4; break;
			}
		}
		fpt = FP[(nw >> 7) & 7];
		switch( fpclassify(fpt) ) {
		case FP_NORMAL: dtag = 0; break;
		case FP_ZERO: dtag = 1; break;
		case FP_INFINITE: dtag = 2; break;
		case FP_NAN: dtag = 3; break;
		case FP_SUBNORMAL: dtag = 4; break;
		}
		if( issignaling(ld) ) {
			FPSR_ex |= EX_SNAN;
			FPSR_aex |= EX_ACC_IOP;
			if( FPCR & 1 << 14 ) {
				FPIAR = cpu.NPC;
				e1 = true;
				throw Exception::FP_SNAN();
			} else {
				// clear SNAN
				char tmp[12];
				memcpy(tmp, &ld, 12);
				tmp[4] |= 1 << 30;
				memcpy(&ld, tmp, 12);
			}
		} else {
			feclearexcept(FE_ALL_EXCEPT);
			FPSR_ex &= ~ 0x3e;

			if( op_list_fpu[nw&0x7f] ) {
				op_list_fpu[nw&0x7f](nw, ld, (nw >> 7) & 7);
			} else {
				FPIAR = cpu.PC-2;
//				throw Exception::FP_unimplemented_inst();
				throw Exception::FLine();
			}
			idle = true;
		}
	}
	
}

void M68040_FPU::set(int dstn, long double ld) {
	test_fp(ld);
	switch( get_fpcr_prec(FPCR)) {
	case EXTENDED: FP[dstn] = ld; break;
	case SINGLE: FP[dstn] = (float)ld; break;
	case DOUBLE: FP[dstn] = (double)ld; break;
	}	
}


void NoFPU::reset() {}
void NoFPU::exec(uint16_t , int , int ) {
	throw Exception::FLine();
}
void M68040_FPU::reset() {
	memset(FP, 0, sizeof(long double)*8);
	FPCR = 0;
	FPSR_aex = FPSR_ex = FPSR_quotient = FPSR_cond = 0;
	fesetround( FE_TONEAREST );
}

M68040_FPU::M68040_FPU() {
	op_list_fpu[0x00] = op_fmove_to_reg;
	op_list_fpu[0x01] = op_fint;
	op_list_fpu[0x02] = op_fsinh;
	op_list_fpu[0x03] = op_fintrz;
	op_list_fpu[0x04] = op_fsqrt;
	op_list_fpu[0x06] = op_flognp1;
	op_list_fpu[0x08] = op_fetoxm1;
	op_list_fpu[0x09] = op_ftanh;
	op_list_fpu[0x0A] = op_fatan;
	op_list_fpu[0x0C] = op_fasin;
	op_list_fpu[0x0D] = op_fatanh;
	op_list_fpu[0x0E] = op_fsin;
	op_list_fpu[0x0F] = op_ftan;
	op_list_fpu[0x10] = op_fetox;
	op_list_fpu[0x11] = op_ftwotox;
	op_list_fpu[0x12] = op_ftentox;
	op_list_fpu[0x14] = op_flogn;
	op_list_fpu[0x15] = op_flog10;
	op_list_fpu[0x16] = op_flog2;
	op_list_fpu[0x18] = op_fabs;
	op_list_fpu[0x19] = op_fcosh;
	op_list_fpu[0x1A] = op_fneg;
	op_list_fpu[0x1C] = op_facos;
	op_list_fpu[0x1D] = op_fcos;
	op_list_fpu[0x1E] = op_fgetexp;
	op_list_fpu[0x1F] = op_fgetman;
	op_list_fpu[0x20] = op_fdiv;
	op_list_fpu[0x21] = op_fmod;
	op_list_fpu[0x22] = op_fadd;
	op_list_fpu[0x23] = op_fmul;
	op_list_fpu[0x24] = op_fsgldiv;
	op_list_fpu[0x25] = op_frem;
	op_list_fpu[0x26] = op_fscale;
	op_list_fpu[0x27] = op_fsglmul;
	op_list_fpu[0x28] = op_fsub;
	for(int fpc=0; fpc < 8; ++fpc) {
		op_list_fpu[0x30|fpc] = op_fsincos;
	}
	op_list_fpu[0x38] = op_fcmp;
	op_list_fpu[0x3A] = op_ftst;
	op_list_fpu[0x40] = op_fsmove_to_reg;
	op_list_fpu[0x41] = op_fssqrt;
	op_list_fpu[0x44] = op_fdmove_to_reg;
	op_list_fpu[0x45] = op_fdsqrt;
	op_list_fpu[0x58] = op_fsabs;
	op_list_fpu[0x5A] = op_fsneg;
	op_list_fpu[0x5C] = op_fdabs;
	op_list_fpu[0x5E] = op_fdneg;
	op_list_fpu[0x60] = op_fsdiv;
	op_list_fpu[0x62] = op_fsadd;
	op_list_fpu[0x63] = op_fsmul;
	op_list_fpu[0x64] = op_fddiv;
	op_list_fpu[0x66] = op_fdadd;
	op_list_fpu[0x67] = op_fdmul;
	op_list_fpu[0x68] = op_fssub;
	op_list_fpu[0x6C] = op_fdsub;
}
