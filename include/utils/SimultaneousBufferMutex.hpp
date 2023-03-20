#pragma once

#include <mutex>
#include <condition_variable>

/**
 * @brief The MutexBuffer class a buffer that allows to simultaneously write into and read from the same buffer
 * Use with caution!
 * Only one reading thread and one writing thread should be used
 * If reading thread is blocked by read function, write a dummy object into the buffer
 * If writing thread is blocked by write function, read an object from the buffer
 */
template<typename T, size_t size>
class SimultaneousBufferMutex {
    static_assert(size > 1);
    size_t read_position{0};
    size_t write_position{0};
    size_t count{0};

    std::mutex mtx;
    std::condition_variable cv;

    std::array<T, size> buf;
public:
    using val_t = T;
    void write(const T& val) {
        if(std::unique_lock lk(mtx); true) {
            cv.wait(lk, [this](){ return count != size; });
        }

        buf.at(write_position++) = val;
        write_position %= size;

        if(std::unique_lock lk(mtx); true) {
            ++count;
            cv.notify_one();
        }
    }
    val_t read() {
        if(std::unique_lock lk(mtx); true) {
            cv.wait(lk, [this](){ return count != 0; });
        }
        val_t val = buf.at(read_position++);
        read_position %= size;
        if(std::unique_lock lk(mtx); true) {
            --count;
            cv.notify_one();
        }
        return val;
    }
    void unlock_read() {
        if(std::unique_lock lk(mtx); true) {
            if(count > 0) return;
            ++count;
            cv.notify_one();
        }
    }
    void unlock_write() {
        if(std::unique_lock lk(mtx); true) {
            if(count < size) return;
            --count;
            cv.notify_one();
        }
    }
};
