/*
 *  serial.h - Serial device driver
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

#ifndef SERIAL_H
#define SERIAL_H

/*
 *  port:
 *    0 - .AIn
 *    1 - .AOut
 *    2 - .BIn
 *    3 - .BOut
 */

#ifdef POWERPC_ROM
extern int16 SerialOpen(uint32 pb, uint32 dce);
extern int16 SerialPrimeIn(uint32 pb, uint32 dce);
extern int16 SerialPrimeOut(uint32 pb, uint32 dce);
extern int16 SerialControl(uint32 pb, uint32 dce);
extern int16 SerialStatus(uint32 pb, uint32 dce);
extern int16 SerialClose(uint32 pb, uint32 dce);
extern int16 SerialNothing(uint32 pb, uint32 dce);
#else
extern int16_t SerialOpen(uint32_t pb, uint32_t dce, int port);
extern int16_t SerialPrime(uint32_t pb, uint32_t dce, int port);
extern int16_t SerialControl(uint32_t pb, uint32_t dce, int port);
extern int16_t SerialStatus(uint32_t pb, uint32_t dce, int port);
extern int16_t SerialClose(uint32_t pb, uint32_t dce, int port);
#endif

extern void SerialInterrupt(void);

// System specific and internal functions/data
extern void SerialInit(void);
extern void SerialExit(void);

// Serial driver Deferred Task structure
enum {
	serdtCode = 20,		// DT code is stored here
	serdtResult = 30,
	serdtDCE = 34,
	SIZEOF_serdt = 38
};

// Variables for one (In/Out combined) serial port
// To implement a serial driver, you create a subclass of SERDPort
class SERDPort {
public:
	SERDPort()
	{
		is_open = false;
		input_dt = output_dt = 0;
	}

	virtual ~SERDPort() {}

	virtual int16_t open(uint16_t config) = 0;
	virtual int16_t prime_in(uint32_t pb, uint32_t dce) = 0;
	virtual int16_t prime_out(uint32_t pb, uint32_t dce) = 0;
	virtual int16_t control(uint32_t pb, uint32_t dce, uint16_t code) = 0;
	virtual int16_t status(uint32_t pb, uint32_t dce, uint16_t code) = 0;
	virtual int16_t close(void) = 0;

	bool is_open;		// Port has been opened
	uint8_t cum_errors;	// Cumulative errors

	bool read_pending;	// Read operation pending
	bool read_done;		// Read operation complete
	uint32_t input_dt;	// Mac address of Deferred Task for reading

	bool write_pending;	// Write operation pending
	bool write_done;	// Write operation complete
	uint32_t output_dt;	// Mac address of Deferred Task for writing

#ifdef POWERPC_ROM
	uint32_t dt_store;
#endif
};

extern SERDPort *the_serd_port[2];

#endif
