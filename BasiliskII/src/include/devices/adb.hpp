#pragma once
#include <memory>
#include <deque>
class VIA1;
class ADBController;
class ADBDevice {
protected:
	ADBController* controller;
	int mid;
	uint16_t reg[4];
public:	
	ADBDevice(ADBController* ctr, int id) :controller(ctr), mid(id) {}
	virtual ~ADBDevice() {}
	std::vector<uint8_t> talk(int r);
	void listen(uint8_t reg, uint16_t v);
	virtual std::vector<uint8_t> talk0() { return {};}
	virtual std::vector<uint8_t> talk1() { return {};}
	virtual std::vector<uint8_t> talk2() { return {};}
	virtual void listen0(uint16_t) { }
	virtual void listen1(uint16_t) { }
	virtual void listen2(uint16_t) { }
	virtual void reset() {}
	virtual void flush() {}
};

class ADBController {
	int st = 3;
	int state = 0;
	int size = 0;
	uint16_t listen_v;
	std::deque<uint8_t> bufs;
	int cmd_target;
	int cmd_reg;
public:
	friend class VIA1;
	std::shared_ptr<ADBDevice> devices[16];
	std::vector<uint8_t> talk(int r);
	void cmd(uint8_t v);
	void data(uint8_t c, int b);
	uint8_t read();
};

class ADBMouse : public ADBDevice {
	int old_x, old_y;
	std::vector<uint8_t> talk0() override;
	std::vector<uint8_t> talk1() override;
	void flush() override { old_x = old_y = 0; }
	void reset() override { old_x = old_y = 0; }
public:
	ADBMouse(ADBController* ctr, int id) :ADBDevice(ctr, id) {
		reg[3] = 3 << 8;
	}
//	void write(uint8_t reg, uint16_t v) override;
};
