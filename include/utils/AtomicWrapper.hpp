#pragma once

#include <atomic>

// this is a wrapper to allow for the usage of C++20 atomic wait and notify in earlier versions
// should be redifined for each type
template<typename T>
struct AtomicWrapper {
    std::atomic<T> val;
    void notify_one();
    void notify_all();
    void wait(T val);
};

