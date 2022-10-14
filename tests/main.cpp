#include <iostream>
#include <thread>
#include <unordered_map>
#include "micro_logger.h"
#include "micro_logger_tools.h"

void msg_hello_world() {
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

int main(int argc, char **argv) {
    std::unordered_map<std::string, void(*)()> options;
    options["msg_hello_world"] = msg_hello_world;
    options["msg_null"] = msg_null;
    options["threads"] = threads;


    for(int i=1; i<argc; i++) {
        try {
            options.at(argv[i])();
        } catch (const std::out_of_range& e) {
            if (std::string("all") == argv[i]) {
                for (auto& option : options) {
                    option.second();
                }
                break;
            }
        }
    }

    return 0;
}
