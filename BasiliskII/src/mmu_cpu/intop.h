#include "newcpu.h"
#include <stdint.h>
inline void TEST_NZ8(int8_t v) {
    regs.n = v < 0;
    regs.z = v == 0;
}
inline void TEST_NZ16(int16_t v) {
    regs.n = v < 0;
    regs.z = v == 0;
}
inline void TEST_NZ32(int32_t v) {
    regs.n = v < 0;
    regs.z = v == 0;
}
inline uint8_t DO_OR_B(uint8_t a, uint8_t b) {
    uint8_t v = a | b;
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint16_t DO_OR_W(uint16_t a, uint16_t b) {
    uint16_t v = a | b;
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint32_t DO_OR_L(uint32_t a, uint32_t b) {
    uint32_t v = a | b;
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint8_t DO_AND_B(uint8_t a, uint8_t b) {
    uint8_t v = a & b;
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint16_t DO_AND_W(uint16_t a, uint16_t b) {
    uint16_t v = a & b;
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint32_t DO_AND_L(uint32_t a, uint32_t b) {
    uint32_t v = a & b;
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint8_t DO_EOR_B(uint8_t a, uint8_t b) {
    uint8_t v = a ^ b;
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint16_t DO_EOR_W(uint16_t a, uint16_t b) {
    uint16_t v = a ^ b;
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint32_t DO_EOR_L(uint32_t a, uint32_t b) {
    uint32_t v = a ^ b;
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint8_t DO_ADD_B(uint8_t a, uint8_t b) {
    uint32_t v = a + b;
    regs.v = ((a & b) ^ v) >> 7 & 1;
    regs.x = regs.c = v >> 8 & 1;
    TEST_NZ8(v);
    return v;
}

inline uint16_t DO_ADD_W(uint16_t a, uint16_t b) {
    uint32_t v = a + b;
    regs.v = ((a & b) ^ v) >> 15 & 1;
    regs.x = regs.c = v >> 16 & 1;
    TEST_NZ16(v);
    return v;
}

inline uint32_t DO_ADD_L(uint32_t a, uint32_t b) {
    uint64_t v = (uint64_t)a + (uint64_t)b;
    regs.v = ((a & b) ^ v) >> 31 & 1;
    regs.x = regs.c = v >> 32 & 1;
    TEST_NZ32(v);
    return v;
}

inline uint8_t DO_SUB_B(uint8_t a, uint8_t b) {
    uint32_t v = a - b;
    regs.v = ((a & ~b) ^ v) >> 7 & 1;
    regs.x = regs.c = v >> 8 & 1;
    TEST_NZ8(v);
    return v;
}

inline uint16_t DO_SUB_W(uint16_t a, uint16_t b) {
    uint32_t v = a - b;
    regs.v = ((a & ~b) ^ v) >> 15 & 1;
    regs.x = regs.c = v >> 16 & 1;
    TEST_NZ16(v);
    return v;
}

inline uint32_t DO_SUB_L(uint32_t a, uint32_t b) {
    uint64_t v = (uint64_t)a - (uint64_t)b;
    regs.v = ((a & ~b) ^ v) >> 31 & 1;
    regs.x = regs.c = v >> 32 & 1;
    TEST_NZ32(v);
    return v;
}

inline void DO_CMP_B(uint8_t a, uint8_t b) {
    uint32_t v = a - b;
    regs.v = ((a & ~b) ^ v) >> 7 & 1;
    regs.c = v >> 8 & 1;
    TEST_NZ8(v);
}

inline void DO_CMP_W(uint16_t a, uint16_t b) {
    uint32_t v = a - b;
    regs.v = ((a & ~b) ^ v) >> 15 & 1;
    regs.c = v >> 16 & 1;
    TEST_NZ16(v);
}

inline void DO_CMP_L(uint32_t a, uint32_t b) {
    uint64_t v = (uint64_t)a - (uint64_t)b;
    regs.v = ((a & ~b) ^ v) >> 31 & 1;
    regs.c = v >> 32 & 1;
    TEST_NZ32(v);
}

inline uint8_t DO_NOT_B(uint8_t a) {
    uint8_t v = ~a;
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint16_t DO_NOT_W(uint16_t a) {
    uint16_t v = ~a;
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline uint32_t DO_NOT_L(uint32_t a) {
    uint32_t v = ~a;
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
    return v;
}

inline int8_t DO_ASR_B(int8_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ8(v);
        return v;
    }
    v >>= (sc - 1);
    regs.c = regs.x = v & 1;
    TEST_NZ8(v >>= 1);
    return v;
}

inline int16_t DO_ASR_W(int16_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ16(v);
        return v;
    }
    v >>= (sc - 1);
    regs.c = regs.x = v & 1;
    TEST_NZ16(v >>= 1);
    return v;
}

inline int32_t DO_ASR_L(int32_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ32(v);
        return v;
    }
    v >>= (sc - 1);
    regs.c = regs.x = v & 1;
    TEST_NZ32(v >>= 1);
    return v;
}

inline int8_t DO_ASL_B(int8_t v, uint8_t sc) {
    if(sc == 0) {
        regs.c = false;
        regs.v = false;
        TEST_NZ8(v);
        return v;
    }
    int16_t tv = static_cast<int16_t>(v) << sc;
    regs.c = regs.x = tv & 0x100;
    uint8_t out = (uint8_t)(tv >> 8);
    regs.v = out != 0 && out != 0xff;
    TEST_NZ8(tv);
    return tv;
}

inline int16_t DO_ASL_W(int16_t v, uint8_t sc) {
    if(sc == 0) {
        regs.c = false;
        regs.v = false;
        TEST_NZ16(v);
        return v;
    }
    int32_t tv = static_cast<int32_t>(v) << sc;
    regs.c = regs.x = tv & 0x10000;
    uint16_t out = (uint16_t)(tv >> 16);
    regs.v = out != 0 && out != 0xffff;
    TEST_NZ16(tv);
    return tv;
}

inline int32_t DO_ASL_L(int32_t v, uint8_t sc) {
    if(sc == 0) {
        regs.c = false;
        regs.v = false;
        TEST_NZ32(v);
        return v;
    }
    int64_t tv = static_cast<int64_t>(v) << sc;
    regs.c = regs.x = tv & 0x100000000;
    uint32_t out = (uint32_t)(tv >> 32);
    regs.v = out != 0 && out != 0xffffffff;
    TEST_NZ32(tv);
    return tv;
}

inline uint8_t DO_LSR_B(uint8_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ8(v);
        return v;
    }
    v >>= (sc - 1);
    regs.c = regs.x = v & 1;
    TEST_NZ8(v >>= 1);
    return v;
}

inline uint16_t DO_LSR_W(uint16_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ16(v);
        return v;
    }
    v >>= (sc - 1);
    regs.c = regs.x = v & 1;
    TEST_NZ16(v >>= 1);
    return v;
}

inline uint32_t DO_LSR_L(uint32_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ32(v);
        return v;
    }
    v >>= (sc - 1);
    regs.c = regs.x = v & 1;
    TEST_NZ32(v >>= 1);
    return v;
}

inline uint8_t DO_LSL_B(uint8_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ8(v);
        return v;
    }
    uint16_t tv = static_cast<uint16_t>(v) << sc;
    regs.c = regs.x = tv & 0x100;
    TEST_NZ8(tv);
    return tv;
}

inline uint16_t DO_LSL_W(uint16_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ16(v);
        return v;
    }
    uint32_t tv = static_cast<uint32_t>(v) << sc;
    regs.c = regs.x = tv & 0x10000;
    TEST_NZ16(tv);
    return tv;
}

inline uint32_t DO_LSL_L(uint32_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ32(v);
        return v;
    }
    uint64_t tv = static_cast<uint64_t>(v) << sc;
    regs.c = regs.x = tv & 0x100000000;
    TEST_NZ32(tv);
    return tv;
}

inline uint8_t DO_ROXR_B(uint8_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = regs.x;
        TEST_NZ8(v);
        return v;
    }
    regs.c = v >> (sc - 1) & 1;
    uint8_t vv = v >> sc | regs.x << ( 8 - sc) | v << (9 - sc);
    regs.x = regs.c;
    TEST_NZ8(vv);
    return vv;
}

inline uint16_t DO_ROXR_W(uint16_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = regs.x;
        TEST_NZ16(v);
        return v;
    }
    regs.c = v >> (sc - 1) & 1;
    uint16_t vv = v >> sc | regs.x << ( 16 - sc) | v << (17 - sc);
    regs.x = regs.c;
    TEST_NZ16(vv);
    return vv;
}

