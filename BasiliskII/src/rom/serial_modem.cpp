#include "data.hpp"
#include "via1.hpp"
#include "prototype.hpp"
#include "scc.hpp"
#include "intel.h"
#include <assert.h>
#include "mem.hpp"
void emul_reset(uint32_t* d, uint32_t* a);
extern uint8_t *RAMBaseHost;
namespace ROM {
bool _4BAC0() { return ram_check(); }
void _BA2CE() {}
void _BA578();
void _B9A2A(uint8_t d0, uint32_t d6);
void _B9A46( uint16_t dv, uint32_t c1);
static const std::unordered_map<char, int> _B99B0 = {
	{ 'S', 0},
	{ 'L', 4},
	{ 'B', 2},
	{ 'D', 0},
	{ 'C', 0},
	{ 'G', 4},
	{ '0', 4},
	{ '1', 4},
	{ '2', 4},
	{ '3', 0},
	{ '4', 0},
	{ '5', 0},
	{ '6', 0},
	{ '7', 0},
	{ 'A', 0},
	{ 'H', 0},
	{ 'R', 0},
	{ 'M', 0},
	{ 'E', 0},
	{ 'I', 0},
	{ 'P', 2},
	{ 'T', 4},
	{ 'N', 4},
	{ 'V', 0},
	{ 'Z', 0},
};
static uint32_t _B97F2[] = {
	0x000BA26A,
	0x00047D1C,
	0x000479C0,
	0x00047CC0,
	0x00047BDE,
	0x00047AD8,
	0x00047C6C,
	0x00047E62,
	0x000BA284,
	0x00047EEE,
};
static void run_comm_T0(int d0 ) {
	switch( d0 ) {
	case 0 : // $BA26A
	{
		cpu.USP = AR(6);
		bool z = _4BAC0();
		DR(7) = z ? 0 : -1;
		if( z ) {
			DR(6) = read_l( AR(7) );
		}
		AR(6) = cpu.USP;
		break;
	}
	case 1 : // $47D1C
		DR(0) = 1;
		DR(1) = -2;
	}
}
// $B98BC
void comm_modem_port(uint32_t d6) {
	getHWInfo2(0);				// $47752
	if( HWCFG_FLAGS.test( HWCFG_T::POWERBOOK ) ) {
		send_to_pb_key();		// $47826
	}
	if( ! INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ENABLE ) ) {
		getHWInfo2(0);				// $47752
		if( INIT_HW_FLG.test( INIT_HW_FLG_T::KNOWN_HW ) ){
			play_chime_4(); // $45C52
		}
	}
	
