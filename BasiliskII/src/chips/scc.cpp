#include "scc.hpp"
#include "devices/serial.hpp"
#include "scc_impl.hpp"
#include "registers.hpp"
#include "machine.hpp"
#include "via1.hpp"
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
}
void SCC::connect_printer(const std::shared_ptr<SerialDevice>& t) {
	printer->device->connected_to.reset();
	printer->device = t;
	t->connected_to = printer;
}
SCC::SCC(const std::shared_ptr<SCC_impl>& o) {
	modem = o->clone(true);
	modem->parent = this;
	printer = o->clone(false);
	printer->parent = this;
	vec_f = 0;
	vec_h = 0;
	vec_l = 0;
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
	r0_latched = false;
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
	tx_overrun_eom = true;
	tx_buffer_empty = true;
	residue_codes = 3;
	end_of_frame = false;
	first_letter = true;
}

// Data Recieve
void SCC_impl::recieve_xd( const std::vector<bool>& vs) {
	// data arrive
	if( ! w_req_is_wait_mode && w_req_is_recv_mode ) {
		machine->via1->scc_wr_req = false;
	}
	interrupt( SCC_INT::RECV_AVAIL );
	if( async_mode ) {
		if( vs.empty() ) {
			// break
			if( ! r0_latched ) {
				break_abort.store( true );
				interrupt( SCC_INT::EXTERNAL);
				r0_latched = true;
			}
		} 
		uint16_t v = 0;
		if( int(vs.size()) != async_recv_size + parity_enable) {
			// framing error
			v = 0x4000;			
		} else {
			bool parity = ! parity_even;
			for(int i = 0; i < 8; ++i ) {
				if( i < async_recv_size ) {
					parity ^= vs[i];
					v |= vs[i] << i;
				} else {
					v |= 1 << i;
				}
			}
			if( parity != vs[async_recv_size] ) {
				// parity error
				v |= 0x1000;
				
			}
		}
		read_buffers.push( v );
		rx_char_avaliable.store( true );
	}
}
void SCC_impl::hsk_i() {
	cts.store( true );
	if( cts_int_enable ) {
		interrupt( SCC_INT::EXTERNAL);
	}
}

void SCC_impl::gp_i(bool b) {
	interrupt( SCC_INT::EXTERNAL);
}
uint8_t SCC_impl::read_data() {
	uint16_t v;
	if( read_buffers.pop(v) ) {
		recv_data = v & 0xff;
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
	return recv_data;
}
void SCC_impl::transmit(const std::vector<bool>& w) {
	device->transmit_xd(w);
	all_sent.store( true );
	tx_buffer_empty.store( true );
	if( w_req_is_wait_mode && ! w_req_is_recv_mode ) {
		machine->via1->scc_wr_req = false;
	}
	interrupt( SCC_INT::TRANS );
}
// transmit data
void SCC_impl::write_data(uint8_t v) {
	tx_buffer_empty.store( true );
	if( ! async_mode ) {
		// sync mode
		std::vector<bool> w(8);
		for(int i = 0; i < 8; ++i ) {
			w[i] = v >> i & 1;
		}
		transmit(w);
	} else {
		// async mode
		int sz = async_send_size == 5 ? 5 - (__builtin_clz(v^0xff)-24) : async_send_size;
		std::vector<bool> w;		
		bool parity = ! parity_even;
		for(int i = 0; i < sz; ++i ) {
			parity ^= (v>>i)&1;
			w.push_back( (v>>i)&1 );
		}
		if( parity_enable ) {
			w.push_back( parity );
		}
		if( rts ) {
			transmit(w);
		} else {
			send_pending.insert( send_pending.end(), w.begin(), w.end() );
		}
	}
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
		v = break_abort.exchange(false) << 7 |
			tx_overrun_eom << 6 |
			cts << 5 |
			sync_hunt << 4 |
			dcd << 3 |
			tx_buffer_empty << 2 |
			zero_count << 1 |
			rx_char_avaliable;
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
			v = parent->vec_f;
		} else {
			v = parent->vec_h << 4 | parent->vec_l;
		}
		break;
	case 3 : // Interrupt pending vector; only modem port
		if( is_modem ) {
			v = parent->int_pending.load();
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
		r0_latched = false;
		break;
	case 3 :
		// TODO: SEND Abort
		tx_overrun_eom = true;
		break;
	case 4 :
		// Enable Int on Next Rx Character
		first_letter = true;
		break;
	case 5 :
		// TODO: Reset Tx Int Pending
		break;
	case 6 :
		// ERROR reset
		top_error = 0;
		break;
	case 7 :
		// TODO: RESET Highest IUS
		parent->IUS = SCC_INT::NONE;
		break;
	}
	switch( (v >> 6) & 3 ) {
	case 0 : break;
	case 1 :
		// TODO: RESET Rx CRC
		break;
	case 2 :
		// TODO: Reset Tx CRC
		break;
	case 3 :
		// Reset Tx Underrun
		tx_overrun_eom = false;
		break;
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
		parent->vec_f = v;
		parent->vec_h = v >> 4;
		parent->vec_l = v & 0xf;
		break;
	case 3 :
		switch( (v >> 6) & 3 ) {
		case 0 : async_recv_size = 5; break;
		case 1 : async_recv_size = 7; break;
		case 2 : async_recv_size = 6; break;
		case 3 : async_recv_size = 8; break;
		}
		auto_enable = v >> 5 & 1;
		if( auto_enable ) {
			rts = true;
		}
		recv_hunt_mode = v >> 4 & 1;
		recv_crc_enable = v >> 3 & 1;
		recv_addr_search_mode = v >> 2 & 1;
		recv_sync_char_load_inhibit = v >> 1 & 1;
		recv_enable = v & 1;
		break;
	case 4 :
		parity_enable = v & 1;
		parity_even = v >> 1 & 1;
		async_mode = ((v >> 2) & 3) != 0;
		return;
	case 5 :
		if( dtr_enable && v & 0x80 ) {
			device->hsk_o();
		}
		switch( (v >> 5) & 3 ) {
		case 0 : async_send_size = 5; break;
		case 1 : async_send_size = 7; break;
		case 2 : async_send_size = 6; break;
		case 3 : async_send_size = 8; break;
		}
		if( v & 1 << 4 ) {
			transmit(std::vector<bool>(32u, false));
			return;
		}
		if( ! ( v & 2 ) ) {
			if( async_mode ) {
				if( ! auto_enable ) {
					rts = false;
				} 
			}
		} else {
			rts = true;
			if( async_mode ) {
				transmit( send_pending );
				send_pending.clear();
			}
		}
		return;
	case 8 : // transmit buffer 
		write_data(v);
		return;
	case 9 :
		parent->write_MIC(v);
		return;
	case 15 :
		break_abort_int_enable = v >> 7 & 1;
		underrun_eom_int_enable = v >> 6 & 1;
		cts_int_enable = v >> 5 & 1;
		sync_hunt_enable = v >> 4 & 1;
		dct_int_enable = v >> 3 & 1;
		zero_count_enable = v >> 1 & 1;
	}
}


