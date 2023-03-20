#pragma once
#include <cstdint>

//template<template<typename, std::size_t> typename Buf_t, typename T, std::size_t SIZE>
//class SimultaneousBufferReader;

template<typename Buf_t>
class SimultaneousBufferReader{
    Buf_t& buf;
public:
    SimultaneousBufferReader(Buf_t& buffer) : buf(buffer) {}

    typename Buf_t::val_t read() {
        return buf.read();
    }
    void unlock() {
        buf.unlock_read();
    }
};
