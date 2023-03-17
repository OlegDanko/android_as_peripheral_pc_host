#pragma once

#include <array>
#include <cstdint>
#include <compare>
#include <ostream>
//#include <functional>

struct ip4 {
    std::array<uint8_t, 4> addr;
    uint16_t port;

    ip4() = default;
    ip4(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t);

//    auto operator<=>(const ip4& other) const;
    bool operator==(const ip4& other) const;
    bool operator!=(const ip4& other) const;
};

std::ostream& operator<<(std::ostream& str, const ip4& ip);

namespace std {
template<>
struct hash<ip4> {
    size_t operator()(const ip4& ip) const;
};
}
