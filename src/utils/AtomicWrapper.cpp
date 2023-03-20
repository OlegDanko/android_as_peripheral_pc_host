#include <utils/AtomicWrapper.hpp>


template<>
void AtomicWrapper<int>::notify_one() {
    val.notify_one();
}
template<>
void AtomicWrapper<int>::notify_all() {
    val.notify_all();
}
template<>
void AtomicWrapper<int>::wait(int v) {
    val.wait(v);
}

template<>
void AtomicWrapper<bool>::notify_one() {
    val.notify_one();
}
template<>
void AtomicWrapper<bool>::notify_all() {
    val.notify_all();
}
template<>
void AtomicWrapper<bool>::wait(bool v) {
    val.wait(v);
}
