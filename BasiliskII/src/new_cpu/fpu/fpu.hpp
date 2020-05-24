#pragma once
class FPU {
public:
	enum STATE { NONE, UNIMP, POSTINST } state = NONE;
	virtual void exec(uint16_t op, int mode, int reg) = 0;
	virtual void reset() = 0;
};

class NoFPU : public FPU {
	void exec(uint16_t op, int mode, int reg) override;
	void reset() override;
};
