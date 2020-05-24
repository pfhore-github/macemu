/*
 *  adb.cpp - ADB emulation (mouse/keyboard)
 *
 *  Basilisk II (C) Christian Bauer
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

/*
 *  SEE ALSO
 *    Inside Macintosh: Devices, chapter 5 "ADB Manager"
 *    Technote HW 01: "ADB - The Untold Story: Space Aliens Ate My Mouse"
 */

#include <stdlib.h>
#include <atomic>
#include "sysdeps.h"
#include "cpu_emulation.h"
#include "emul_op.h"
#include "main.h"
#include "prefs.h"
#include "video.h"
#include "adb.h"
#include <SDL_events.h>
#include <SDL_thread.h>
#include <SDL_timer.h>
#include <SDL_mouse.h>
#include <SDL_keyboard.h>
#include <unordered_map>
#include <deque>
#include "via.hpp"
#ifdef POWERPC_ROM
#include "thunks.h"
#endif
#include "machine.hpp"
#define DEBUG 0
#include "debug.h"
#include <queue>
void ADB_Bus::send_to(uint8_t c) {
	switch( listen_state ) {
	case WAIT :
		// do nothing
		break;
	case DATA_SIZE:
		listen_data.push_back(c);
		listen_left = c;
		listen_state = DATA;
		break;
	case DATA:
		listen_data.push_back(c);
		if( --listen_left == 0 ) {
			listen_state = WAIT;
			devices[listen_dev]->listen(listen_cmd & 3, listen_data );
			listen_data.clear();
			listen_cmd = 0;
		}
		break;
	}
}
			
void ADB_Bus::cmd(uint8_t c) {
	int target = c >> 4;
	switch( c & 0xf ) {
	case 0 :
		for( auto& dev : devices ) {
			if( dev )
				dev->reset();
		}
		break;
	case 1 :
		if( devices[target] ) {
			devices[target]->flush();
		}
		break;
	case 8 :
	case 9 :
	case 10 :
	case 11 :  		
		// LISTEN 
		if( devices[target] ) {
			listen_cmd = c & 3;
			listen_dev = target;
			state = DATA_SIZE;
		}
		break;
	case 12 :
	case 13 :
	case 14 :
	case 15 : 
		// TALK
		adb_talk(c);
		break;
	}
}
void ADB_device::talk(int r) {
	lock();
	std::vector<uint8_t> data;
	switch(r) {
	case 0:
		data = talk0();
		break;
	case 1:
		data = talk1();
		break;
	case 2:
		data = talk2();
		break;
	case 3 :
		data = { 2, uint8_t( (reg[3] >> 8 & 0xf0) | (rand() & 0xf) ),
				 uint8_t( reg[3] ) };
		break;
	}
	unlock();
	for( uint8_t d : data ) {
		bus->talk_data.push( d );
	}
}


void ADB_device::listen(int r, const std::vector<uint8_t>& data) {
	lock();
	switch(r) {
	case 0 :
		listen0(data);
		break;
	case 1 :
		listen1(data);
		break;
	case 2 :
		listen2(data);
		break;		
	case 3: {
		uint8_t high = data[0];
		uint8_t low = data[1];
		if( low == 0xfe ) {
			int i = high & 0x0f;
			reg[3] = (reg[3] & 0xf0ff) | (i << 8);
			change(i);
		} else if( low == 1 || low == 2 || low == 4 ) {
			reg[3] = (reg[3] & 0xff00) | low;
		} else if( low == 0 ) {
			reg[3] = (reg[3] & 0xd0ff) | ((high & 0x2f) << 8);
			change(high & 0x0f);
		}
	}
	}
	unlock();
}
void ADB_device::change(int i) {
	bus->devices[i] = this;
	bus->devices[c] = nullptr;
	c = i;
}
class ADB_Mouse : public ADB_device {
	union reg0_t {
		uint16_t v;
		struct {
			int8_t xv: 7;
			bool : 1;
			int8_t yv: 7;
			bool btn : 1;
		};
	};
public:	
	ADB_Mouse() { c = 3; }
	void handle_event(SDL_Event* e) {
		lock();
		switch(e->type) {
		case SDL_MOUSEMOTION : {
			reg0_t t = { reg[0] };
			t.xv = t.xv + e->motion.xrel;
			t.yv = t.yv + e->motion.yrel;
			reg[0] = t.v;
			break;
		}
		case SDL_MOUSEBUTTONDOWN :
			// STUB multi button support
			reg[0] &= 0x7fff;
			break;
		case SDL_MOUSEBUTTONUP :
			// STUB multi button support
			reg[0] |= 0x8000;
			break;
		}
		unlock();
	}
	void reset() {
		lock();
		reg[0] = 0x8080;
		reg[3] = 0x6301;
		unlock();
	}
	// 300dpi
	std::vector<uint8_t> talk1() override {
		return { 9,
				 // Identifier				 
				 'a', 'p', 'p', 'l',
				 // Resolution(dpi)
				 uint8_t(300 >> 8), uint8_t(300 & 0xff),
				 1, 3
		};
	}
};

