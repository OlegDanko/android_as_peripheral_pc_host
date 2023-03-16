#include <IpAddrStringCodeCvt.hpp>
#include <bit_io/BitIOTypes.hpp>
#include <unordered_map>
#include <ranges>

constexpr std::array<uint8_t, 32> mk_chars_by_index_array() {
    std::array<uint8_t, 32> arr;
    size_t index{0};
    for(uint ch = '0'; ch <= '9'; ch++) {
        arr[index++] = ch;
    }
    for(uint8_t ch = 'A'; ch < 'W'; ch++) {
        arr[index++] = ch;
    }
    return arr;
}

using char_to_index_map_t = std::unordered_map<uint8_t, uint8_t>;
char_to_index_map_t mk_char_to_index_map() {
    char_to_index_map_t map;
    auto arr = mk_chars_by_index_array();

    namespace v = std::views;
    for(auto [i, val] : v::iota(0u, arr.size()) | v::transform([&arr](std::size_t index) {
                            return std::tuple{ index, arr[index] };
                        })) {
        map[val] = i;
    }

    return map;
}

uint8_t index_to_char(uint8_t i) {
    static const auto arr = mk_chars_by_index_array();
    return arr.at(i);
}
uint8_t char_to_index(uint8_t ch) {
    static const auto map = mk_char_to_index_map();
    return map.at(ch);
}


std::string ip_to_str_code(const ip4 &ip) {
    using namespace bit_io;
    buffer_t buffer;
    BitWriter bit_writer(buffer);

    bit_io::write_many(bit_writer,
                       type_t(ip.addr[0]),
            type_t(ip.addr[1]),
            type_t(ip.addr[2]),
            type_t(ip.addr[3]),
            type_t(ip.port));

    std::string str;
    BitReader bit_reader(buffer);
    for(int i = 0; i < 9; i++) {
        str.push_back(index_to_char(type_t<uint8_t, 5>(bit_reader)));
    }
    str.push_back(index_to_char(type_t<uint8_t, 3>(bit_reader)));
    return str;
}

std::optional<ip4> str_code_to_ip(const std::string &str) {
    if(str.size() != 10)
        return {};

    using namespace bit_io;
    buffer_t buffer;
    BitWriter bit_writer(buffer);

    for (const auto& ch : str | std::views::take(str.size() - 1)) {
        auto index = char_to_index(ch);
        write_many(bit_writer, bit_io::type_t<u8, 5>(index));
    }
    write_many(bit_writer, bit_io::type_t<u8, 3>(char_to_index(str.back())));

    ip4 ip;
    BitReader bit_reader(buffer);
    for(auto& a : ip.addr) {
        a = type_t<u8>(bit_reader).val;
    }
    ip.port = type_t<u16>(bit_reader).val;
    return {ip};
}
