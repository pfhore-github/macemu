/* c++ */
/*
 *  adb.h - ADB emulation (mouse/keyboard)
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

#ifndef ADB_H
#define ADB_H
#include <memory>
#include <vector>
#include <queue>
#include "SDL_mutex.h"
extern void ADBInit(void);
extern void ADBExit(void);

void adb_talk(uint8_t v);
uint8_t adb_read();

extern void ADBOp(uint8_t op, uint8_t *data);

extern void ADBMouseMoved(int x, int y);
extern void ADBMouseDown(int button);
extern void ADBMouseUp(int button);

extern void ADBKeyDown(int code);
extern void ADBKeyUp(int code);

extern void ADBInterrupt(void);

extern void ADBSetRelMouseMode(bool relative);
class ADB_Bus;
class ADB_device {
	friend class ADB_Bus;
	SDL_mutex* mutex;
protected:
	ADB_device() :mutex(SDL_CreateMutex()) {}
	std::shared_ptr<ADB_Bus> bus;
	int c;
	uint16_t reg[3];
	void lock() { SDL_LockMutex(mutex); }
	void unlock() { SDL_UnlockMutex(mutex); }
	virtual std::vector<uint8_t> talk0() {
		return { 2, uint8_t(reg[0]>>8), uint8_t(reg[0]) };
	}
	virtual std::vector<uint8_t> talk1() {
		return { 2, uint8_t(reg[1]>>8), uint8_t(reg[1]) };
	}
	virtual std::vector<uint8_t> talk2() {
		return { 2, uint8_t(reg[2]>>8), uint8_t(reg[2]) };
	}
	virtual void listen0(const std::vector<uint8_t>& data) {
		reg[0] = data[0] << 8 | data[1];
	}
	virtual void listen1(const std::vector<uint8_t>& data) {
		reg[1] = data[0] << 8 | data[1];
	}
	virtual void listen2(const std::vector<uint8_t>& data) {
		reg[2] = data[0] << 8 | data[1];
	}
public:
	void talk(int v);
	void listen(int r, const std::vector<uint8_t>& data);
	virtual void reset() {}
	virtual void flush() {}
	virtual ~ADB_device() { SDL_DestroyMutex(mutex); }
	friend void adb_set_state(int b);
	void change(int i) ;
};
/** ADB Bus */
class ADB_Bus {
	friend class ADB_device;
	enum STATE { WAIT, DATA_SIZE, DATA } listen_state;
	int listen_cmd;
	int listen_left;
	int listen_dev;
	std::vector<uint8_t> listen_data;
	std::queue<uint8_t> talk_data;
public:
	/** connected devices */
	ADB_device* devices[16];
	int state;
	void cmd(uint8_t c);
	void send_to(uint8_t c);
	uint8_t get_from();
};


#endif

