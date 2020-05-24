#include "machine.hpp"
#include "io.hpp"
class GLU : public Machine {
public:
	GLU();
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
	~GLU() {}
};
