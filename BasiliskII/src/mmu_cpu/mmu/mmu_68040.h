#ifndef MMU_68040_
#define MMU_68040_
#include <stdint.h>
struct ttc_t {
    uint8_t address_base;
    uint8_t address_mask;
    bool E;
    uint8_t S;
    uint8_t U;
    uint8_t CM;
    bool W;
};
struct mmusr_t {
    uint32_t PA;
    bool B, G, S, M, W, T, R;
    uint8_t U, CM;
};
struct atc_entry_t {
    uint32_t addr : 20;
    bool R : 1;
    bool W : 1;
    bool M : 1;
    uint8_t cm : 2;
    bool S : 1;
    uint8_t U : 2;
};
struct mmu_68040_cmn {
    uint32_t urp;
    uint32_t srp;
    bool tcr_e, tcr_p;

    mmusr_t MMUSR;

    void pflushn_an(uint32_t atc_key);
    void pflush_an(uint32_t atc_key);
    void pflushn();
    void pflush();
};

struct mmu_68040 {
    ttc_t TTR[2];
    std::unordered_map<uint32_t, atc_entry_t> atc_cache_l_u, atc_cache_l_s,
        atc_cache_g_u, atc_cache_g_s;
    presult ptest(const uint32_t addr, bool rw, bool s);
    presult test_TTR(uint32_t addr);
    uint32_t do_mmu(uint32_t vaddr, bool rw, bool s);
};
extern mmu_68040_cmn mmu;
extern mmu_68040 mmu_d, mmu_i;
#endif