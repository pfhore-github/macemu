class FPU {
public:
	virtual void exec(CPU* cpu, uint16_t op, int mode, int reg) = 0;
};