class ADB_keyboard : public ADB_device {
	std::deque<uint8_t> keys;
public:
	static std::unordered_map<SDL_Scancode, uint8_t> keymaps;
	ADB_keyboard() { c = 2; }
	void reset() {
		lock();
		keys.clear();
		reg[0] = 0xffff;
		reg[2] = 0xffff;
		reg[3] = 0x62 << 8 | 
			(uint8)PrefsFindInt32("keyboardtype");
		unlock();
	}
	void handle_event(SDL_Event* e) {
		lock();
		switch(e->type) {
		case SDL_QUIT:
			// power key alternative
			reg[0] = 0x7f7f;
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			uint8_t v = (e->key.state == SDL_KEYUP) << 7;
			auto key = keymaps.find( e->key.keysym.scancode );
			if( key != keymaps.end() ) {
				v |= key->second;
			}
			reg[2] = 0xff;
			if( (v & 0x7f) == 0x75 ) {
				// Delete
				v &= ~ 0x4000;
			}
			if( e->key.keysym.mod & KMOD_CAPS ) {
				v &= ~ 0x2000;
			}
			if( e->key.keysym.mod & KMOD_CTRL ) {
				v &= ~ 0x800;
			}
			if( e->key.keysym.mod & KMOD_SHIFT ) {
				v &= ~ 0x400;
			}
			if( e->key.keysym.mod & KMOD_ALT ) {
				v &= ~ 0x200;
			}
			if( e->key.keysym.mod & KMOD_GUI ) {
				v &= ~ 0x100;
			}
			if( e->key.keysym.mod & KMOD_NUM ) {
				v &= ~ 0x80;
			}
			if( (v & 0x7f) == 0x6b ) {
				// Scroll lock
				v &= ~ 0x4000;
			}
			keys.push_back(v);
			break;
		}
		}
		unlock();
	}
	std::vector<uint8_t> talk0() {
		uint8_t ks[2];
		for(int i = 0; i < 2; ++i ) {
			if( ! keys.empty() ) {
				ks[i] = keys.front();
				keys.pop_front();
			} else {
				ks[i] = 0xff;
			}
		}
		return { ks[0], ks[1] };
	}
};
std::unordered_map<SDL_Scancode, uint8_t> ADB_keyboard::keymaps;
/*
 *  Initialize ADB emulation
 */
