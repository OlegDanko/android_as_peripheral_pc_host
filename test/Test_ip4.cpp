//#define BOOST_TEST_MODULE pc_host_ip4
#include <boost/test/unit_test.hpp>

#include <ip4.hpp>

BOOST_AUTO_TEST_SUITE(ip4Tests)

BOOST_AUTO_TEST_CASE(ip4_cmp) {
    ip4 ip(10, 20, 30, 40, 50);

    BOOST_CHECK_EQUAL(ip, ip4(10, 20, 30, 40, 50));
    BOOST_CHECK_NE(ip, ip4(15, 20, 30, 40, 50));
    BOOST_CHECK_NE(ip, ip4(10, 25, 30, 40, 50));
    BOOST_CHECK_NE(ip, ip4(10, 20, 35, 40, 50));
    BOOST_CHECK_NE(ip, ip4(10, 20, 30, 45, 50));
    BOOST_CHECK_NE(ip, ip4(10, 20, 30, 40, 55));
}

BOOST_AUTO_TEST_SUITE_END()
