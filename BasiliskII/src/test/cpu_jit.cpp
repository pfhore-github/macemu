#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
extern std::unordered_map<std::string, YAML::Node> tests;
BOOST_AUTO_TEST_SUITE(JIT) 
BOOST_AUTO_TEST_CASE(run) {
//    /*
    for(const auto &[k, v] : tests) {
        cpu_test(v, true);
    }
//    */
//   cpu_test(tests["SCC/HI/F/1"], true);
}

BOOST_AUTO_TEST_SUITE_END()