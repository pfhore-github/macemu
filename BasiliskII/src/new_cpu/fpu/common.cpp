#include "68881.hpp"
#include <fenv.h>
#include <limits.h>
long double read_ld(uint32_t addr) {
	long double ld;
	uint16_t exp = read_w(addr);
	uint64_t frac = ((uint64_t)read_l(addr+4) << 32) |
		((uint64_t)read_l(addr+8));
	memcpy(&ld, &frac, 8);
	memcpy((char*)&ld + 8, &exp, 2);
	return ld;
}
long double M68040_FPU::load_fd(bool rm, int t, int mode, int reg) {
	if( rm ) {
		switch(t) {
		case 0 : 
			cpu.parse_EA( mode, 4, reg, false);
			return (long double)cpu.ea_read();
		case 1 : {
			cpu.parse_EA( mode, 4, reg, false);
			float f = bit_cast<float, uint32_t>(cpu.ea_read());
			if( fpclassify(f) == FP_SUBNORMAL ) {
				stag = 5;
			}
			return f;
		}				
		case 2 :
			cpu.parse_EA( mode, 12, reg, false);
			return read_ld( cpu.ea_v );
		case 3 :	
#ifndef NO_PACKED_DECIMAL			
		{
			cpu.parse_EA( mode, 12, reg, false);
			uint32_t addr = cpu.ea_v;
			uint8_t v[12];
			for(int i = 0; i < 12; ++i ) {
				v[i] = read_b(addr+i);
			}
			return load_packed_decimal(v);
		}
#else
		cpu.exception(EX_FP_UNIMPLMENTED_TYPE_PRE);
#endif
		case 4 : 
			cpu.parse_EA( mode, 2, reg, false);
			return (long double)cpu.ea_read();
		case 5 : {
			cpu.parse_EA( mode, 8, reg, false);
			uint32_t addr = cpu.ea_v;
			double f;
			uint64_t lv = (uint64_t)read_l(addr) << 32 |
				read_l(addr+4);
			memcpy(&f, &lv, 8);
			if( fpclassify(f) == FP_SUBNORMAL ) {
				stag = 5;
			}
			return f;

			return f;
		}
		case 6 :
			cpu.parse_EA( mode, 1, reg, false);
			return (long double)cpu.ea_read();
		case 7 :
			throw Exception::IllegalInst();
		}
	} else {
		return FP[t];
	}
	return 0.0;
}
void write_ld(uint32_t addr, long double ld) {
	uint16_t exp;
	uint64_t frac;
	memcpy(&frac, &ld, 8);
	memcpy(&exp, (char*)&ld + 8, 2);
	write_w(addr, exp);
	write_l(addr+4, frac >> 32);
	write_l(addr+8, (uint32_t)frac);
}
void M68040_FPU::store_fd(int t, int mode, int reg, long double ld, int k) {
	if( issignaling(ld) ) {
		FPSR_ex |= EX_SNAN;
	}
	switch(t) {
	case 0 : {
		cpu.parse_EA( mode, 4, reg, false);
		int32_t v = ld;
		if( isnan(ld) ) {
			FPSR_ex |= EX_OPERR;
			v = (int32_t)getpayloadl(&ld);
		} else if ( (long)ld > INT_MAX ) {
			FPSR_ex |= EX_OPERR;
			v = INT_MAX;
		} else if( (long)ld < INT_MIN ) {
			FPSR_ex |= EX_OPERR;
			v = INT_MIN;
		}
		cpu.ea_write(v);
		return;
	}
	case 1 : 
		cpu.parse_EA( mode, 4, reg, false);
		cpu.ea_write( bit_cast<int32_t, float>(ld));
		return;
	case 2 : 
		cpu.parse_EA( mode, 12, reg, false);
		write_ld( cpu.ea_v, ld);
		return;
	case 3 :
	case 7 :
#ifndef NO_PACKED_DECIMAL			
	{
		cpu.parse_EA( mode, 12, reg, false);
		uint32_t addr = cpu.ea_v;
		uint8_t v[12];
		store_packed_decimal(ld, v, k);
		for(int i = 0; i < 12; ++i ) {
			write_b(addr+i, v[i]);
		}
		return;
	}
#else
	cpu.in_fpupost = true;
	cpu.exception(EX_FP_UNIMPLMENTED_TYPE_POST);
#endif
	case 4 : {
		cpu.parse_EA( mode, 2, reg, false);
		int16_t v = ld;
		if( isnan(ld) ) {
			FPSR_ex |= EX_OPERR;
			v = (int16_t)getpayloadl(&ld);
		} else if( (long)ld > SHRT_MAX ) {
			FPSR_ex |= EX_OPERR;
			v = SHRT_MAX;
		} else if( (long)ld < SHRT_MIN ) {
			FPSR_ex |= EX_OPERR;
			v = SHRT_MIN;
		}
		cpu.ea_write(v);
		return;
	}
	case 5 : {
		cpu.parse_EA( mode, 8, reg, false);
		uint32_t addr = cpu.ea_v;
		uint64_t lv = bit_cast<uint64_t, double>(ld);
		write_l(addr, lv >> 32);
		write_l(addr+4, (uint32_t)lv);
		return;
	}
	case 6 : {
		cpu.parse_EA( mode, 1, reg, false);
		int8_t v = ld;
		if( isnan(ld) ) {
			FPSR_ex |= EX_OPERR;
			v = (int8_t)getpayloadl(&ld);
		} else if( (long)ld > SCHAR_MAX ) {
			FPSR_ex |= EX_OPERR;
			v = SCHAR_MAX;
		} else if( (long)ld < SCHAR_MIN ) {
			FPSR_ex |= EX_OPERR;
			v = SCHAR_MIN;
		}
		cpu.ea_write(v);
		return;
	}
	}
}


