#include "../registers.hpp"
#include "../common.hpp"
#include "68881.hpp"
#include "SDL_endian.h"
// <SM SE Y Y> <EXP>[4] <XX>[2] <DIGIT>[17]
struct PDR  {
	unsigned int exp2 : 4;
	unsigned int yy : 2;
	bool SE : 1;
	bool SM : 1;
	unsigned int exp10 : 8;	
	int : 4;
	unsigned int exp3 : 4;
	unsigned int digit16 : 4;
};
#include <math.h>
void M68040_FPU::store_packed_decimal(long double from, uint8_t raw[], int k) {
	PDR ret;
	int cf = fpclassify(from);
	ret.SM = !!signbit(from);
	from = fabsl(from);
	switch( cf ) {
	case FP_INFINITE:
		ret.SE = true;
		ret.yy = 3;
		ret.exp2 = 0xf;
		ret.exp10 = 0xff;
		memcpy(raw, &ret, 4);
		memset(raw+4, 0, 8);
		return;
	case FP_NAN: {
		ret.SE = true;
		ret.yy = 3;
		ret.exp2 = 0xf;
		ret.exp10 = 0xff;
		memcpy(raw, &ret, 4);
		uint64_t payload;
		memcpy(&payload, &from, 8);
		payload = SDL_SwapBE64(payload);
		memcpy(raw+4, &payload, 8);
		return;
	}
	case FP_ZERO:
		ret.exp2 = 0;
		ret.exp10 = 0;
		memcpy(raw, &ret, 4);
		memset(raw+4, 0, 8);
		return;
	}
	int left = log10l(from);
	if( k <= 0 ) {
		k = left - k+1;
	} else if( k > 17 ) {
		k = 17;
		FPSR_ex |= EX_OPERR;
	}
	char tmp[24] = {};
	snprintf(tmp, 24, "%.*Le", k-1, from);
	ret.digit16 = tmp[0] & 0xf;
	ret.exp3 = 0;
	ret.yy = 0;
	int c = tmp[1] == '.' ? 2 : 1;
	// read mantissa
	uint8_t digits[8] = {};
	for(;tmp[c] != 'e';c++) {
		digits[(c-2)/2] |= (uint64_t)(tmp[c]&0xf) << ((c&1) ? 0 : 4); 
	}
	c++; // skip e
	ret.SE = tmp[c++] == '-'; // read sign of exp
	if(tmp[c+3] ) { // exp is 4 digit
		ret.exp3 = tmp[c] & 0xf;
		ret.exp2 = tmp[c+1] & 0xf;
		ret.exp10 = (tmp[c+2]&0xf) << 4 | (tmp[c+3] & 0xf);
	} else if( tmp[c+2] ) { // exp is 3 digit
		ret.exp2 = tmp[c] & 0xf;
		ret.exp10 = (tmp[c+1]&0xf) << 4 | (tmp[c+2] & 0xf);
	} else {					// exp is 2 digit
		ret.exp2 = 0;
		ret.exp10 = (tmp[c]&0xf) << 4 | (tmp[c+1] & 0xf);
	}
	memcpy(raw, &ret, 4);
	memcpy(raw+4, &digits, 8);
	return;
}


long double M68040_FPU::load_packed_decimal(const uint8_t raw[]) {
	PDR ret;
	uint64_t digits;
	memcpy(&ret, raw, 4);
	memcpy(&digits, raw + 4, 8);
	long double sg = ret.SM ? -1.0 : 1.0;
	if( ret.SE && ret.yy == 3 && ret.exp2 == 0xf && ret.exp10 == 0xff ) {
		if( digits == 0 ) {
			return copysign(INFINITY, sg);
		} else {
			long double dx = copysign(NAN, sg);
			digits = SDL_SwapBE64(digits);
			memcpy(&dx, &digits, 8);
			return dx;
		}
	}
	if( digits == 0 ) {
		return copysign(0.0, sg);
	}
	char tp[32] = {};
	tp[0] = '0' + (raw[3] & 0xf);
	tp[1] = '.';
	for(int i = 4; i < 12; ++i ) {
		tp[(i-4)*2+2] = '0' + ((raw[i] & 0xf0) >> 4);
		tp[(i-4)*2+3] = '0' + (raw[i] & 0xf);
	}
	long double rtv;
	sscanf(tp, "%Le", &rtv);
	// check INEX1
	if( rtv * 1e16 - floor(rtv * 1e16) ) {
		FPSR_ex |= EX_INEX1;
	}
	tp[18] = 'e';
	tp[19] = ret.SE ? '-' : '+';
	tp[20] = ret.exp3 + '0';
	tp[21] = ret.exp2 + '0';
	tp[22] = (ret.exp10 >> 4) + '0';
	tp[23] = (ret.exp10&0xf) + '0';
	sscanf(tp, "%Le", &rtv);
	return copysign(rtv, sg);
}