void SerialDevice::recieve_xd(const std::vector<bool>& v) {
	connected_to.lock()->recieve_xd(v);
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
	switch( i ) {
	case SCC_INT::NONE: break;
	case SCC_INT::EXTERNAL_B : int_pending |= 1; break;
	case SCC_INT::TRANS_B : int_pending |= 2; break;
	case SCC_INT::RECV_AVAIL_B : 
	case SCC_INT::RECV_SPECIAL_B : int_pending |= 4; break;
	case SCC_INT::EXTERNAL_A : int_pending |= 8; break;
	case SCC_INT::TRANS_A : int_pending |= 0x10; break;
	case SCC_INT::RECV_AVAIL_A : 
	case SCC_INT::RECV_SPECIAL_A : int_pending |= 0x20; break;
	}
	if( (IUS == SCC_INT::NONE) ||
		(! ( DLC && i != SCC_INT::RECV_SPECIAL_A ) &&
		 get_int_priority(IUS) <= get_int_priority(i) )) {
		IUS = i;
		if( status_H ) {
			switch( i ) {
			case SCC_INT::NONE: break;
			case SCC_INT::EXTERNAL_B : vec_h = 4; break;
			case SCC_INT::TRANS_B : vec_h = 0; break;
			case SCC_INT::RECV_AVAIL_B : vec_h = 2; break;		
			case SCC_INT::RECV_SPECIAL_B : vec_h = 6; break;
			case SCC_INT::EXTERNAL_A : vec_h = 5; break;
			case SCC_INT::TRANS_A : vec_h = 1; break;
			case SCC_INT::RECV_AVAIL_A : vec_h = 3; break;
			case SCC_INT::RECV_SPECIAL_A : vec_h = 7; break;
			}
		} else {
			switch( i ) {
			case SCC_INT::NONE: break;
			case SCC_INT::EXTERNAL_B : vec_l = 1; break;
			case SCC_INT::TRANS_B : vec_l = 0; break;
			case SCC_INT::RECV_AVAIL_B : vec_l = 2; break;		
			case SCC_INT::RECV_SPECIAL_B : vec_l = 3; break;
			case SCC_INT::EXTERNAL_A : vec_l = 5; break;
			case SCC_INT::TRANS_A : vec_l = 4; break;
			case SCC_INT::RECV_AVAIL_A : vec_l = 6; break;
			case SCC_INT::RECV_SPECIAL_A : vec_l = 7; break;
			}
		}
		cpu.irq( 4 );
	}
}

