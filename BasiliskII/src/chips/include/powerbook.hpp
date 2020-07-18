#pragma once
#include <vector>
#include <deque>
#include <optional>
class PB_EX_REG {
	friend class MSC_VIA1;
	friend class MSCRbv;
protected:
	int in_cmd = -1;
	uint8_t firmware[0x8004];
	bool firmware_loaded = false;
	bool ready = true;
	std::vector<uint8_t> buffers;
	std::deque<uint8_t> c_out;
public:
	std::optional<uint8_t> pop_out();
	std::optional<uint8_t> c_in;	
	void cmd(uint8_t c);
	virtual std::vector<uint8_t> do_cmd(uint8_t c);
	virtual unsigned int arg_size(uint8_t cmd);
};
static constexpr int PB_TRANS_READY = 1;
static constexpr int PB_TRANS_READ_MODE = 2;
