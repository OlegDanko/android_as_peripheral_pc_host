#include <ip4.hpp>


//auto ip4::operator<=>(const ip4& other) const {
//        for(auto [t, o] : std::views::zip(addr, other.addr)) {
//        if(auto cmp = t <=> o; cmp != 0)
//            return cmp;
//    }
//}
auto cmp(const ip4& a, const ip4& b) {
    for(std::size_t i = 0; i < a.addr.size(); i++) {
        if(auto cmp = a.addr[i] <=> b.addr[i]; cmp != 0)
            return cmp;
    }
    return a.port <=> b.port;
}

std::ostream &operator<<(std::ostream &str, const ip4 &ip) {
    return str << (int)ip.addr[0] << "."
                                  << (int)ip.addr[1] << "."
                                  << (int)ip.addr[2] << "."
                                  << (int)ip.addr[3] << ":"
                                  << ip.port;
}

size_t std::hash<ip4>::operator()(const ip4& ip) const {
    union {
        ip4 ip;
        size_t s;
    } u{.ip = ip};
    return u.s;
}

ip4::ip4(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint16_t port)
    : addr{a0, a1, a2, a3}
    , port(port) {}

bool ip4::operator==(const ip4 &other) const {
    return cmp(*this, other) == 0;
}

bool ip4::operator!=(const ip4 &other) const {
    return cmp(*this, other) != 0;
}