	// $B9916
	DR(5) = 0;
	// get_scc_addr(); // $B9F46
	init_modem_port(); // $B9F78
	for(;;) {
	  LOOP:
		auto [ status, v ] = read_modem_port(); // $BA0EA
		if( status < 0 ) {
			DR(4) = modem_port_send1( DR(4), d6); // $B9A4E
			goto LOOP;
		}
		if( v == '*' ) {
			INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_IN_CMD );			
			DR(4) = modem_port_send1( DR(4), d6);
			goto LOOP;
		}
		// $B9948
		if( ! INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_IN_CMD ) ) {
			if( v != '!' ) {
				_B9A46(v, d6);
				goto LOOP;
			} else {
				// $BA578
				_BA578();
			}
		} else {
			// $B997C
			INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_IN_CMD );
			auto p = _B99B0.find( v );
			if( p == _B99B0.end() ) {
				_B9A46(v, d6);
				goto LOOP;
			}
			uint32_t arg = read_modem_cmd_dword( p->second); // $BA138
			uint8_t cmd = v;
			int16_t counter = DR(4) >> 16;
			uint32_t addr = AR(4);
			switch( v ) {
			case 'S':			// $B9AF4
				// reset modem port status
				INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_UNSEND );
				INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_WAIT );
				write_modem_integer( v, p->second ); // $BA182
				break;
			case 'L':			// $B9B04
				// load address register & reset count register
				addr = arg;
				counter = 0;
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
					write_modem_integer( arg, 4);
				}
				break;
			case 'B':			// $B9B24
				// load count register
				counter = arg;
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
					write_modem_integer( arg, 2);
				}
				break;
			case 'D':			// $B9B42
				// read and store memory
				for(counter = counter-1; counter >= 0; counter-- ) {
					uint8_t v = read_modem_cmd_dword(1);
					write_b( addr++, v );
					d6 += v;
				}
				break;
			case 'C':			// $B9B66
			{
				// caluclrate check sum
				uint32_t sum = 0;
				for(counter = counter - 1; counter >= 0; counter-- ) {
					sum += read_b( addr++ );
				}
				INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
				write_modem_integer( sum, 4 );
				break;
			}
				
			case 'G':			// $B9B8C
				// jump
				DR(0) = v;
				DR(1) = cmd;
				DR(4) = counter << 16 | (DR(4) & 0xffff);
				DR(7) = INIT_FLAGS.to_ulong() << 16 | EXCEPTION_STATUS;
				AR(4) = addr;
				run_rom(arg );
				cmd = 'G';
				break;
			case '0':			// $B9BA6
				// set A0
				AR(0) = arg;
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
					write_modem_integer( arg, 4 );
				}
				break;
			case '1':			// $B9BC0
				// set A1
				AR(1) = arg;
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
					write_modem_integer( arg, 4 );
				}
				break;
			case '2':			// $B9BDA
				// set MMU CACR
				mmu->movec_to( 2, arg );
				break;
			case '3':			// $B9BE6
				// reset MMU?
				if( dynamic_cast<MC68040_MMU*>(mmu) ) {
					// 68040
					uint32_t cacr = mmu->movec_from(2);
					mmu->exec( 0xF478, 7, 0 ); // CPUSHA DC
					mmu->movec_to( 3, mmu->movec_from( 3 ) &~ (1 << 15) ); // Disable paging
					mmu->movec_to( 2, cacr ); // CACR set
				} else if( auto mmup = dynamic_cast<MC68030_MMU*>(mmu) ) {
					// 68030
					mmu->movec_to( 2, 0x3819 );
					mmup->pmove(0, 0); // TC // from $B9BE2
				}
				break;				
			case '4':			// $B9C3A
				// reset error status
				EXCEPTION_STATUS = 0;
				d6 = 0;
				break;
			case '5':			// $B9C42
				// reset modem port flag
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_WAIT );
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_UNSEND );
				reset_VIA1_timer2();		// $BA090
