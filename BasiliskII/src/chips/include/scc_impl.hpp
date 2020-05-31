#pragma once
#include <atomic>
#include <queue>
#include "boost/lockfree/queue.hpp"
namespace WREG {
enum {
	CMD,
	IRQ_XFER_MODE,
	IRQ_VEC,
	RECV_PARAM_CTL,
	XFER_RECV_MISC_PARAM,
	XFER_PARAM_CTL,
	SYNC_ADDRESS,
	SYNC_FLAG,
};
}
static constexpr int W_CMD_REG = 0;
class SCC_impl {
	friend class SCC;
	friend class TransferModeBase;
	SCC* parent;
protected:
	const bool is_modem;
	virtual void write_reg_impl(int n, uint8_t v);
	virtual uint8_t read_reg_impl(int n);
	std::atomic<uint8_t> rr[16];
	std::atomic<uint8_t> wr[16];
	// RR0
	std::atomic<bool> rx_char_avaliable;
	std::atomic<bool> zero_count;
	std::atomic<bool> tx_buffer_empty;
	std::atomic<bool> dcd;
	std::atomic<bool> sync_hunt;
	std::atomic<bool> cts;
	bool tx_overrun_eom;
	std::atomic<bool> break_abort ;
	// RR1
	std::atomic<bool> all_sent;
	uint8_t residue_codes;
	bool end_of_frame;
    // RR8
	uint8_t recv_data;
	// WR1
	bool w_req_is_wait_mode;
	bool w_req_is_recv_mode;
	char recv_interrupt_mode;
	bool parity_error_is_special;
	bool transmit_empty_interrupt;
	bool external_interrupt_enable;
	// WR3
	uint8_t async_recv_size = 5;
	bool auto_enable = false;
	bool recv_hunt_mode = false;
	bool recv_crc_enable = false;
	bool recv_addr_search_mode = false;
	bool recv_sync_char_load_inhibit = false;
	bool recv_enable = false;
	// WR4
	bool parity_enable = false;
	bool parity_even = false;
	bool async_mode = false;
	// WR5
	uint8_t async_send_size = 5;
	bool rts = true;
	// WR 14
	bool dtr_enable = false;
	// WR 15
	bool break_abort_int_enable;
	bool underrun_eom_int_enable;
	bool cts_int_enable;
	bool sync_hunt_enable;
	bool dct_int_enable;
	bool zero_count_enable;
	
	SCC_impl(bool is_modem);
	void transmit(const std::vector<bool>& w);
	int reg_ptr;
	void interrupt(SCC_INT i);
public:
	// data read
	void recieve_xd( const std::vector<bool>& vs);
	// handshake input
	void hsk_i();
	// GP input
	void gp_i(bool v);

	virtual std::shared_ptr<SCC_impl> clone(bool t) = 0;
	std::shared_ptr<SerialDevice> device;
	// FIFO doens't overflow(no overrun support...)
	boost::lockfree::queue<uint16_t> read_buffers { 3 };
	std::vector<bool> send_pending;
	uint8_t top_error;
	virtual ~SCC_impl() {}
	uint8_t read_reg();
	void write_reg(uint8_t);
	void cmd(uint8_t c);
	uint8_t read_data();
	void write_data(const uint8_t v);
	void ch_reset();
	void reset();
	void write_reg0(uint8_t);
private:
	// internal
	bool first_letter;
	bool r0_latched;
};
// NMOS
class Z8530 : public SCC_impl {
public:
	Z8530(bool c = false) :SCC_impl(c) {}
	std::shared_ptr<SCC_impl> clone(bool t) override { return std::make_shared<Z8530>(t); }
};
// CMOS
class Z85C80 : public Z8530 {
//	void write_reg_impl(int n, uint8_t v) override;
//	uint8_t read_reg_impl(int n) override;
public:
	Z85C80(bool c = false) :Z8530(c) {}
	std::shared_ptr<SCC_impl> clone(bool t) override { return std::make_shared<Z85C80>(t); }
};
