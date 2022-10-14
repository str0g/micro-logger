#include <iostream>
#include <thread>
#include "micro_logger.h"

void msg_debug() {
    MSG_DEBUG("hello world");
}

void threads() {
    std::thread th1([&](){
        std::cout << std::this_thread::get_id() << std::endl;
        MSG_DEBUG("hello");
        MSG_DEBUG("1");
    });
    std::thread th2([&](){
        std::cout << std::this_thread::get_id() << std::endl;
        MSG_DEBUG("world");
        MSG_DEBUG("2");
    });
    th1.join();
    th2.join();
}

int main() {
    std::cout << "hello world" << std::endl;

    msg_debug();
    threads();

    std::cout << std::this_thread::get_id() << std::endl;

    return 0;
}