ADB_keyboard* kbd;
ADB_Mouse* mouse;
void ADBInit(void)
{

	machine->adb_bus->devices[2] = kbd = new ADB_keyboard();	 
	machine->adb_bus->devices[3] = mouse = new ADB_Mouse();

	ADB_keyboard::keymaps = {
		{ SDL_SCANCODE_ESCAPE,           0x35 },
		{ SDL_SCANCODE_F1,               0x7A },
		{ SDL_SCANCODE_F2,               0x78 },
		{ SDL_SCANCODE_F3,               0x63 },
		{ SDL_SCANCODE_F4,               0x76 },
		{ SDL_SCANCODE_F5,               0x60 },
		{ SDL_SCANCODE_F6,               0x61 },
		{ SDL_SCANCODE_F7,               0x62 },
		{ SDL_SCANCODE_F8,               0x64 },
		{ SDL_SCANCODE_F9,               0x65 },
		{ SDL_SCANCODE_F10,              0x6D },
		{ SDL_SCANCODE_F11,              0x67 },
		{ SDL_SCANCODE_F12,              0x6F },
		{ SDL_SCANCODE_PRINTSCREEN,      0x69 },
		{ SDL_SCANCODE_SCROLLLOCK,       0x6B },
		{ SDL_SCANCODE_PAUSE,            0x71 },
		{ SDL_SCANCODE_GRAVE,            0x32 },
		{ SDL_SCANCODE_1,                0x12 },
		{ SDL_SCANCODE_2,                0x13 },
		{ SDL_SCANCODE_3,                0x14 },
		{ SDL_SCANCODE_4,                0x15 },
		{ SDL_SCANCODE_5,                0x17 },
		{ SDL_SCANCODE_6,                0x16 },
		{ SDL_SCANCODE_7,                0x1A },
		{ SDL_SCANCODE_8,                0x1C },
		{ SDL_SCANCODE_9,                0x19 },
		{ SDL_SCANCODE_0,                0x1D },
		{ SDL_SCANCODE_MINUS,            0x1B },
		{ SDL_SCANCODE_EQUALS,           0x18 },
		{ SDL_SCANCODE_BACKSPACE,        0x33 },
		{ SDL_SCANCODE_INSERT,           0x72 },
		{ SDL_SCANCODE_INSERT,           0x72 },
		{ SDL_SCANCODE_HOME,             0x73 },
		{ SDL_SCANCODE_PAGEUP,           0x74 },
		{ SDL_SCANCODE_NUMLOCKCLEAR,     0x47 },
		// No KP = key for PC keyboard...		
		{ SDL_SCANCODE_KP_DIVIDE,        0x4B },
		{ SDL_SCANCODE_KP_MULTIPLY,      0x43 },
		{ SDL_SCANCODE_TAB,              0x30 }, 
		{ SDL_SCANCODE_Q,                0x0C }, 
		{ SDL_SCANCODE_W,                0x0D }, 
		{ SDL_SCANCODE_E,                0x0E }, 
		{ SDL_SCANCODE_R,                0x0F }, 
		{ SDL_SCANCODE_T,                0x11 }, 
		{ SDL_SCANCODE_Y,                0x10 }, 
		{ SDL_SCANCODE_U,                0x20 }, 
		{ SDL_SCANCODE_I,                0x22 }, 
		{ SDL_SCANCODE_O,                0x1F }, 
		{ SDL_SCANCODE_P,                0x23 }, 
		{ SDL_SCANCODE_LEFTBRACKET,      0x21 }, 
		{ SDL_SCANCODE_RIGHTBRACKET,     0x1E }, 
		{ SDL_SCANCODE_BACKSLASH,        0x2A },
		{ SDL_SCANCODE_DELETE,           0x75 },
		{ SDL_SCANCODE_END,              0x77 },
		{ SDL_SCANCODE_PAGEDOWN,         0x79 },
		{ SDL_SCANCODE_KP_7,             0x59 },
		{ SDL_SCANCODE_KP_8,             0x5B },
		{ SDL_SCANCODE_KP_9,             0x5C },
		{ SDL_SCANCODE_KP_MINUS,         0x4E },
		{ SDL_SCANCODE_CAPSLOCK,         0x39 },
		{ SDL_SCANCODE_A,                0x00 },
		{ SDL_SCANCODE_S,                0x01 },
		{ SDL_SCANCODE_D,                0x02 },
		{ SDL_SCANCODE_F,                0x03 },
		{ SDL_SCANCODE_G,                0x05 },
		{ SDL_SCANCODE_H,                0x04 },
		{ SDL_SCANCODE_J,                0x26 },
		{ SDL_SCANCODE_K,                0x28 },
		{ SDL_SCANCODE_L,                0x25 },
		{ SDL_SCANCODE_SEMICOLON,        0x29 },
		{ SDL_SCANCODE_APOSTROPHE,       0x27 },
		{ SDL_SCANCODE_RETURN,           0x24 }, // MAC RETURN
		{ SDL_SCANCODE_KP_4,             0x56 },
		{ SDL_SCANCODE_KP_5,             0x57 },
		{ SDL_SCANCODE_KP_6,             0x58 },
		{ SDL_SCANCODE_KP_PLUS,          0x45 },
		{ SDL_SCANCODE_LSHIFT,           0x38 },
		{ SDL_SCANCODE_Z,                0x06 },
		{ SDL_SCANCODE_X,                0x07 },
		{ SDL_SCANCODE_C,                0x08 },
		{ SDL_SCANCODE_V,                0x09 },
		{ SDL_SCANCODE_B,                0x0B },
		{ SDL_SCANCODE_N,                0x2D },
		{ SDL_SCANCODE_M,                0x2E },
		{ SDL_SCANCODE_COMMA,            0x2B },
		{ SDL_SCANCODE_PERIOD,           0x2F },
		{ SDL_SCANCODE_SLASH,            0x2C },
		{ SDL_SCANCODE_RSHIFT,           0x7B },
		{ SDL_SCANCODE_UP,               0x3E }, // MAC RETURN
		{ SDL_SCANCODE_KP_1,             0x53 },
		{ SDL_SCANCODE_KP_2,             0x54 },
		{ SDL_SCANCODE_KP_3,             0x55 },
		{ SDL_SCANCODE_KP_ENTER,         0x4C },
		{ SDL_SCANCODE_LCTRL,            0x36 },
		{ SDL_SCANCODE_LALT,             0x3A },
		{ SDL_SCANCODE_LGUI,             0x37 },
		{ SDL_SCANCODE_SPACE,            0x31 },
		{ SDL_SCANCODE_RALT,             0x7C },
		{ SDL_SCANCODE_RCTRL,            0x7D },
		{ SDL_SCANCODE_LEFT,             0x3B }, 
		{ SDL_SCANCODE_DOWN,             0x3D }, 
		{ SDL_SCANCODE_RIGHT,            0x3C }, 
		{ SDL_SCANCODE_KP_0,             0x52 },
		{ SDL_SCANCODE_KP_PERIOD,        0x41 },
	};
}

