#include <stdint.h>
#include "sysdeps.h"
#include "main.h"
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "mac_via.h"
#include <unordered_map>
#include <functional>
#include "via.hpp"
#include "machine.hpp"
#include "adb.h"
#include "prefs.h"
MB_TYPE mb_type;
// $3166(V8); LC/IIvx/IIvi
bool check_V8(const ROM_DATA<MB_MEM, ROM_LOCAL>& mb) {
	return mb_type == MB_TYPE::V8;
}

// $31F2(Niagara)
bool check_niagara(const ROM_DATA<MB_MEM, ROM_LOCAL>& mb) {
	return mb_type == MB_TYPE::NIAGARA;
}

//$31AE(jaws); PowerBook
bool check_jaws(const ROM_DATA<MB_MEM, ROM_LOCAL>& mb) {
	return mb_type == MB_TYPE::JAWS;
}

// $31A0(MCU); Quadra
bool check_mcu(const ROM_DATA<MB_MEM, ROM_LOCAL>& mb) {
	return mb_type == MB_TYPE::MCU;
}

// $30AC(GLU); II/IIx/IIcx/SE30; VIA1 is mirrored at $20000
bool check_glu(const ROM_DATA<MB_MEM, ROM_LOCAL>& mb) {
	return mb_type == MB_TYPE::GLU;
}

// $30CA(MDU); IIci/IIsi
bool check_mdu(const ROM_DATA<MB_MEM, ROM_LOCAL>& mb) {
	return mb_type == MB_TYPE::MDU;
}

// $310A(OSS); IIfx
bool check_oss(const ROM_DATA<MB_MEM, ROM_LOCAL>& mb) {
	return mb_type == MB_TYPE::OSS;
}