//				get_scc_addr();
				break;
			case '6':			// $B9C5E
				// hardware reset
				emul_reset(cpu.R, &cpu.R[8]);
				break;
			case '7':			// $B9C64
				// set unknown flag
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_UNKNOWN );
				break;				
			case 'A':			// $B9C6C
				// set ASCII flag
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ASCII );
				break;
			case 'H':			// $B9C74
				// reset ASCII flag
				INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_ASCII );
				break;
			case 'R':			// $B9C7C
				// dump d6&d7.L
				INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
				write_modem_integer( d6, 4 );
				INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
				write_modem_integer( EXCEPTION_STATUS, 2 );
				break;
			case 'M':			// $B9C9C
				// read memory and transport
				if( --counter < 0 ) {
					counter = 0;
				}
				
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_CRLF );
				do {
					uint32_t d = read_l(addr);
					addr += 4;
					write_modem_integer( d, 4 );
				} while( --counter >= 0 );
				break;
			case 'E':			// $B9CBE
				// enable echo
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ECHO );
				break;
			case 'I':			// $B9CC6
				// restart ROM
				cpu.PC = 0x8C;
				return boot();
			case 'P':			// $B9CD0
				// print error msg
				write_modem_port_cstr("*ERROR*"); // $B9FF6; arg=$BA008
				break;
			case 'T':			// $B9CDC
			{
				cpu.VBR = rom_base | 0x46FF4;					
				addr = arg;
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					write_modem_integer( arg, 4 ); // $BA182
				}
				uint16_t flag = read_modem_cmd_dword( 2); // $BA138
				EXCEPTION_STATUS &= 0x7fff;
				INIT_FLAGS = flag;
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					write_modem_integer( DR(1), 2 );
				}
				int8_t ret;
				uint8_t val;
				do {
					try {
						DR(6) = 0;
						uint16_t cmd = addr >> 16;
						if( cmd >= 0xA ) { // value@$B981A
							_B9A46( 0x6f, 0 );
							break;
						}
						run_comm_T0( cmd );
					} catch (Exception::Base& e) {
						if( ! ex_handler(e.vec) ) {
							return;
						}
					}

					// $B9D6E
					AR(7) = 0x2600;
//					get_scc_addr();
					if( DR(6) ) {
						write_modem_port_cstr("*ERROR*");
						if( INIT_FLAGS.test( 29 - 16 ) ) {
							for(;;) {
								try {
									run_comm_T0( cmd );
								} catch (Exception::Base& e) {
									if( ! ex_handler(e.vec) ) {
										return;
									}
									break;
								}
								if( INIT_FLAGS.test( 28 - 16 ) ) {
									break;
								}
							}
						}
					}
					// $B9DA4
					DR(1) = addr - 1;
					if( int16_t(DR(1)) == 0 ) {
						break;
					}
					std::tie( ret, val ) = read_modem_port();
					DR(0) = ret << 8 | val;
				} while( ret < 0 );
				break;
			}
			case 'N':			// $B9DC2
			{
				addr = arg;
				cpu.Z = _4BAC0();
				AR(1) = read_l(AR(7));
				AR(1) = read_l(AR(1));
				_BA2CE();
//				get_scc_addr();
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					write_modem_integer( addr, 4); // $BA182
				}
				DR(1) = read_modem_cmd_dword( 2); // $BA138
				EXCEPTION_STATUS &= 0x7fffff;
				INIT_FLAGS |= DR(1) << 12;
				if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
					write_modem_integer( DR(0), 2 );
				}
				int16_t dr1 = (addr >> 16) - 128;
				if( dr1 > 33 ) { // value@$B98A2
					_B9A46( 63, 0 );
					break;
				}
				DR(1) <<= 2;
				DR(1) = read_rom32( 0xB981E + DR(1) );
				uint32_t ar5 = read_rom32( 0xB981E + DR(1) );
				int16_t a4 = addr;
				getHWInfo2( 0 );
				uint32_t d0 = DR(0), d1 = DR(1), d2 = DR(2), a0 = AR(0), a1 = AR(1), a2 = AR(2), a5 = ar5;
				for(;;) {
					DR(6) = 0;
					DR(0) = d0;
					DR(1) = d1;
					DR(2) = d2;
					AR(0) = a0;
					AR(1) = a1;
					AR(2) = a2;
					AR(5) = a5;
					run_rom( AR(5) );
//					get_scc_addr();
					init_modem_port();
					if( DR(6) ) {
						write_modem_port_cstr("*ERROR*");
						if( INIT_FLAGS.test( 29 - 16 ) ) {
							run_rom( ar5 );						
						}
						if( ! INIT_FLAGS.test( 28 - 16 ) ) {
							goto _B9EBC;
						}
					}
					if( --a4 == 0 ) {
						break;
					}
					auto [ ret, v ] = read_modem_port( );
					DR(0) = ret << 8 | v;
					if( ret >= 0 )
						break;
				}
				// $B9EAA
				cmd = 'N';
			  _B9EBC:
				break;				
			}
			case 'V':			// $B9ECA
//				get_scc_addr();
				init_modem_port();
				write_modem_port_cstr("STM Version 2.2, Aaron Ludtke \r\n"
									  "CTE Version 2.1 \r\n"
									  "ROM Version "); // @BA050
				DR(4) = DR(7);
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ASCII );
				INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
				write_modem_integer( 0x7C, 1 ); // @9(ROM MAJOR VERSION.L)
				write_modem_port_byte( ' ' );
				INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_CRLF );
				write_modem_integer( 0x32F1, 2 ); // @12(ROM MINOR VERSION)
				DR(7) = DR(4);				
				break;
			case 'Z':			// $B9F32
				for(;;) {
					SDL_Delay(1000);
				}
			}

			DR(4) = ((uint16_t)counter << 16) | (DR(4) & 0xffff);
			DR(6) = d6;
			AR(4) = addr;
			// B9A18
			write_modem_port_byte( '*' ); // B9FDA
			write_modem_port_byte( cmd ); // B9FDA
			INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_CRLF );
			write_modem_integer( 0, 0 );
			INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
			DR(4) = modem_port_send1(DR(4), d6);			
		}
	}
}