inline uint32_t DO_ROXR_L(uint32_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = regs.x;
        TEST_NZ32(v);
        return v;
    }
    regs.c = v >> (sc - 1) & 1;
    uint32_t vv = v >> sc | regs.x << ( 32 - sc) | v << (33 - sc);
    regs.x = regs.c;
    TEST_NZ32(vv);
    return vv;
}

inline uint8_t DO_ROXL_B(uint8_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = regs.x;
        TEST_NZ8(v);
        return v;
    }
    regs.c = v >> (8-sc) & 1;
    uint8_t vv = v << sc | regs.x << (sc-1) | v >> (9 - sc);
    regs.x = regs.c;
    TEST_NZ8(vv);
    return vv;
}

inline uint16_t DO_ROXL_W(uint16_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = regs.x;
        TEST_NZ16(v);
        return v;
    }
    regs.c = v >> (16-sc) & 1;
    uint16_t vv = v << sc | regs.x << (sc-1) | v >> (17 - sc);
    regs.x = regs.c;
    TEST_NZ16(vv);
    return vv;
}

inline uint32_t DO_ROXL_L(uint32_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = regs.x;
        TEST_NZ32(v);
        return v;
    }
    regs.c = v >> (32-sc) & 1;
    uint32_t vv = v << sc | regs.x << (sc-1) | v >> (33 - sc);
    regs.x = regs.c;
    TEST_NZ32(vv);
    return vv;
}