bool M68040_FPU::cond(int op) {
	bool nan = FPSR_cond & 1;
	if( op >= 0x10 && nan ) {
		FPSR_ex |= EX_BSUN;
		FPSR_aex |= EX_ACC_IOP;
		if( FPCR & 1 << 15 ) {
			FPIAR = cpu.NPC;
			throw Exception::FP_BSUN();
		}
	}
	bool N = FPSR_cond >> 3 & 1;
	bool Z = FPSR_cond >> 2 & 1;
	op &= 0x0f;
	if( op > 7 ) {
		return ! cond(15-op);
	}
	switch(op) {
	case 0 :
		// F/T
		return false;
	case 1 :
		// EQ/NE
		return Z;
	case 2 :
		// (N)GT
		return ! nan & ! Z & ! N;
	case 3 :
		// (N)GE
		return Z | ( ! nan & ! N);
	case 4 :
		// (N)LT
		return ! nan & N & ! Z;
	case 5 :
		// (N)LE
		return Z | ( ! nan & N );
	case 6 :
		// (N)GL
		return ! nan & ! Z;
	case 7 :
		// (N)GLE
		return ! nan;
	}	
	return false;
}

void M68040_FPU::test_fp(long double l) {
	if( fetestexcept(FE_INVALID) ) {
		e1 = true;
		FPSR_ex |= EX_OPERR;
	}
	if( fetestexcept(FE_OVERFLOW) ) {
		FPSR_ex |= EX_OVFL;
	}	
	
	if( fetestexcept(FE_UNDERFLOW) ) {
		FPSR_ex |= EX_UNFL;
	}		
	if( fetestexcept(FE_DIVBYZERO) ) {
		FPSR_ex |= EX_DZ;
	}
	if( fetestexcept(FE_INEXACT) ) {
		FPSR_ex |= EX_INEX2;
	}
	int p = fpclassify(l);
	FPSR_cond =
		!!signbit(l) << 3 |
		(p == FP_ZERO) << 2 |
		(p == FP_INFINITE) << 1 |
		(p == FP_NAN);
	
	if( FPSR_ex & EX_OPERR )
		FPSR_aex |= EX_ACC_IOP;
	if( FPSR_ex & EX_OVFL )
		FPSR_aex |= EX_ACC_OVFL;
	if( (FPSR_ex & EX_UNFL) && (FPSR_ex & EX_INEX2) ) 
		FPSR_aex |= EX_ACC_UNFL;
	if( FPSR_ex & EX_DZ )
		FPSR_aex |= EX_ACC_DZ;
	if( FPSR_ex & (EX_INEX1 | EX_INEX2 | EX_OVFL) )
		FPSR_aex |= EX_ACC_INEX;
	
	uint8_t ex_v = FPSR_ex & (FPCR >> 8);
	if( ex_v ) {
		FPIAR = cpu.NPC;
		if( ex_v & EX_SNAN ) {
			throw Exception::FP_SNAN();
		}
		if( ex_v & EX_BSUN ) {
			throw Exception::FP_BSUN();
		}
		if( ex_v & EX_OPERR ) {
			throw Exception::FP_OPERR();
		}
		if( ex_v & EX_OVFL ) {
			throw Exception::FP_OVERFLOW();
		}
		if( ex_v & EX_UNFL ) {
			throw Exception::FP_UNDERFLOW();
		}
		if( ex_v & EX_DZ ) {
			throw Exception::FP_DIV_BY_0();
		}
		if( ex_v & ( EX_INEX1 | EX_INEX2 ) )  {
			throw Exception::FP_INEXACT();
		}
	}

}
