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
