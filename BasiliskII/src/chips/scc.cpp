#include "scc.hpp"
#include "devices/serial.hpp"
#include "scc_impl.hpp"
#include "registers.hpp"
#include "machine.hpp"
#include "via1.hpp"
#include <boost/crc.hpp>
#include <thread>
SCC_impl::SCC_impl(bool is_modem)
	:is_modem(is_modem),
	 device(std::make_shared<SerialDevice>()) {
}
void SCC::reset() {
	IUS = SCC_INT::NONE;
	modem->reset();
	printer->reset();
}
void SCC::connect_modem(const std::shared_ptr<SerialDevice>& t) {
	modem->device->connected_to.reset();
	modem->device = t;
	t->connected_to = modem;
	t->run( *modem ); // Driver should make thread
}
void SCC::connect_printer(const std::shared_ptr<SerialDevice>& t) {
	printer->device->connected_to.reset();
	printer->device = t;
	t->connected_to = printer;
	t->run( *printer );  // Driver should make thread
}
SCC::SCC(const std::shared_ptr<SCC_impl>& o) {
	modem = o->clone(true);
	modem->parent = this;
	printer = o->clone(false);
	printer->parent = this;
	vec_f = 0;
	modem->external_pending = false;
	modem->trans_pending = false;
	modem->recv_avail_pending = false;
	modem->recv_special_pending = false;
	printer->external_pending = false;
	printer->trans_pending = false;
	printer->recv_avail_pending = false;
	printer->recv_special_pending = false;
	reset();
}

void SCC::write_MIC(uint8_t v) {
	switch( v >> 6 & 3 ) {
	case 0 : break; // do nothing
	case 1 : printer->ch_reset(); break;
	case 2 : modem->ch_reset(); break;
	case 3 : reset(); break;
	}
	VIS = v & 1;
	NV = v >> 1 & 1;
	DLC = v >> 2 & 1;
	MIE = v >> 3 & 1;
	status_H = v >> 4 & 1;
	INTACK = v >> 5 & 1;
		
}
void SCC_impl::reset() {
	ch_reset();
	write_reg_impl(11, 8);
	write_reg_impl(14, 0x30);
}

