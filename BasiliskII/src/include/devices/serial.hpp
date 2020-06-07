#pragma once
#include <vector>
#include <memory>
#include <variant>
class SCC_impl;
struct data_async {
	uint8_t value;
	bool parity;
	data_async(uint8_t v) :value(v), parity(__builtin_parity(v)) {}
	data_async(uint8_t v, bool p) :value(v), parity(p) {}
};
struct data_sync {
	uint16_t sync;
	std::vector<uint8_t> values;
	std::vector<bool> parities;
	uint16_t crc;
};
struct data_sdlc {
	uint8_t begin;
	uint8_t addr;
	uint8_t ctrl;
	std::vector<uint8_t> values;
	std::vector<bool> parities;
	uint16_t crc;
	uint8_t end;
};

struct data_break {};
struct data_eom {};
using stream_t = std::variant<data_async, data_sync, data_sdlc, data_break, data_eom>;
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
	virtual void transmit_xd(const stream_t& out ) {}
	virtual ~SerialDevice() {}
	void recieve_xd(const stream_t& in);
	void hsk_i(bool v);
	void gp_i(bool v);
	virtual void run(SCC_impl&) { }
};

