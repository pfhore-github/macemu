#pragma once
#include <optional>
#include <memory>
class SCC_impl;
class SerialDevice {
	friend class SCC;
	friend std::shared_ptr<SCC_impl> which(SerialDevice& t) {
		return t.connected_to.lock();
	}
	std::weak_ptr<SCC_impl> connected_to;
public:
    // handshake output
	virtual void hsk_o(bool t) = 0;
    // data output
	virtual void t_xd(uint8_t v) = 0;
    // data output
	virtual void abort() = 0;
};
