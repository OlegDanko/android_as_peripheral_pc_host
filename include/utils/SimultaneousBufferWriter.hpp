#pragma once
#include <cstdint>

template<typename Buf_t>
class SimultaneousBufferWriter {
    Buf_t& buf;
public:
    SimultaneousBufferWriter(Buf_t& buffer) : buf(buffer) {}

    void write(const typename Buf_t::val_t& val) {
        return buf.write(val);
    }
    void unlock() {
        buf.unlock_write();
    }
};
