#ifndef COMPILER_JIT_
#define COMPILER_JIT_
#include <stdint.h>
#include <unordered_map>
#include <functional>
struct JIT_OP {
    std::function<void()> exec;
    uint32_t nextop;
};
extern std::unordered_map<uint32_t, JIT_OP> compiled;

#endif