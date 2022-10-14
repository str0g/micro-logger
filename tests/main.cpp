#include <iostream>
#include <thread>
#include "micro_logger.h"
#include "micro_logger_tools.h"

void msg_debug() {
    MSG_DEBUG("hello world");
}

void msg_null() {
    MSG_DEBUG("%s", nullptr);
}

void threads() {
    std::thread th1([&](){
        auto tid = std::this_thread::get_id();
        auto tid_to_text = micro_logger::bytes_to_hex(reinterpret_cast<const uint8_t*>(&tid), sizeof(tid));
        MSG_DEBUG("hello %s", tid_to_text.c_str());
    });
    std::thread th2([&](){
        auto tid = std::this_thread::get_id();
        auto tid_to_text = micro_logger::bytes_to_hex(reinterpret_cast<const uint8_t*>(&tid), sizeof(tid));
        MSG_DEBUG("world %s", tid_to_text.c_str());
    });
    th1.join();
    th2.join();
}

int main() {
    std::cout << "hello world" << std::endl;

    msg_debug();
    msg_null();
    threads();

    return 0;
}