void handle_adb(SDL_Event* e) {
	switch(e->type) {
	case SDL_MOUSEMOTION :
	case SDL_MOUSEBUTTONDOWN :
	case SDL_MOUSEBUTTONUP :
		mouse->handle_event(e);
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		kbd->handle_event(e);
		break;
	}
}


/*
 *  Exit ADB emulation
 */

void ADBExit(void)
{
	delete machine->adb_bus->devices[2];
	delete machine->adb_bus->devices[3];
	machine->adb_bus->devices[2] = kbd = nullptr;
	machine->adb_bus->devices[3] = mouse = nullptr;
	
}


/*
 *  ADBOp() replacement
 */

void ADBOp(uint8 op, uint8 *data)
{
#if 0
	D(bug("ADBOp op %02x, data %02x %02x %02x\n", op, data[0], data[1], data[2]));

	// ADB reset?
	if ((op & 0x0f) == 0) {
		mouse_reg_3[0] = 0x63;
		mouse_reg_3[1] = 0x01;
		key_reg_2[0] = 0xff;
		key_reg_2[1] = 0xff;
		key_reg_3[0] = 0x62;
		key_reg_3[1] = m_keyboard_type;
		return;
	}

	// Cut op into fields
	uint8 adr = op >> 4;
	uint8 cmd = (op >> 2) & 3;
	uint8 reg = op & 3;

	// Check which device was addressed and act accordingly
	if (adr == (mouse_reg_3[0] & 0x0f)) {

		// Mouse
		if (cmd == 2) {

			// Listen
			switch (reg) {
				case 3:		// Address/HandlerID
					if (data[2] == 0xfe)			// Change address
						mouse_reg_3[0] = (mouse_reg_3[0] & 0xf0) | (data[1] & 0x0f);
					else if (data[2] == 1 || data[2] == 2 || data[2] == 4)	// Change device handler ID
						mouse_reg_3[1] = data[2];
					else if (data[2] == 0x00)		// Change address and enable bit
						mouse_reg_3[0] = (mouse_reg_3[0] & 0xd0) | (data[1] & 0x2f);
					break;
			}

		} else if (cmd == 3) {

			// Talk
			switch (reg) {
				case 1:		// Extended mouse protocol
					data[0] = 8;
					data[1] = 'a';				// Identifier
					data[2] = 'p';
					data[3] = 'p';
					data[4] = 'l';
					data[5] = 300 >> 8;			// Resolution (dpi)
					data[6] = 300 & 0xff;
					data[7] = 1;				// Class (mouse)
					data[8] = 3;				// Number of buttons
					break;
				case 3:		// Address/HandlerID
					data[0] = 2;
					data[1] = (mouse_reg_3[0] & 0xf0) | (rand() & 0x0f);
					data[2] = mouse_reg_3[1];
					break;
				default:
					data[0] = 0;
					break;
			}
		}
		D(bug(" mouse reg 3 %02x%02x\n", mouse_reg_3[0], mouse_reg_3[1]));

	} else if (adr == (key_reg_3[0] & 0x0f)) {

		// Keyboard
		if (cmd == 2) {

			// Listen
			switch (reg) {
				case 2:		// LEDs/Modifiers
					key_reg_2[0] = data[1];
					key_reg_2[1] = data[2];
					break;
				case 3:		// Address/HandlerID
					if (data[2] == 0xfe)			// Change address
							key_reg_3[0] = (key_reg_3[0] & 0xf0) | (data[1] & 0x0f);
					else if (data[2] == 0x00)		// Change address and enable bit
						key_reg_3[0] = (key_reg_3[0] & 0xd0) | (data[1] & 0x2f);
					break;
			}

		} else if (cmd == 3) {

			// Talk
			switch (reg) {
				case 2: {	// LEDs/Modifiers
					uint8 reg2hi = 0xff;
					uint8 reg2lo = key_reg_2[1] | 0xf8;
					if (MATRIX(0x6b))	// Scroll Lock
						reg2lo &= ~0x40;
					if (MATRIX(0x47))	// Num Lock
						reg2lo &= ~0x80;
					if (MATRIX(0x37))	// Command
						reg2hi &= ~0x01;
					if (MATRIX(0x3a))	// Option
						reg2hi &= ~0x02;
					if (MATRIX(0x38))	// Shift
						reg2hi &= ~0x04;
					if (MATRIX(0x36))	// Control
						reg2hi &= ~0x08;
					if (MATRIX(0x39))	// Caps Lock
						reg2hi &= ~0x20;
					if (MATRIX(0x75))	// Delete
						reg2hi &= ~0x40;
					data[0] = 2;
					data[1] = reg2hi;
					data[2] = reg2lo;
					break;
				}
				case 3:		// Address/HandlerID
					data[0] = 2;
					data[1] = (key_reg_3[0] & 0xf0) | (rand() & 0x0f);
					data[2] = key_reg_3[1];
					break;
				default:
					data[0] = 0;
					break;
			}
		}
		D(bug(" keyboard reg 3 %02x%02x\n", key_reg_3[0], key_reg_3[1]));

	} else												// Unknown address
		if (cmd == 3)
			data[0] = 0;								// Talk: 0 bytes of data
#endif
}


