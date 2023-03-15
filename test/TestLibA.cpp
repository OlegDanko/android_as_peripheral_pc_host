#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>
#include <LibA.hpp>

BOOST_AUTO_TEST_CASE(lib_a_add) {
    LibA lib_a;
    BOOST_CHECK_EQUAL(lib_a.add(1, 2), 3);
}
