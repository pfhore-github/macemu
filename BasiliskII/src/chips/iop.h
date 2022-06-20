#ifndef IOP_H__
#define IOP_H__
#include <stdint.h>
/* IOP (Z8530) */
class IOP  {
	uint8_t A, X, Y, S;
protected:
	uint8_t data[0x10000];
	uint8_t ctrl = 0;
	virtual uint8_t read_hw(uint32_t addr) { return 0; }
	virtual void write_hw(uint32_t addr, uint8_t v) {  }
private:
	uint16_t ram_addr = 0;
public:
	uint8_t read(int addr);
	void write(int addr, uint8_t v);
};
namespace IOP_REG {
enum IOP_t {
	ADDR_HI = 0,
	ADDR_LO,
	CTRL,
	RAM = 4
};
}

#define IOP_BYPASS      0x01    /* bypass-mode hardware access */
#define IOP_AUTOINC     0x02    /* allow autoincrement of ramhi/lo */
#define IOP_RUN         0x04    /* set to 0 to reset IOP chip */
#define IOP_IRQ         0x08    /* generate IRQ to IOP if 1 */
#define IOP_INT0        0x10    /* intr priority from IOP to host */
#define IOP_INT1        0x20    /* intr priority from IOP to host */
#define IOP_HWINT       0x40    /* IRQ from hardware; bypass mode only */
#define IOP_DMAINACTIVE 0x80    /* no DMA request active; bypass mode only */
#endif