/*
 *  Mouse was moved (x/y are absolute or relative, depending on ADBSetRelMouseMode())
 */

void ADBMouseMoved(int x, int y)
{
}


/* 
 *  Mouse button pressed
 */

void ADBMouseDown(int button)
{
}


/*
 *  Mouse button released
 */

void ADBMouseUp(int button)
{
}


/*
 *  Set mouse mode (absolute or relative)
 */

void ADBSetRelMouseMode(bool relative)
{
}


/*
 *  Key pressed ("code" is the Mac key code)
 */

void ADBKeyDown(int code)
{
}


/*
 *  Key released ("code" is the Mac key code)
 */

void ADBKeyUp(int code)
{
}


/*
 *  ADB interrupt function (executed as part of 60Hz interrupt)
 */

void ADBInterrupt(void)
{
#if 0

	if (relative_mouse) {

		// Mouse movement (relative) and buttons
		if (mx != 0 || my != 0 || mb[0] != old_mouse_button[0] || mb[1] != old_mouse_button[1] || mb[2] != old_mouse_button[2]) {

			// Call mouse ADB handler
			if (mouse_reg_3[1] == 4) {
				// Extended mouse protocol
				WriteMacInt8(tmp_data, 3);
				WriteMacInt8(tmp_data + 1, (my & 0x7f) | (mb[0] ? 0 : 0x80));
				WriteMacInt8(tmp_data + 2, (mx & 0x7f) | (mb[1] ? 0 : 0x80));
				WriteMacInt8(tmp_data + 3, ((my >> 3) & 0x70) | ((mx >> 7) & 0x07) | (mb[2] ? 0x08 : 0x88));
			} else {
				// 100/200 dpi mode
				WriteMacInt8(tmp_data, 2);
				WriteMacInt8(tmp_data + 1, (my & 0x7f) | (mb[0] ? 0 : 0x80));
				WriteMacInt8(tmp_data + 2, (mx & 0x7f) | (mb[1] ? 0 : 0x80));
			}	
			r.a[0] = tmp_data;
			r.a[1] = ReadMacInt32(mouse_base);
			r.a[2] = ReadMacInt32(mouse_base + 4);
			r.a[3] = adb_base;
			r.d[0] = (mouse_reg_3[0] << 4) | 0x0c;	// Talk 0
			Execute68k(r.a[1], &r);

			old_mouse_button[0] = mb[0];
			old_mouse_button[1] = mb[1];
			old_mouse_button[2] = mb[2];
		}

	} else {

		// Update mouse position (absolute)
		if (mx != old_mouse_x || my != old_mouse_y) {
#ifdef POWERPC_ROM
			static const uint8 proc_template[] = {
				0x2f, 0x08,		// move.l a0,-(sp)
				0x2f, 0x00,		// move.l d0,-(sp)
				0x2f, 0x01,		// move.l d1,-(sp)
				0x70, 0x01,		// moveq #1,d0 (MoveTo)
				0xaa, 0xdb,		// CursorDeviceDispatch
				M68K_RTS >> 8, M68K_RTS & 0xff
			};
			BUILD_SHEEPSHAVER_PROCEDURE(proc);
			r.a[0] = ReadMacInt32(mouse_base + 4);
			r.d[0] = mx;
			r.d[1] = my;
			Execute68k(proc, &r);
#else
			WriteMacInt16(0x82a, mx);
			WriteMacInt16(0x828, my);
			WriteMacInt16(0x82e, mx);
			WriteMacInt16(0x82c, my);
			WriteMacInt8(0x8ce, ReadMacInt8(0x8cf));	// CrsrCouple -> CrsrNew
#endif
			old_mouse_x = mx;
			old_mouse_y = my;
		}

		// Send mouse button events
		if (mb[0] != old_mouse_button[0] || mb[1] != old_mouse_button[1] || mb[2] != old_mouse_button[2]) {
			uint32 mouse_base = adb_base + 16;

			// Call mouse ADB handler
			if (mouse_reg_3[1] == 4) {
				// Extended mouse protocol
				WriteMacInt8(tmp_data, 3);
				WriteMacInt8(tmp_data + 1, mb[0] ? 0 : 0x80);
				WriteMacInt8(tmp_data + 2, mb[1] ? 0 : 0x80);
				WriteMacInt8(tmp_data + 3, mb[2] ? 0x08 : 0x88);
			} else {
				// 100/200 dpi mode
				WriteMacInt8(tmp_data, 2);
				WriteMacInt8(tmp_data + 1, mb[0] ? 0 : 0x80);
				WriteMacInt8(tmp_data + 2, mb[1] ? 0 : 0x80);
			}
			r.a[0] = tmp_data;
			r.a[1] = ReadMacInt32(mouse_base);
			r.a[2] = ReadMacInt32(mouse_base + 4);
			r.a[3] = adb_base;
			r.d[0] = (mouse_reg_3[0] << 4) | 0x0c;	// Talk 0
			Execute68k(r.a[1], &r);

			old_mouse_button[0] = mb[0];
			old_mouse_button[1] = mb[1];
			old_mouse_button[2] = mb[2];
		}
	}

	// Process accumulated keyboard events
	while (key_read_ptr != key_write_ptr) {

		// Read keyboard event
		uint8 mac_code = key_buffer[key_read_ptr];
		key_read_ptr = (key_read_ptr + 1) % KEY_BUFFER_SIZE;

		// Call keyboard ADB handler
		WriteMacInt8(tmp_data, 2);
		WriteMacInt8(tmp_data + 1, mac_code);
		WriteMacInt8(tmp_data + 2, mac_code == 0x7f ? 0x7f : 0xff);	// Power key is special
		r.a[0] = tmp_data;
		r.a[1] = ReadMacInt32(key_base);
		r.a[2] = ReadMacInt32(key_base + 4);
		r.a[3] = adb_base;
		r.d[0] = (key_reg_3[0] << 4) | 0x0c;	// Talk 0
		Execute68k(r.a[1], &r);
	}

	// Clear temporary data
	WriteMacInt32(tmp_data, 0);
	WriteMacInt32(tmp_data + 4, 0);
#endif
}
std::atomic<uint8_t> state = 0;
std::atomic<bool> adb_interrupt = false;


void adb_talk(uint8_t v) {
	int target = v >> 4;
	if( machine->adb_bus->devices[target] ) {
		machine->adb_bus->devices[target]->talk(v & 3);
	}
}

uint8_t adb_read() {
	return machine->adb_bus->get_from();
}

void adb_write(uint8_t v) {
	machine->adb_bus->send_to(v);
}

	
ADB_device* adb_devices[16];