void SCC_impl::ch_reset() {
	reg_ptr = 0;
	top_error = 0;
	latch_enable = false;
	current_rr0 = uint8_t(RR0::TX_OVERRUN_EOM) | uint8_t(RR0::TX_BUFFER_EMPTY);	
	first_letter = false;
	read_buffers.consume_all([](uint16_t) {} );
	write_reg_impl(0, 0);
	write_reg_impl(1, 0);
	write_reg_impl(3, 0);
	write_reg_impl(4, 4);
	write_reg_impl(5, 0);
	write_reg_impl(7, 0x20);
	write_reg_impl(9, 0);
	write_reg_impl(10, 0);
	write_reg_impl(14, 0x20);
	write_reg_impl(15, 0xF8);
	residue_codes = 3;
	end_of_frame = false;
	first_letter = true;
}
// Data Recieve
void SCC_impl::recieve_xd( const stream_t& in) {
	// data arrive
	if( ! w_req_is_wait_mode && w_req_is_recv_mode ) {
		machine->via1->scc_wr_req = false;
	}
	interrupt( SCC_INT::RECV_AVAIL );
	current_rr0 &= uint8_t(RR0::RX_CHAR_AVAILABLE) ^ 0xff;
	if( async_mode ) {
		switch( in.index() ) {
		case 0 :
		{
			current_rr0 &= uint8_t(RR0::BREAK_ABORT) ^ 0xff;
			uint16_t v = std::get<0>(in).value;
			bool parity = (! parity_even) ^ __builtin_parity(v);
			if( parity != std::get<0>(in).parity ) {
				v |= 0x1000;
			}
			switch( recv_size ) {
			case 5 : v |= 0xE0; break;
			case 6 : v |= 0xC0; break;
			case 7 : v |= 0x80; break;
			}
			read_buffers.push( v );
			current_rr0 |= uint8_t(RR0::RX_CHAR_AVAILABLE);
			break;
		}
		case 3 :
			// BREAK
			read_buffers.push( 0x0000 );
			int_external( RR0::BREAK_ABORT );
			return;
		default :
			// framing error
			read_buffers.push( 0x4000 );
		}
	} else {
		switch( in.index() ) {
		case 1 :
		{
			current_rr0 &= uint8_t(RR0::BREAK_ABORT) ^ 0xff;
			auto recv = std::get<1>(in);
			uint16_t sync = recv.sync;
			uint16_t sync_v = 0;
			switch( sync_size ) {
			case 6 : sync_v = sync_char & 0x3f; break;
			case 8 : sync_v = sync_char & 0xff; break;
			case 12 : sync_v = (sync_char & 0xff00) >> 4 | ( (sync_char & 0xf0)>> 4); break;
			case 16 : sync_v = sync_char; break;
			default :
				/* */
				break;
			}
			if( recv_hunt_mode ) {
				if( sync != sync_v ) {
					// skip data
					return;
				} else {
					recv_hunt_mode = false;
					recv_crc16.reset( crc_init ? 0xffff : 0);
					recv_crc_ccitt.reset(crc_init ? 0xffff : 0);
					int_external(RR0::SYNC_HUNT);
				}
			}
			if( sync_size != 8 || ! recv_sync_char_load_inhibit ) {
				read_buffers.push( sync_v );
			}
			for(unsigned int i = 0; i < recv.values.size(); ++i ) {
				uint16_t v = recv.values[i] & ((1 << recv_size) - 1);
				if( i < recv.parities.size() ) { 
					bool parity = (! parity_even) ^ __builtin_parity(recv.values[i]);
					if( parity != recv.parities[i] ) {
						v |= 0x1000;
					}
				}
				
				read_buffers.push( v );
			}
			recieved_crc = recv.crc;
			current_rr0 |= uint8_t(RR0::RX_CHAR_AVAILABLE);
			break;
		}
		default :
			// framing error
			read_buffers.push( 0x4000 );
		}
	}
}
void SCC_impl::hsk_i(bool v) {
	if( v && !( current_rr0 & uint8_t(RR0::CTS) )) {
		int_external( RR0::CTS );
	} else if( ! v && ( current_rr0 & uint8_t(RR0::CTS) ) ) {
		current_rr0 &= uint8_t(RR0::CTS) ^ 0xff;
	}
}

void SCC_impl::gp_i(bool b) {
	if( b && !( current_rr0 & uint8_t(RR0::DCD) )) {
		int_external( RR0::DCD );
	} else if( ! b && ( current_rr0 & uint8_t(RR0::DCD) ) ){
		current_rr0 &= uint8_t(RR0::DCD) ^ 0xff;
	}
}
uint8_t SCC_impl::read_data() {
	uint16_t v;
	if( read_buffers.pop(v) ) {
		{
			std::lock_guard lock(parent->rr2_mutex);
			recv_avail_pending = false;
		}
		recv_data = v & 0xff;
		if( enable_crc ) {
			if( crc_is_16 ) {
				recv_crc16.process_byte( recv_data );
			} else {
				recv_crc_ccitt.process_byte( recv_data );
			}
		}
		top_error = ((v >> 8) & 0x40) | ((top_error | (v >> 8)) & 0x30);
		if( ( parity_error_is_special && (v & 0x1000) ) || 	// parity error
			( v & 0x4000 ) 			// framing error
			) {
			first_letter = true;
			interrupt( SCC_INT::RECV_SPECIAL);
		}
	} else {
		if( w_req_is_wait_mode && w_req_is_recv_mode ) {
			machine->via1->scc_wr_req = false;
		}
	}
	if( read_buffers.empty() ) {
		// transmission done; calc CRC
		if( recieved_crc != crc_is_16 ? recv_crc16.checksum() : recv_crc16.checksum() ) {
			// CRC ERROR
			top_error |= 0x4000;
			interrupt( SCC_INT::RECV_SPECIAL);
		}
	}
	return recv_data;
}
uint8_t SCC_impl::decode_byte(uint8_t w) {
	switch( send_size ) {
	case 8 : return w;
	case 7 : return w & 0x7f;
	case 6 : return w & 0x3f;
	case 5 :
		if( (w & 0xf0) == 0xf0 ) {
			return w & 1;
		} else if( (w & 0xe0) == 0xe0 ) {
			return w & 3;
		} else if( (w & 0xc0) == 0xc0 ) {
			return w & 7;
		} else if( (w & 0x80) == 0x80 ) {
			return w & 0xf;
		} else {
			return w & 0x1f;
		}
	}
	return 0;
}

