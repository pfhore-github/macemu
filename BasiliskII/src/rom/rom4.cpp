#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "mac_via.h"
#include <cassert>
#include <stdio.h>
#include "prefs.h"
#include "via.hpp"
#include "rbv.hpp"
#include "iop.hpp"
#include "via1.hpp"
#include "ncr5380.hpp"
#include "machine.hpp"
#include "sonic.hpp"
#include "mcu.hpp"
#include "jaws.hpp"
#include "memcjr.hpp"
#include "oss.hpp"
#include <bitset>

namespace ROM {










void reset_via();








struct _B2A7C_t {
	uint16_t machine_gen;
	uint32_t v2;

	uint8_t* v3;
};
uint8_t  _AA9C0[] = {
	0x42, 0x4f, 0x52, 0x47, 0x7c, 0x30, 0xf9, 0x12,
	0x02, 0xcc, 0x81, 0x2f, 0xcc, 0xcc, 0xea, 0xcc,
	0xcc, 0x5e, 0xcc, 0x81, 0x17, 0xcc, 0x81, 0x19,
	0xcc, 0x86, 0x93
};

constexpr uint16_t MODEL_ID_IS_IN_VIA = 1 << 10;

}
