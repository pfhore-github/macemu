#pragma once
#include <boost/lockfree/queue.hpp>
#include <atomic>
class SCC_impl {
	friend class SCC;
protected:
	const bool is_modem;
	virtual void write_reg_impl(int n, uint8_t v) = 0;
	virtual uint8_t read_reg_impl(int n) = 0;
	std::atomic<uint8_t> rr[16];
	std::atomic<uint8_t> wr[16];
	SCC_impl(bool is_modem);
	int reg_ptr;
public:
	virtual std::shared_ptr<SCC_impl> clone(bool t) = 0;
	std::shared_ptr<SerialDevice> device;

	boost::lockfree::queue<uint8_t> data_buffers {3};
	boost::lockfree::queue<uint8_t> status_buffers {3};
	virtual ~SCC_impl() {}
	uint8_t read_reg();
	void write_reg(uint8_t);
	void cmd(uint8_t c);
	uint8_t read_data();
	void write_data(uint8_t v);
	void reset();
};

class Z8530 : public SCC_impl {
	void write_reg_impl(int n, uint8_t v) override;
	uint8_t read_reg_impl(int n) override;
	void write_reg0(uint8_t);
	void write_reg9(uint8_t);
public:
	Z8530(bool c = false) :SCC_impl(c) {}
	std::shared_ptr<SCC_impl> clone(bool t) override { return std::make_shared<Z8530>(t); }
};

class Z85C80 : public Z8530 {
//	void write_reg_impl(int n, uint8_t v) override;
//	uint8_t read_reg_impl(int n) override;
public:
	Z85C80(bool c = false) :Z8530(c) {}
	std::shared_ptr<SCC_impl> clone(bool t) override { return std::make_shared<Z85C80>(t); }
};