// transmit data(async)
void SCC_impl::write_data(uint8_t v) {
	uint8_t val = decode_byte(v);
	if( async_mode ) {
		{
			std::lock_guard lock(parent->rr2_mutex);
			trans_pending = false;
		}
		current_rr0 &= uint8_t( RR0::TX_BUFFER_EMPTY ) ^ 0xff;
		if( rts ) {
			device->transmit_xd(data_async(val));
		} else {
			send_pending.emplace_back( data_async(val) );
		}
		all_sent.store( true );
	} else {
		if( first_letter ) {
			switch( sync_size ) {
			case 6 : sync_data.sync = sync_char & 0x3f; break;
			case 8 : sync_data.sync = sync_char & 0xff; break;
			case 12 : /* no 12bit sync char for send*/
			case 16 :
				sync_data.sync = sync_char;
				break;
			}
			first_letter = false;
		}
		sync_data.values.push_back( val );
		sync_data.parities.push_back( __builtin_parity( val ) );
		if( enable_crc ) {
			if( crc_is_16 ) {
				send_crc16.process_byte( val );
			} else {
				send_crc_ccitt.process_byte( val );
			}
		}
	}
	current_rr0 |= uint8_t(RR0::TX_BUFFER_EMPTY);
	if( w_req_is_wait_mode && ! w_req_is_recv_mode ) {
		machine->via1->scc_wr_req = false;
	}
	interrupt( SCC_INT::TRANS );
}
uint8_t SCC::read(int addr) {
	switch(addr) {
	case SCC_REG::B_CMD : return printer->read_reg();
	case SCC_REG::A_CMD : return modem->read_reg();
	case SCC_REG::B_DATA : return printer->read_data();
	case SCC_REG::A_DATA : return modem->read_data();
	default : return 0;
	}
}
void SCC::write(int addr, uint8_t v) {
	switch(addr) {
	case SCC_REG::B_CMD : printer->write_reg(v); break;
	case SCC_REG::A_CMD : modem->write_reg(v); break;
	case SCC_REG::B_DATA : printer->write_data(v); break;
	case SCC_REG::A_DATA : modem->write_data(v); break;
	}
}
uint8_t SCC_impl::read_reg() {
	int reg = reg_ptr;
	reg_ptr = 0;
	return read_reg_impl(reg);
}


void SCC_impl::write_reg(uint8_t v) {
	if( reg_ptr == 0 ) {
		write_reg_impl(0, v);
	} else {
		write_reg_impl(reg_ptr, v);
		reg_ptr = 0;
	}
}
uint8_t SCC_impl::read_reg_impl(int reg) {
	uint8_t v = 0;
	switch(reg) {
	case 0 :					// Buffer Status
	case 4 :
		if( latch_enable ) {
			v = ( external_status_int_enable & latched_rr0 ) |
				( (external_status_int_enable ^ 0xff) & current_rr0 );
		} else {
			v = current_rr0;
		}
		break;
	case 1 :
	case 5 :
		v = end_of_frame << 7 |
			top_error | // get from error FIFO
			residue_codes << 1 |
			all_sent;
		break;
	case 2 : // Interrupt vector; taken from parent
		if( is_modem ) {
			v = parent->vec_f.load();
		} else {
			std::shared_lock lock(parent->rr2_mutex);
			if( parent->modem->recv_avail_pending ) {
				v = parent->status_H ? 0x30 : 0xC;
			} else if( parent->modem->recv_special_pending ) {
				v = parent->status_H ? 0x70 : 0xE;
			} else if( parent->modem->trans_pending ) {
				v = parent->status_H ? 0x10 : 0x8;
			} else if( parent->modem->external_pending ) {
				v = parent->status_H ? 0x50 : 0xA;
			} else if( recv_avail_pending ) {
				v = parent->status_H ? 0x20 : 0x4;
			} else if( recv_special_pending ) {
				v = parent->status_H ? 0x60 : 0x6;
			} else if( trans_pending ) {
				v = 0;
			} else if( external_pending ) {
				v = parent->status_H ? 0x40 : 0x2;
			}
		}
		break;
	case 3 : // Interrupt pending vector; only modem port
		if( is_modem ) {
			v = parent->get_int_vec();
		} else {
			v = 0;
		}
		break;		
	case 8 :					// Recieve Data Register
		v = read_data();
		break;
	case 9 :
	case 13 :
		return rr[13];
	default :
		return rr[reg & 15];
	}
	return v;
}

