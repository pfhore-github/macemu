#pragma once
#include "via.hpp"
namespace RBV_REG {
enum RBV_REG_t {
	RB,
	EXP,
	SIFR,
	IFR,
	MONP,
	CHPT,
	SIER,
	IER,
	RA = SIFR,
	RBV_4 = 8,
	RBV_5 = 9,
	MSC_RBV_X16 = MONP, // powerbook reuse this flag?
	PB_RBV_X33 = 9,
};
}
class RBV : public Via_Lite {
	uint8_t sIER;
	uint8_t monP;
public:
	void reset() { Via_Lite::reset(); sIER = monP = 0; }
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
	void set_monp(uint8_t v);
};
class SonorRBV : public RBV {
public:
//	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
// TODO
class VDAC : public IO_BASE {
public:
	uint8_t read(int addr) override { return 0; }
	void write(int addr, uint8_t v) override  {}
};

constexpr int RBV_DEPTH = 0x07; /* bits per pixel: 000=1,001=2,010=4,011=8 */
constexpr int RBV_MONID = 0x38; /* monitor type, as below. */
constexpr int RBV_VIDOFF = 0x40; /* 1 turns off onboard video */
/* Supported monitor types: */
constexpr int MON_15BW = (1<<3); /* 15" BW portrait. */
constexpr int MON_IIGS = (2<<3); /* 12" color (modified IIGS monitor). */
constexpr int MON_15RGB = (5<<3); /* 15" RGB portrait. */
constexpr int MON_12OR13 = (6<<3); /* 12" BW or 13" RGB. */
constexpr int MON_NONE =(7<<3); /* No monitor attached. */