// $B995A
void _B995A() {
	uint32_t d0_tp = DR(0);
	get_scc_addr();				// $B9F46
	init_modem_port();			// $B9F78
	if( d0_tp == 0 ) {
		_B9A2A( 0x21, DR(6) );	
	} else {
		_B9A2A( d0_tp, DR(6) );	
	}
}
// $B9A2A
void _B9A2A(uint8_t d0, uint32_t d6) {
	write_modem_port_byte(d0);
	INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_CRLF );
	write_modem_integer( 0, 0 );
	INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
	DR(4) = modem_port_send1(DR(4), d6);
}

// $B9A46
void _B9A46( uint16_t dv, uint32_t c1) {
	write_modem_port_byte(dv ); // $B9FDA	
	DR(4) = modem_port_send1(DR(4), c1);
}

// $B9A4E
uint32_t modem_port_send1(uint32_t retry, uint32_t c1) {
	if( ! INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_UNSEND ) ) {
		return retry;
	}
	if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_WAIT ) ) {
		if( ! machine->via1->bit( VIA_REG::IFR, 5 ) ) {
			return retry;
		}
		machine->via1->write( VIA_REG::IER, 0x20 );
		machine->via1->write( VIA_REG::TIMER2_L, 0xff );
		machine->via1->write( VIA_REG::TIMER2_H, 0xff );
		
		if( int16_t(--retry) > 0 ) {
			return retry;
		}
		retry = 12;						
	}
	// $B9A82
	static const char* _BA010 = "*APPLE*";

	write_modem_port_cstr( _BA010); // $B9FF6
	INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_CRLF );
	INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_ASCII );
	write_modem_integer( c1, 4); // $BA182
	write_modem_integer( EXCEPTION_STATUS, 2); // $BA182
	INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_ASCII );
	write_modem_port_byte( '*' ); // $B9FDA
	auto offsets = get_scc_addr();	 // $B9F46
	static const char* _BA01B = "03211116788AAABCDEHIJKLMNOPQRSTUVWXYZabcdefghijklmno"; 
	uint16_t via_w = (int8_t)(offsets.via_offset >> 8);
	write_modem_port_byte( _BA01B[ via_w+3 ] ); // $B9FDA
	static const char* _BA04C = "*\r\n";
	write_modem_port_cstr( _BA04C );
	if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_WAIT ) ) {
		return retry;
	}
	INIT_FLAGS.reset( INIT_FLAG_T::MODEM_PORT_UNSEND );
	return retry;
}

// $B9F78
void init_modem_port() {
	// $B9FC0
	static const uint8_t table[][2] = {
		// register & data
		{ 0x09, 0xC0, },		// MASTER Interrupt Control (HW RESET)
		{ 0x0F, 0x00, },		// External Status Interrupt Ctl(all disable)
		{ 0x04, 0x4C, },		// transmit param(x16 clock/8bit sync/1 stop/parity disalbe)
		{ 0x0B, 0x50, },		// clock mode(recv=BR, tran=BR/TRxC=Xtal)
		{ 0x0E, 0x00, },		// Misc Ctl(all disable)
		{ 0x0C, 0x0A, },		// baud rate lower=10
		{ 0x0D, 0x00, },		// baud rate high=0(10Hz)
		{ 0x0E, 0x01, },		// MIsc Ctl(BR Enable)
		{ 0x0A, 0x00, },		// Misc Tran/Recv Ctl(all disable)
		{ 0x03, 0xC1, },		// Rcv param(Rx 8bit enable)
		{ 0x05, 0xEA, },		// Tran param(8bit DTR/RTS on)
		{ 0x01, 0x00, },		// Tran IRQ & transfer mode(all disable)
		{ 0xff, 0xff, },		// END
	};
	uint32_t offset = 0;
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::IOP_SCC ) ) {
		offset = 0x10;
		init_scc_iop_fw(); // $47EF6
	}
	machine->scc->read(offset+SCC_REG::B_CMD);
	for( int i = 0; table[i][0] != 0xff; ++i ) {
		machine->scc->write(offset+SCC_REG::A_CMD, table[i][0]); // REG number
		machine->scc->write(offset+SCC_REG::A_CMD, table[i][1]); // DATA
	}
	INIT_FLAGS.set( INIT_FLAG_T::MODEM_PORT_VALID );
	machine->scc->read(offset+SCC_REG::A_DATA);
	machine->via1->set( VIA_REG::DDRA, 3 );
	machine->via1->clear( VIA_REG::RA, 3 );
}
// $B9FDA
void write_modem_port_byte(uint8_t val) {
	machine->scc->write(SCC_REG::A_CMD, 0x30); // ERROR RESET
	machine->scc->write(SCC_REG::A_DATA, val);	
	do {
		machine->scc->write(SCC_REG::A_CMD, 1);
	} while( ! (machine->scc->bit(SCC_REG::A_CMD, 0)) ); //RR[1].0 [ ALL Sent ]
}