void SCC_impl::write_reg0(uint8_t v) {
	reg_ptr = v & 7;
	switch( (v>>3) & 7 ) {
	case 0 : break;
	case 1 : reg_ptr |= 8; break;
	case 2 :
		// RESET Ext/Status Interrupts
	{
		std::lock_guard lock(parent->rr2_mutex);
		latch_enable = false;		
		external_pending = false;
	}
		break;
	case 3 :		
		// SEND Abort SDLC
		device->transmit_xd( data_eom {} );
		int_external(RR0::TX_OVERRUN_EOM);
		break;
	case 4 :
		// Enable Int on Next Rx Character
		first_letter = true;
		break;
	case 5 : {
		// Reset Tx Int Pending
		std::lock_guard lock(parent->rr2_mutex);
		trans_pending = false;
	}
		if( ! async_mode && ! (current_rr0 & uint8_t(RR0::TX_OVERRUN_EOM) ) ) {
			// transmit CRC
			sync_data.crc = crc_is_16 ? send_crc16.checksum() : send_crc_ccitt.checksum();
			device->transmit_xd(sync_data);
			sync_data.values.clear();
			sync_data.parities.clear();
			current_rr0 |= uint8_t(RR0::TX_OVERRUN_EOM);
			first_letter = false;
		}
		break;
	case 6 :
		// ERROR reset
	{
		std::lock_guard lock(parent->rr2_mutex);
		top_error = 0;
		recv_special_pending = false;
		break;
	}
	case 7 :
		// RESET Highest IUS
		parent->IUS = SCC_INT::NONE;
		break;
	}
	switch( (v >> 6) & 3 ) {
	case 0 : break;
	case 1 :
		// RESET Rx CRC
		recv_crc16.reset( crc_init ? 0xffff : 0);
		recv_crc_ccitt.reset(crc_init ? 0xffff : 0);
		break;
	case 2 :
		// Reset Tx CRC
		send_crc16.reset(crc_init ? 0xffff : 0);
		send_crc_ccitt.reset(crc_init ? 0xffff : 0);
		break;
	case 3 :
		// Reset Tx Underrun
		current_rr0 &= uint8_t(RR0::TX_OVERRUN_EOM) ^ 0xff;
		break;
	}
}

void SCC_impl::int_external(RR0 v) {
	current_rr0 |= uint8_t(v);
	if( v == RR0::BREAK_ABORT && ( external_status_int_enable & uint8_t(RR0::BREAK_ABORT))) {
		latched_rr0 |= current_rr0.load();
	}
	if( ! latch_enable && ( external_status_int_enable & uint8_t(v)) ) {
		interrupt( SCC_INT::EXTERNAL);
	}

}

