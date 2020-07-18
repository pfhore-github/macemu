#pragma once
#include <memory>
#include <deque>
class SCC_impl;
constexpr uint32_t ASYNC_STOP_1 = 0x80;
constexpr uint32_t ASYNC_STOP_1_HALF = 0xC0;
constexpr uint32_t ASYNC_STOP_2 = 0xE0;
class SerialDevice {
	friend class SCC;
	friend std::shared_ptr<SCC_impl> which(SerialDevice& t) {
		return t.connected_to.lock();
	}
protected:
	std::weak_ptr<SCC_impl> connected_to;
public:
    // handshake output
	virtual void hsk_o() {}
    // data output
	virtual void transmit_xd(uint8_t out ) {}
	virtual ~SerialDevice() {}
	void recieve_xd(const std::deque<uint8_t>& in);
	void hsk_i(bool v);
	void gp_i(bool v);
	virtual void run(SCC_impl&) { }
};

