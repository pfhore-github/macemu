/*
 *  main.h - General definitions
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MAIN_H
#define MAIN_H
#include <atomic>
// CPU type (0 = 68000, 1 = 68010, 2 = 68020, 3 = 68030, 4 = 68040/060)
extern int CPUType;

// FPU type (0 = no FPU, 1 = 68881, 2 = 68882)
extern int FPUType;

// Flag: 24-bit-addressing?
extern bool TwentyFourBitAddressing;

template<class T, class U>
inline T bit_cast(U v) {
	static_assert(sizeof(T) == sizeof(U));
	T r;
	memcpy(&r, &v, sizeof(U));
	return r;
}
// 68k register structure (for Execute68k())
struct M68kRegisters {
	uint32_t d[8];
	uint32_t a[8];
	uint16_t sr;
};

// General functions
extern bool InitAll(const char *vmdir);
extern void ExitAll(void);

// Platform-specific functions
extern void FlushCodeCache(void *start, uint32_t size);	// Code was patched, flush caches if neccessary
extern void QuitEmulator(void);							// Quit emulator
extern void ErrorAlert(const char *text);				// Display error alert
extern void ErrorAlert(int string_id);
extern void WarningAlert(const char *text);				// Display warning alert
extern void WarningAlert(int string_id);
extern bool ChoiceAlert(const char *text, const char *pos, const char *neg);	// Display choice alert


// Interrupt flags
enum {
	INTFLAG_60HZ = 1,	// 60.15Hz VBL
	INTFLAG_1HZ = 2,	// ~1Hz interrupt
	INTFLAG_SERIAL = 4,	// Serial driver
	INTFLAG_ETHER = 8,	// Ethernet driver
	INTFLAG_AUDIO = 16,	// Audio block read
	INTFLAG_TIMER = 32,	// Time Manager
	INTFLAG_ADB = 64,	// ADB
	INTFLAG_NMI = 128	// NMI
};

extern std::atomic<uint32_t> InterruptFlags;									// Currently pending interrupts
extern void SetInterruptFlag(uint32_t flag);						// Set/clear interrupt flags
extern void ClearInterruptFlag(uint32_t flag);

// vde switch variable
extern char* vde_sock;

// Array length
#if __cplusplus >= 201103L || (_MSC_VER >= 1900 && defined __cplusplus)
template <typename T, size_t size>
constexpr size_t lengthof(T (& a)[size])
{
	return size;
}
#else
#define lengthof(a) (sizeof(a) / sizeof(a[0]))
#endif

#endif