void SCC_impl::write_reg_impl(int n, uint8_t v) {
	switch(n) {
	case 0 :
		write_reg0(v);
		return;
	case 1 :
		w_req_is_wait_mode = v >> 7 & 1;
		w_req_is_recv_mode = v >> 6 & 1;
		recv_interrupt_mode = v >> 3 & 3;
		parity_error_is_special = v >> 2 & 1;
		transmit_empty_interrupt = v >> 1 & 1;
		external_interrupt_enable = v & 1;
		break;
	case 2 :
	{
		std::lock_guard lock(parent->rr2_mutex);
		parent->vec_f = v;
		parent->modem->recv_avail_pending = v >> 5 & 1;
		parent->modem->recv_special_pending = v >> 5 & 1 ;
		parent->modem->trans_pending = v >> 4 & 1;
		parent->modem->external_pending = v >> 3 & 1;
		parent->printer->recv_avail_pending = v >> 2 & 1;
		parent->printer->recv_special_pending = v >> 2 & 1;
		parent->printer->trans_pending = v >> 1 & 1;
		parent->printer->external_pending = v & 1;
		break;
	}
	case 3 :
		switch( (v >> 6) & 3 ) {
		case 0 : recv_size = 5; break;
		case 1 : recv_size = 7; break;
		case 2 : recv_size = 6; break;
		case 3 : recv_size = 8; break;
		}
		auto_enable = v >> 5 & 1;
		if( auto_enable ) {
			rts = true;
		}
		if( v >> 4 & 1 ) {
			recv_hunt_mode = true;
			int_external(RR0::SYNC_HUNT);
		}
		recv_crc_enable = v >> 3 & 1;
		recv_addr_search_mode = v >> 2 & 1;
		recv_sync_char_load_inhibit = v >> 1 & 1;
		recv_enable = v & 1;
		break;
	case 4 :
	{
		parity_enable = v & 1;
		parity_even = v >> 1 & 1;
		async_mode = ((v >> 2) & 3) != 0;
		if( async_mode ) {
			current_rr0 |= uint8_t(RR0::TX_OVERRUN_EOM);
		}
		sdlc_mode = false;
		switch( (v>>4)&3 ) {
		case 0 : sync_size = sync_6bit ? 6 : 8; break;
		case 1 : sync_size = sync_6bit ? 12 : 16; break;
		case 2 : sdlc_mode = true; break;
		case 3 : sync_size = -1; break;
		return;
		}
		break;
	}
	case 5 :
		if( dtr_enable && v & 0x80 ) {
			device->hsk_o();
		}
		switch( (v >> 5) & 3 ) {
		case 0 : send_size = 5; break;
		case 1 : send_size = 7; break;
		case 2 : send_size = 6; break;
		case 3 : send_size = 8; break;
		}
		if( v & 1 << 4 ) {
			device->transmit_xd(data_break {});
			return;
		}
		crc_is_16 = v >> 2 & 1;
		if( ! ( v & 2 ) ) {
			if( async_mode ) {
				if( ! auto_enable ) {
					rts = false;
				} 
			}
		} else {
			rts = true;
			if( async_mode ) {
				for( const data_async& c : send_pending ) {
					device->transmit_xd( c );
				}
				send_pending.clear();
			}
		}
		enable_crc = v & 1;
		return;
	case 6 :
		if( sync_size <= 8 ) {			
			sync_char = v;
		} else {
			sync_char = (sync_char & 0xff00) | v;
		}
		break;
	case 7 :
		sync_char = (sync_char & 0xff) | v << 8;
		break;
	case 8 : // transmit buffer 
		write_data(v);
		return;
	case 9 :
		parent->write_MIC(v);
		return;
	case 10 :
		sync_6bit = v & 1;
		switch( sync_size ) {
		case 6 :
		case 8 : sync_size = sync_6bit ? 6 : 8; break;
		case 12 :
		case 16 : sync_size = sync_6bit ? 12 : 16; break;
		}
		crc_init = v >> 7 & 1;
		break;
	case 15 :
		external_status_int_enable = v & 0xFA; // BIT0, 2 is reserved
	}
}

void SerialDevice::recieve_xd(const stream_t& v) {
	connected_to.lock()->recieve_xd(v);
}
void SerialDevice::hsk_i(bool v) {
	connected_to.lock()->hsk_i(v);
}

void SerialDevice::gp_i(bool v) {
	connected_to.lock()->gp_i(v);
}

