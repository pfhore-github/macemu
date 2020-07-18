#include "machine.hpp"
#include "io.hpp"
class MDU : public Machine {
public:
	enum class MODEL {
		IIci, IIci_PGC, IIsi, UNKNOWN,
	};
	MDU(MODEL model = MODEL::IIci, SIMM30_SIZE bankA = SIMM30_SIZE::SIZE_64MB, SIMM30_SIZE bankB = SIMM30_SIZE::SIZE_64MB);	
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
	void* get_ram_addr(uint32_t addr, int attr) override;
private:
	uint32_t bank_size[2];
};