// $B9FF6
void write_modem_port_cstr( const char* c) {
	while(char p = *c++) {
		write_modem_port_byte(p); // $B9FDA
	}
}

// $BA0EA
std::pair<int8_t, uint8_t> read_modem_port() {
	uint16_t v = 0x8000;
	int offset = std::dynamic_pointer_cast<SCC_IOP>(machine->scc) ? 0x10 : 0;
	if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_VALID )
		// Data Avaliable
		&& machine->scc->bit(offset+SCC_REG::A_CMD, 0) ) {
		
		// read register 1(ERROR)
		machine->scc->write(offset+SCC_REG::A_CMD, 1);
		v = machine->scc->read(offset+SCC_REG::A_CMD) & 0x70;
		if( v ) {
			// ERROR Reset
			machine->scc->write(offset+SCC_REG::A_CMD, 0x30 );			
		}
		return std::make_pair(v, machine->scc->read(offset+SCC_REG::A_DATA));
	}
	return std::make_pair(0x80, 0);
}

// $BA11A
int8_t from_ascii(int8_t v ) {
	v &= 0x7F;
	v -= 0x30;
	if( v < 0 || v > 0x16 ) {
		return 0;
	}
	if( v > 9 ) {
		return v-7;
	}
	return v;	
}

// $BA138
uint32_t read_modem_cmd_dword( int size) {
	uint32_t ret = 0;
	if( size ) {
		if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ASCII) ) {
			size <<= 1;
		}
		for(int n = 0; n < size; ++n ) {
			int8_t status;
			uint8_t val;
			do {
				std::tie(status, val) = read_modem_port(); // $BA0EA
			} while( status < 0 );
			if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ASCII ) ) {
				ret = ret << 4 | from_ascii(val);
			} else {
				ret = ret << 8 | val;
			}
		}
	}
	return ret;
}

// $BA174
uint8_t to_ascii(uint8_t v) {
	if( v >= 10 ) {
		return v + 0x37;
	} else {
		return v + 0x30;
	}
}

// $BA182
// d2=1;  000000aa
// d2=2;  0000aabb
// d2=1;  000000xy
void write_modem_integer( uint32_t value, int16_t len) {
	if( len ) {
		uint32_t v = ROL_L(value, (4 - len) * 8);
		if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ASCII ) ) {
			len <<= 1;
		}
		for(int i = 0; i < len; ++i ) {
			if( ! INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ASCII ) ) {
				v = ROL_L(v, 8);
				write_modem_port_byte(v); // $B9FDA
			} else {
				v = ROL_L(v, 4);
				write_modem_port_byte( to_ascii(v & 0x0f)); // $B9FDA
			}
		}
	}
	if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_CRLF ) ) {
		write_modem_port_byte( 13); // SEND CR; $B9FDA
		write_modem_port_byte( 10); // SEND LF; $B9FDA
	}
}
// $BA578
void _BA578() {
	AR(7) -= 6;
	char* p = reinterpret_cast<char*>(&RAMBaseHost[ AR(7) ]);
	read_modem_cmd1( p, 4 );
	AR(7) += 8;
	DR(0) = pop32();
	if( DR(0) == 0 ) {
		AR(7) += 2;
	} else {
		DR(1) = pop16();
		AR(5) = read_l( AR(7)+4);
		push32(DR(3));
		push32(AR(3));
		write_l( AR(5), DR(0) );
		write_l( AR(5)+1, DR(1) );
		if( DR(1) ) {
			AR(7) -= DR(1);
			p = reinterpret_cast<char*>(&RAMBaseHost[ AR(7) ]);
			read_modem_cmd1( p, DR(1) );
			AR(7) += 9;
			DR(0) = read_l( AR(5) );
		}
		AR(0) = 0x408BA52C;
		if( DR(0) >= 0x10000 ) {
			AR(0) = 0x408BA558;
			DR(0) -= 0x10000;
		}
		push32( read_l( AR(5) + 22 ) );
		DR(0) <<= 2;
	}	
	// BA60C
}