void SCC_impl::interrupt(SCC_INT i ) {
	switch( i ) {
	case SCC_INT::TRANS :
		if( transmit_empty_interrupt ) {
			parent->interrupt( is_modem ? SCC_INT::TRANS_A : SCC_INT::TRANS_B );
		}
		break;
	case SCC_INT::RECV_AVAIL :
		if( (recv_interrupt_mode == 1 && std::exchange(first_letter, false)) ||
			recv_interrupt_mode == 2 ) {
			parent->interrupt( is_modem ? SCC_INT::RECV_AVAIL_A : SCC_INT::RECV_AVAIL_B );
		}
		break;
	case SCC_INT::RECV_SPECIAL :
		if( recv_interrupt_mode ) {
			first_letter = true;
			parent->interrupt( is_modem ? SCC_INT::RECV_SPECIAL_A : SCC_INT::RECV_SPECIAL_B );
		}
		break;
	case SCC_INT::EXTERNAL :
		if( external_interrupt_enable ) {
			parent->interrupt( is_modem ? SCC_INT::EXTERNAL_A : SCC_INT::EXTERNAL_B );
		}
		break;
	default :
		break;
	}
}
static constexpr int get_int_priority(SCC_INT i) {
	switch(i) {
	case SCC_INT::TRANS_B : return 1;
	case SCC_INT::EXTERNAL_B : return 0;
	case SCC_INT::RECV_AVAIL_B :
	case SCC_INT::RECV_SPECIAL_B : return 2;
	case SCC_INT::TRANS_A : return 4;
	case SCC_INT::EXTERNAL_A : return 3;
	case SCC_INT::RECV_AVAIL_A : 
	case SCC_INT::RECV_SPECIAL_A : return 5;
	default : return -1;
	}
}

void SCC::interrupt(SCC_INT i) {
	if( ! MIE ) {
		return;
	}
	std::lock_guard lock(rr2_mutex);
	switch( i ) {
	case SCC_INT::NONE: break;
	case SCC_INT::EXTERNAL_B : printer->external_pending = true; break;
	case SCC_INT::TRANS_B : printer->trans_pending = true; break;
	case SCC_INT::RECV_AVAIL_B : printer->recv_avail_pending = true; break;
	case SCC_INT::RECV_SPECIAL_B : printer->recv_special_pending = true; break;
	case SCC_INT::EXTERNAL_A : modem->external_pending = true; break;
	case SCC_INT::TRANS_A : modem->trans_pending = true; break;
	case SCC_INT::RECV_AVAIL_A : modem->recv_avail_pending = true; break;
	case SCC_INT::RECV_SPECIAL_A : modem->recv_special_pending = true; break;
	}
	if( (IUS == SCC_INT::NONE) ||
		(! ( DLC && i != SCC_INT::RECV_SPECIAL_A ) &&
		 get_int_priority(IUS) <= get_int_priority(i) )) {
		if( i == SCC_INT::EXTERNAL_A ) {
			modem->latch_enable = true;
			modem->latched_rr0 = modem->current_rr0.load();
		} else if( i == SCC_INT::EXTERNAL_B ) {
			printer->latch_enable = true;
			printer->latched_rr0 = printer->current_rr0.load();
		}
		IUS = i;
		cpu.irq( 4 );
	}
}

uint8_t SCC::get_int_vec() {
	std::shared_lock lock(rr2_mutex);
	return ( modem->recv_avail_pending | modem->recv_special_pending ) << 5 |
		modem->trans_pending << 4 |
		modem->external_pending << 3 |
		( printer->recv_special_pending | printer->recv_avail_pending ) << 2 |
		printer->trans_pending << 1 |
		printer->external_pending;
}
uint8_t SCC::interrupt_ack() {
	if( ! NV ) {
		return get_int_vec();
	}
	return 0;
}

void SCC::write_reg(bool is_modem, int reg, uint8_t value) {
	if( is_modem) {
		modem->write_reg_impl(reg, value);
	} else {
		printer->write_reg_impl(reg, value);
	}
}
uint8_t SCC::read_reg(bool is_modem, int reg) {
	if( is_modem ) {
		return modem->read_reg_impl(reg);
	} else {
		return printer->read_reg_impl(reg);
	}		
}
