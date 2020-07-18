#include "machine.hpp"
#include "io.hpp"
class GLU : public Machine {
public:
	friend class VIA2;
	enum class MODEL {
		IIx, SE30, IIcx
	};
	
	GLU( MODEL model = MODEL::IIx, SIMM30_SIZE bankA = SIMM30_SIZE::SIZE_64MB, SIMM30_SIZE bankB = SIMM30_SIZE::SIZE_64MB);
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
	void* get_ram_addr(uint32_t addr, int attr) override;
	~GLU() {}
private:
	uint32_t bank_size[2];
	uint8_t bankA_size;

};
