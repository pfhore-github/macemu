#pragma once
#include <atomic>
#include <queue>
#include "boost/lockfree/queue.hpp"
#include "boost/crc.hpp"
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
enum class RR0 : unsigned char {
	RX_CHAR_AVAILABLE = 1U,
	BAUD_ZERO_COUNT = 1U << 1,
	TX_BUFFER_EMPTY = 1U << 2,
	DCD = 1U << 3,
	SYNC_HUNT = 1U << 4, 
	CTS = 1U << 5, 
	TX_OVERRUN_EOM = 1U << 6,
	BREAK_ABORT = 1U << 7
};
enum class SYNC_SIZE {
	SIZE_6,
	SIZE_8,
	SIZE_12,
	SIZE_16,
	EXTERNAL
};
class SCC_impl {
	friend class SCC;
	friend class RecieveVisiter;
	SCC* parent;
protected:
	const bool is_modem;
	virtual void write_reg_impl(int n, uint8_t v);
	virtual uint8_t read_reg_impl(int n);
	std::atomic<uint8_t> rr[16];
	std::atomic<uint8_t> wr[16];
	// RR0
	std::atomic<uint8_t> current_rr0;
	uint8_t latched_rr0;
	// RR1
	std::atomic<bool> all_sent;
	uint8_t residue_codes;
	bool end_of_frame;
	// RR3
	bool external_pending;
	bool trans_pending;
	bool recv_avail_pending;
	bool recv_special_pending;
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
	uint8_t recv_size;
	bool auto_enable;
	bool recv_hunt_mode;
	bool recv_crc_enable;
	bool recv_addr_search_mode;
	bool recv_sync_char_load_inhibit;
	bool recv_enable;
	// WR4
	bool parity_enable;
	bool parity_even;
	uint8_t async_mode; // terminate char; #80/#C0/E0
	bool sdlc_mode;
	SYNC_SIZE sync_size;
	// WR5
	uint8_t send_size = 5;
	bool send_enable;
	bool crc_is_16;
	bool rts = true;
	bool enable_crc;
	// WR6/7
	uint8_t sync_char[2];
	// WR8
	std::optional<uint8_t> send_char;
	// WR10
	bool sync_6bit = false;
	bool crc_init;
	// WR 14
	bool dtr_enable = false;
	// WR 15
	uint8_t external_status_int_enable;
	
	SCC_impl(bool is_modem);
	void write_wr8(uint8_t v);
	uint8_t decode_byte(uint8_t w);
	int reg_ptr;
	void interrupt(SCC_INT i);
public:
	// data read
	void recieve_xd(const std::deque<uint8_t>& in);
	// handshake input
	void hsk_i(bool v);
	// GP input
	void gp_i(bool v);

	virtual std::shared_ptr<SCC_impl> clone(bool t) = 0;
	std::shared_ptr<SerialDevice> device;
	// FIFO doens't overflow(no overrun support...)
	boost::lockfree::queue<uint16_t> read_buffers { 256 };
	
	uint8_t top_error;
	virtual ~SCC_impl() {}
	uint8_t read_reg();
	void write_reg(uint8_t);
	void cmd(uint8_t c);
	uint8_t read_data();
	void write_data();
	void ch_reset();
	void reset();
	void write_reg0(uint8_t);

private:
	// internal
	bool first_letter;
	bool latch_enable;
	boost::crc_16_type send_crc16;
	boost::crc_ccitt_type send_crc_ccitt;
	boost::crc_16_type recv_crc16;
	boost::crc_ccitt_type recv_crc_ccitt;
	bool sync_sending;
	void int_external(RR0);
	void sync_done();

	void recieve_async(std::deque<uint8_t>& in);
	void recieve_sync(std::deque<uint8_t>& in);
	void recieve_sdlc(std::deque<uint8_t>& in);
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
