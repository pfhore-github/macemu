#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
extern std::unordered_map<std::string, YAML::Node> tests;
BOOST_AUTO_TEST_SUITE(CPU) 
BOOST_AUTO_TEST_CASE(run) {
//    /*
    for(const auto &[k, v] : tests) {
        regs.exception = false;
        cpu_test(v);
    }
 //   */
//    cpu_test(tests["CHK2/WithDn/Long/under"]);
}

BOOST_AUTO_TEST_SUITE_END()