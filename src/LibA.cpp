#include "../include/LibA.hpp"
#include <iostream>

void LibA::say_hello() {
    std::cout << "hello world" << std::endl;
}

int LibA::add(int a, int b) {
    return a+b;
}
