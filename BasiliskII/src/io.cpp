#include "mmu/mmu.hpp"
#include <unordered_map>
#include <string>
#include <stdio.h>
#include "via.hpp"
#include "asc.hpp"
#include "rbv.hpp"
#include "machine.hpp"
#include "exceptions.hpp"
// model id(settable from pref?)
std::unordered_map<std::string, uint16_t> model_ids;

IO_BASE::~IO_BASE() {}
