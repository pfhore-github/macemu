#pragma once
#include <vector>
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
	virtual void hsk_o() {}
    // data output
	virtual void transmit_xd(const std::vector<bool>& ) {}
	void recieve_xd(const std::vector<bool>& v);

};
