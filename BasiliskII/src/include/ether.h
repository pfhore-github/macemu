/*
 *  ether.h - Ethernet device driver
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

#ifndef ETHER_H
#define ETHER_H

struct sockaddr_in;

extern void EtherInit(void);
extern void EtherExit(void);

extern int16_t EtherOpen(uint32_t pb, uint32_t dce);
extern int16_t EtherControl(uint32_t pb, uint32_t dce);
extern void EtherReadPacket(uint32_t &src, uint32_t &dest, uint32_t &len, uint32_t &remaining);

// System specific and internal functions/data
extern void EtherReset(void);
extern void EtherInterrupt(void);

extern bool ether_init(void);
extern void ether_exit(void);
extern void ether_reset(void);
extern int16_t ether_add_multicast(uint32_t pb);
extern int16_t ether_del_multicast(uint32_t pb);
extern int16_t ether_attach_ph(uint16_t type, uint32_t handler);
extern int16_t ether_detach_ph(uint16_t type);
extern int16_t ether_write(uint32_t wds);
extern bool ether_start_udp_thread(int socket_fd);
extern void ether_stop_udp_thread(void);
extern void ether_udp_read(uint32_t packet, int length, struct sockaddr_in *from);

extern uint8_t ether_addr[6];	// Ethernet address (set by ether_init())

// Ethernet driver data in MacOS RAM
enum {
	ed_DeferredTask = 0,	// Deferred Task struct
	ed_Code = 20,			// DT code is stored here
	ed_Result = 30,			// Result for DT
	ed_DCE = 34,			// DCE for DT (must come directly behind ed_Result)
	ed_RHA = 38,			// Read header area
	ed_ReadPacket = 52,		// ReadPacket/ReadRest routines
	SIZEOF_etherdata = 76
};

extern uint32_t ether_data;	// Mac address of driver data in MacOS RAM

// Ethernet packet allocator (optimized for 32-bit platforms in real addressing mode)
class EthernetPacket {
#if SIZEOF_VOID_P == 4 && REAL_ADDRESSING
	uint8_t packet[1516];
 public:
	uint32_t addr(void) const { return (uint32_t)packet; }
#else
	uint32_t packet;
 public:
	EthernetPacket();
	~EthernetPacket();
	uint32_t addr(void) const { return packet; }
#endif
};

// Copy packet data from WDS to linear buffer (must hold at least 1514 bytes),
// returns packet length
static inline int ether_wds_to_buffer(uint32_t wds, uint8_t *p)
{
	int len = 0;
	while (len < 1514) {
		int w = ReadMacInt16(wds);
		if (w == 0)
			break;
		Mac2Host_memcpy(p, ReadMacInt32(wds + 2), w);
		len += w;
		p += w;
		wds += 6;
	}
	return len;
}

#endif
