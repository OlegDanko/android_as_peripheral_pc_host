//#define BOOST_TEST_MODULE pc_host_IpAddrStrCodeVct
#include <boost/test/unit_test.hpp>
#include <IpAddrStringCodeCvt.hpp>

#include <unordered_set>
#include <cstdlib>

BOOST_AUTO_TEST_CASE(Test2)
{
    std::unordered_set<ip4> ips;
    std::unordered_set<std::string> strings;

    for(int i = 0; i < 100; i++) {
        ip4 ip;
        do {
            ip = ip4(rand(), rand(), rand(), rand(), rand());
        } while(ips.count(ip) != 0);
        ips.insert(ip);

        auto str = ip_to_str_code(ip);
        BOOST_CHECK_EQUAL(strings.count(str), 0);
        strings.insert(str);

        BOOST_CHECK_EQUAL(ip, str_code_to_ip(str).value());
    }
}