// $BA964
void read_modem_cmd1( char* dst, uint32_t count ) {
	if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ASCII ) ) {
		count <<= 1;
	}
	for(uint32_t i = 0; i < count; ++i ) {
		int8_t status;
		uint8_t val;
		do {
			std::tie(status, val) = read_modem_port(); // BA0EA
		} while ( status < 0 );
		if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ECHO ) ) {
			write_modem_port_byte( val ); // $B9FDA
		}
		if( INIT_FLAGS.test( INIT_FLAG_T::MODEM_PORT_ASCII ) ) {
			if( val >= 0x61 ) {
				val &= 0xdf;
			}
			val = from_ascii(val);
			if( ! (i & 1 ) ) {
				*dst = val << 4;
			} else {
				*dst++ |= val;
			}		
		} else {
			*dst++ = val;
		}
	}
}

}

static void check_scc() {
	assert( ROM::test_machine_register(machine->scc, AR(3)));
	assert( ROM::test_machine_register(machine->scc, AR(3)+DR(3)));
}
namespace ROMWrapper {
using namespace ROM;
void run_B98BC() {
	ROM::INIT_FLAGS = DR(7) >> 16;
	ROM::EXCEPTION_STATUS = DR(7) & 0xffff;
	ROM::comm_modem_port( DR(6) );
	DR(7) = ROM::INIT_FLAGS.to_ulong() << 16 | ROM::EXCEPTION_STATUS;
}
void run_B9A4E() {
	check_scc();
	assert( test_machine_register(machine->via1, AR(2)));
	INIT_FLAGS = DR(7) >> 16;
	EXCEPTION_STATUS = DR(7);
	DR(4) = modem_port_send1( DR(4), DR(6));
	cpu.PC = rom_base | 0xB9928;
	DR(7) = INIT_FLAGS.to_ulong() << 16 | EXCEPTION_STATUS;
}

void run_B9F78() {
	check_scc();
	assert( test_machine_register(machine->via1, AR(2)));
	INIT_FLAGS = DR(7) >> 16;
	init_modem_port();
	DR(7) = INIT_FLAGS.to_ulong() << 16 | EXCEPTION_STATUS;

}



void run_B9FDA() {
	check_scc();
	ROM::write_modem_port_byte( DR(0));
}

void run_B9FF6() {
	check_scc();
	std::string p;
	while( char c = ROM::read_rom8( AR(1)++ ) ) {
		p += c;
	}
	ROM::write_modem_port_cstr(p.c_str());
}

void run_BA0EA() {
	check_scc();
	ROM::INIT_FLAGS = DR(7) >> 16;
	auto [ code, val ] = ROM::read_modem_port();
	DR(0) = ( (uint8_t)code << 8 | val );
}


void run_BA11A() {
	DR(0) = ROM::from_ascii(DR(0));
 	cpu.PC = AR(6);
}

void run_BA138() {
	check_scc();
	
	ROM::INIT_FLAGS = DR(7) >> 16;
	DR(1) = ROM::read_modem_cmd_dword(DR(2));
	cpu.PC = AR(6);
}


void run_BA174() {
	DR(0) = (DR(0) & 0xffffff00) | ROM::to_ascii(DR(0));
 	cpu.PC = AR(6);
}

void run_BA182() {
	check_scc();
	ROM::INIT_FLAGS = DR(7) >> 16;
	ROM::write_modem_integer( DR(0), DR(2));
	cpu.PC = AR(6);
}

void run_BA964() {
	check_scc();
	ROM::INIT_FLAGS = DR(7) >> 16;
	ROM::EXCEPTION_STATUS = DR(7);
	int pc = pop32();
	int sz = pop32();
	uint32_t dst = pop32();
	std::vector<char> buf( sz );
	ROM::read_modem_cmd1(buf.data(), sz);
	for(int i = 0; i < sz; ++i ) {
		write_b(dst+i, buf[i]);
	}
	DR(7) = ROM::INIT_FLAGS.to_ulong() << 16 | ROM::EXCEPTION_STATUS;
	cpu.PC = pc;
}


}