inline uint8_t DO_ROR_B(uint8_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ8(v);
        return v;
    }
    uint8_t vv = v >> sc | v << (8 - sc);
    TEST_NZ8(vv);
    regs.c = regs.n;
    return vv;
}

inline uint16_t DO_ROR_W(uint16_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ16(v);
        return v;
    }
    uint16_t vv = v >> sc | v << (16 - sc);
    TEST_NZ16(vv);
    regs.c = regs.n;
    return vv;
}

inline uint32_t DO_ROR_L(uint32_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ32(v);
        return v;
    }
    uint32_t vv = v >> sc | v << (32 - sc);
    TEST_NZ32(vv);
    regs.c = regs.n;
    return vv;
}

inline uint8_t DO_ROL_B(uint8_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ8(v);
        return v;
    }
    uint8_t vv = v << sc | v >> (8 - sc);
    regs.c = vv & 1;
    TEST_NZ8(vv);
    return vv;
}

inline uint16_t DO_ROL_W(uint16_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ16(v);
        return v;
    }
    uint16_t vv = v << sc | v >> (16 - sc);
    regs.c = vv & 1;
    TEST_NZ16(vv);
    return vv;
}

inline uint32_t DO_ROL_L(uint32_t v, uint8_t sc) {
    regs.v = false;
    if(sc == 0) {
        regs.c = false;
        TEST_NZ32(v);
        return v;
    }
    uint32_t vv = v << sc | v >> (32 - sc);
    regs.c = vv & 1;
    TEST_NZ32(vv);
    return vv;
}
